#include "Node.h"
#include "Sink.h"
#include "Ctrl.h"
//#include "Epidemic.h"
#include "SortHelper.h"
#include "FileHelper.h"
#include <typeinfo.h>
#include "Prophet.h"
#include "MacProtocol.h"
#include "Trace.h"
#include "PrintHelper.h"
#include "Configuration.h"

int CNode::COUNT_ID = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������

int CNode::encounterAtHotspot = 0;
//int CNode::encounterActiveAtHotspot = 0;
//int CNode::encounterActive = 0;
int CNode::encounter = 0;
int CNode::visiterAtHotspot = 0;
int CNode::visiter = 0;

vector<CNode*> CNode::nodes;
vector<CNode*> CNode::deadNodes;
vector<CNode *> CNode::deletedNodes;


void CNode::init() 
{
	trace = nullptr;
	dataRate = 0;
	atHotspot = nullptr;
	//timerCarrierSense = configs.mac.CYCLE_CARRIER_SENSE;
	//discovering = false;
	timeLastData = 0;
	timeDeath = 0;
	capacityBuffer = configs.node.CAPACITY_BUFFER;
	sumTimeAwake = 0;
	sumTimeAlive = 0;
	sumBufferRecord = 0;
	countBufferRecord = 0;
	dutyCycle = configs.mac.DUTY_RATE;
	SLOT_WAKE = int( dutyCycle * configs.mac.CYCLE_TOTAL );
	SLOT_SLEEP = configs.mac.CYCLE_TOTAL - SLOT_WAKE;
	timerCarrierSense = UNVALID;
	if( SLOT_WAKE == 0 )
	{
		state = _asleep;
		timerWake = UNVALID;
		if( configs.mac.SYNC_CYCLE )
			timerSleep = SLOT_SLEEP;
		else
			timerSleep = RandomInt(1, SLOT_SLEEP);
	}
	else
	{
		state = _awake;
		timerSleep = UNVALID;
		if( configs.mac.SYNC_CYCLE )
			timerWake = SLOT_WAKE;
		else
			timerWake = RandomInt(1, SLOT_WAKE);
		timerCarrierSense = RandomInt(0, CMacProtocol::getMaxTransmissionDelay());
	}
}

CNode::CNode() 
{
	throw string("CNode::CNode() : Default constructor is disabled.");
}

CNode::CNode(double dataRate) 
{
	init();
	this->dataRate = dataRate;
}

CNode::~CNode()
{
	if( trace != nullptr )
	{
		delete trace;
		trace = nullptr;
	}
}

void CNode::initNodes() {
	if( nodes.empty() && deadNodes.empty() )
	{
		vector<string> filenames = CFileHelper::ListDirectory(configs.trace.PATH_TRACE);
		filenames = CFileHelper::FilterByExtension(filenames, configs.trace.EXTENSION_TRACE);

		if( filenames.empty() )
			throw string("CNode::initNodes(): Cannot find any trace files under \"" + configs.trace.PATH_TRACE
						  + "\".");

		for(int i = 0; i < filenames.size(); ++i)
		{
			double dataRate = configs.node.DEFAULT_DATA_RATE;
			if(i % 5 == 0)
				dataRate *= 5;
			CNode* node = new CNode(dataRate);
			node->generateID();
			node->loadTrace(filenames[i]);
			CNode::nodes.push_back(node);
		}
	}
}

void CNode::generateData(int currentTime) {
	int timeDataIncre = currentTime - timeLastData;
	int nData = int( timeDataIncre * dataRate / configs.data.SIZE_DATA);
	if(nData > 0)
	{
		for(int i = 0; i < nData; ++i)
		{
			CData data(ID, currentTime, configs.data.SIZE_DATA);
			this->buffer = CSortHelper::insertIntoSortedList(this->buffer, data, CSortHelper::ascendByTimeBirth, CSortHelper::descendByTimeBirth);
		}
		timeLastData = currentTime;
	}
	updateBufferStatus(currentTime);
}


vector<CNode*>& CNode::getNodes() 
{
	if( configs.mac.CYCLE_TOTAL == 0 || ( ZERO( configs.mac.DUTY_RATE ) && ZERO( configs.hdc.HOTSPOT_DUTY_RATE ) ) )
	{
		throw string("CNode::getNodes() : configs.mac.CYCLE_TOTAL || ( configs.mac.DUTY_RATE && configs.hdc.HOTSPOT_DUTY_RATE ) = 0");
	}

	if( nodes.empty() && deadNodes.empty() )
		initNodes();
	return nodes;
}

int CNode::getNodeCount() 
{
	return nodes.size();
}

vector<CNode *> CNode::getAllNodes(bool sort)
{
	vector<CNode *> allNodes = CNode::getNodes();
	allNodes.insert(allNodes.end(), deadNodes.begin(), deadNodes.end());
	allNodes.insert(allNodes.end(), deletedNodes.begin(), deletedNodes.end());
	if( sort )
		allNodes = CSortHelper::mergeSort(allNodes, CSortHelper::ascendByID);
	return allNodes;
}

vector<int> CNode::getIdNodes() 
{
	if( nodes.empty() && deadNodes.empty() )
		initNodes();
	vector<int> ids;
	for( CNode * inode : nodes )
		ids.push_back(inode->getID());
	return ids;
}

bool CNode::finiteEnergy() 
{
	return configs.node.CAPACITY_ENERGY > 0;
}

bool CNode::hasNodes(int currentTime) 
{
	if( nodes.empty() && deadNodes.empty() )
	{
		initNodes();
		return true;
	}
		
	bool death = ClearDeadNodes(currentTime);
	if(death)
		CPrintHelper::PrintAttribute("Node", CNode::getNodes().size());

	return ( ! nodes.empty() );
}

bool CNode::ClearDeadNodes(int currentTime) 
{
	bool death = false;
	for(vector<CNode *>::iterator ipNode = nodes.begin(); ipNode != nodes.end(); )
	{
		if(! (*ipNode)->isAlive())
		{
			death = true;
			deadNodes.push_back( *ipNode );
			ipNode = nodes.erase( ipNode );
		}
		else
			++ipNode;
	}

	// TODO: more detailed feedback
	if( death )
	{
		ofstream death(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_DEATH, ios::app);
		if( currentTime == 0 )
		{
			death << endl << configs.log.INFO_LOG << endl;
			death << configs.log.INFO_DEATH;
		}
		death << currentTime << TAB << CNode::getAllNodes(false).size() - CNode::getNodeCount()
			<< TAB << CData::getCountDelivery() << TAB << CData::getDeliveryRatio() << endl;
		death.close();
	}
	return death;
}

bool CNode::ifNodeExists(int id) 
{
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		if((*inode)->getID() == id)
			return true;
	}
	return false;
}

CNode* CNode::getNodeByID(int id) 
{
	for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		if((*inode)->getID() == id)
			return *inode;
	}
	return nullptr;
}

double CNode::getSumEnergyConsumption() 
{
	double sumEnergyConsumption = 0;
	auto allNodes = getAllNodes(false);
	for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		sumEnergyConsumption += (*inode)->getEnergyConsumption();
	return sumEnergyConsumption;
}

void CNode::raiseDutyCycle() 
{
	if( useHotspotDutyCycle() )
		return;

	dutyCycle = configs.hdc.HOTSPOT_DUTY_RATE;
	int oldSlotWake = SLOT_WAKE;
	SLOT_WAKE = int( configs.mac.CYCLE_TOTAL * dutyCycle );
	SLOT_SLEEP = configs.mac.CYCLE_TOTAL - SLOT_WAKE;
	//����״̬�£��ӳ�����ʱ��
	if( isAwake() )
		timerWake += SLOT_WAKE - oldSlotWake;
	//����״̬�£���������
	else
		Wake();
}

void CNode::resetDutyCycle() 
{
	if( useDefaultDutyCycle() )
		return;

	dutyCycle = configs.hdc.HOTSPOT_DUTY_RATE;
	int oldSlotWake = SLOT_WAKE;
	SLOT_WAKE = int(configs.mac.CYCLE_TOTAL * dutyCycle);
	SLOT_SLEEP = configs.mac.CYCLE_TOTAL - SLOT_WAKE;
}

void CNode::checkDataByAck(vector<CData> ack)
{
	if( configs.node.SCHEME_FORWARD == config::_dump )
		RemoveFromList(buffer, ack);
}

void CNode::Overhear(int currentTime)
{
	CGeneralNode::Overhear(currentTime);

	//�����ز�����
	//ʱ�䴰�����ֵ / �������ߣ�
	delayDiscovering( CMacProtocol::getMaxTransmissionDelay() );
}

void CNode::Wake()
{
	//Always On
	if( SLOT_WAKE <= 0 )
	{
		Sleep();
		return;
	}
	state = _awake;
	timerSleep = UNVALID;
	timerWake = SLOT_WAKE;
	timerCarrierSense = RandomInt(0, CMacProtocol::getMaxTransmissionDelay());
}

void CNode::Sleep()
{
	//Always On
	if( SLOT_SLEEP <= 0 )
	{
		Wake();
		return;
	}
	state = _asleep;
	timerWake = UNVALID;
	timerCarrierSense = UNVALID;
	timerSleep = SLOT_SLEEP;
}

CFrame* CNode::sendRTSWithCapacityAndPred(int currentTime)
{
	vector<CPacket*> packets;
	if( configs.node.SCHEME_RELAY == config::_selfish
		&& ( ! buffer.empty() ) )
		packets.push_back( new CCtrl(ID, capacityBuffer - buffer.size(), currentTime, configs.data.SIZE_CTRL, CCtrl::_capacity) );
	packets.push_back( new CCtrl(ID, currentTime, configs.data.SIZE_CTRL, CCtrl::_rts) );
	packets.push_back( new CCtrl(ID, deliveryPreds, currentTime, configs.data.SIZE_CTRL, CCtrl::_index) );
	CFrame* frame = new CFrame(*this, packets);

	return frame;	
}

bool CNode::hasSpokenRecently(CNode* node, int currentTime) 
{
	if( configs.node.LIFETIME_SPOKEN_CACHE == 0 )
		return false;

	map<CNode *, int>::iterator icache = spokenCache.find( node );
	if( icache == spokenCache.end() )
		return false;
	else if( ( currentTime - icache->second ) == 0
		|| ( currentTime - icache->second ) < configs.node.LIFETIME_SPOKEN_CACHE )
		return true;
	else
		return false;
}

void CNode::addToSpokenCache(CNode* node, int currentTime) 
{
	spokenCache.insert( pair<CNode*, int>(node, currentTime) );
}

//vector<CData> CNode::dropOverdueData(int currentTime) {
//	if( buffer.empty() )
//		return vector<CData>();
////	if( ! CData::useTTL() )
////		return vector<CData>();
//
//	vector<CData> overflow;
//	for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
//	{
//		idata->updateStatus(currentTime);
//		//���TTL���ڣ�����
//		if( idata->isOverdue() )
//		{
//			overflow.push_back( *idata );
//			idata = buffer.erase( idata );
//		}
//		else
//			++idata;
//	}
//	return overflow;
//}


//�ֶ�������ѹ�� buffer�������������κβ���
//ע�⣺�����ڵ��ô˺���֮���ֶ����� updateBufferStatus() ������

void CNode::pushIntoBuffer(vector<CData> datas)
{
	this->buffer = CSortHelper::insertIntoSortedList(this->buffer, datas, CSortHelper::ascendByTimeBirth, CSortHelper::descendByTimeBirth);
}

vector<CData> CNode::removeDataByCapacity(vector<CData> &datas, int capacity, bool fromLeft)
{
	vector<CData> overflow;
	if( capacity <= 0
	   || datas.size() <= capacity )
		return overflow;

	if( fromLeft )
	{
		overflow = vector<CData>(datas.begin(), datas.begin() + capacity);
		datas = vector<CData>(datas.begin() + capacity, datas.end());
	}
	else
	{
		datas = vector<CData>(datas.begin(), datas.begin() + capacity);
		overflow = vector<CData>(datas.begin() + capacity, datas.end());
	}

	return overflow;
}

vector<CData> CNode::dropDataIfOverflow()
{
	if( buffer.empty() )
		return vector<CData>();

	vector<CData> myData;
	vector<CData> overflow;

//	if( CEpidemic::MAX_DATA_RELAY > 0 )
//	{
//		vector<CData> otherData;
//		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); ++idata)
//		{
//			if( idata->getNode() == this->ID )
//				myData.push_back( *idata );
//			else
//				otherData.push_back( *idata );
//		}
//		otherData = CSortHelper::mergeSort(otherData, CSortHelper::ascendByTimeBirth);
//		myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByTimeBirth);
//		//�������MAX_DATA_RELAY
//		if(otherData.size() > CEpidemic::MAX_DATA_RELAY)
//			otherData = vector<CData>( otherData.end() - CEpidemic::MAX_DATA_RELAY, otherData.end() );
//		myData.insert( myData.begin(), otherData.begin(), otherData.end() );
//		//����ܳ������
//		if( myData.size() > capacityBuffer )
//		{
//			//flash_cout << "######  ( Node " << this->ID << " drops " << myData.size() - capacityBuffer << " data )                    " << endl;
//			myData = vector<CData>( myData.end() - configs.node.CAPACITY_BUFFER, myData.end() );
//		}		
//	}
//	else :

	myData = buffer;
	//myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByTimeBirth);
	overflow = removeDataByCapacity(myData, capacityBuffer, true);

	buffer = myData;
	return overflow;
}

vector<CData> CNode::updateBufferStatus(int currentTime) 
{
	vector<CData> overflow = dropDataIfOverflow();

	return overflow;
}

vector<int> CNode::updateSummaryVector() 
{
	if( buffer.size() > configs.node.CAPACITY_BUFFER )
	{
		throw string("CNode::updateSummaryVector() : Buffer isn't clean !");
	}

	summaryVector.clear();
	for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); ++idata)
	{

		if( CData::useHOP() && ( ! idata->allowForward() ) )
			continue;
		else
			summaryVector.push_back( idata->getID() );
	}

	return summaryVector;
}

void CNode::restoreNodes(int n) 
{
	// TODO: �ָ�ʱ���³���������
	for(int i = nodes.size(); i < nodes.size() + n; ++i)
	{
		if( deletedNodes.empty() )
			break;

		CNode::nodes.push_back(deletedNodes[0]);
		--n;
	}
}

void CNode::removeNodes(int n) 
{
	//FIXME: Random selected ?
	vector<CNode *>::iterator start = nodes.begin();
	vector<CNode *>::iterator end = nodes.end();
	vector<CNode *>::iterator fence = nodes.begin();
	fence += nodes.size() - n;
	vector<CNode *> leftNodes(start, fence);

	//Remove invalid positoins belonging to the deleted nodes
	vector<CNode *> deletedNodes(fence, end);
	vector<int> deletedIDs;
	for(auto inode = deletedNodes.begin(); inode != deletedNodes.end(); ++inode)
		deletedIDs.push_back( (*inode)->getID() );

	for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
	{
		if( IfExists(deletedIDs, (*ipos)->getNode()) )
			ipos = CPosition::positions.erase(ipos);
		else
			++ipos;
	}

	nodes = leftNodes;
	CNode::deletedNodes.insert(CNode::deletedNodes.end(), deletedNodes.begin(), deletedNodes.end());
}

void CNode::updateStatus(int currentTime)
{
	if( this->time == currentTime )
		return;

	int newTime = this->time;

	//�����ܺġ����¹���״̬
	while( ( newTime + configs.simulation.SLOT ) <= currentTime )
	{
		newTime += configs.simulation.SLOT;

		// 0 ʱ��ʱ��ʼ��
		if( newTime <= 0 )
		{
			newTime = 0;
			this->time = 0;
			break;
		}

		if( timerSleep <= 0
		   && timerWake <= 0 )
		{
			throw string("CNode::updateStatus() : timerSleep : " + STRING(timerSleep) + ", timerWake : " + STRING(timerWake) );
		}

		switch( state )
		{
			case _awake:
				consumeEnergy(configs.trans.CONSUMPTION_WAKE * configs.simulation.SLOT, currentTime);
				updateTimerWake(newTime);

				break;
			case _asleep:
				consumeEnergy(configs.trans.CONSUMPTION_SLEEP * configs.simulation.SLOT, currentTime);
				updateTimerSleep(newTime);

				break;
			default:
				break;
		}

		//updateTimerOccupied(newTime);

	}

	if( !isAlive() )
		return;

	sumTimeAlive += newTime - this->time;

	//��������
	if( currentTime <= configs.simulation.DATATIME )
		generateData(currentTime);


	/**************************************  Prophet  *************************************/
	if( configs.ROUTING_PROTOCOL == config::_prophet 
		 && (currentTime % configs.trace.SLOT_TRACE) == 0  )
		CProphet::decayDeliveryPreds(this, currentTime);

	//�������꼰ʱ���
	if( ! trace->isValid(currentTime) )
	{
		die(currentTime);  //�� trace ��Ϣ��ֹ�������Ľڵ㣬�޷�����
		return;
	}
	setLocation(trace->getLocation(currentTime));
	setTime(currentTime);

}

void CNode::updateTimerWake(int time)
{
	if( state != _awake )
		return;

	int incr = time - this->time;
	if( incr <= 0 )
		return;

	timerWake -= incr;
	sumTimeAwake += incr;

	if( timerCarrierSense > 0 )
	{
		timerCarrierSense -= incr;
	}
	if( timerWake == 0 )
		Sleep();

	if( timerCarrierSense == 0 )
		startDiscovering();
	if( SLOT_SLEEP <= 0 )
		startDiscovering();
}

void CNode::updateTimerSleep(int time)
{
	if( state != _asleep )
		return;

	int incr = time - this->time;
	if( incr <= 0 )
		return;

	timerSleep -= incr;

	if( timerSleep == 0 )
		Wake();
}

void CNode::recordBufferStatus() 
{
	if( timeDeath > 0 )
		return;

	sumBufferRecord += buffer.size();
	++countBufferRecord;
}

vector<CData> CNode::getDataByRequestList(vector<int> requestList) const
{
	if( requestList.empty() || buffer.empty() )
		return vector<CData>();

	vector<CData> result;
	result = CData::GetItemsByID(buffer, requestList);
	return result;
}

int CNode::getCapacityForward()
{
	int capacity = capacityBuffer - buffer.size();
	if( capacity < 0 )
		capacity = 0;

	if( configs.node.SCHEME_RELAY == config::_selfish )
		return capacity;
	else if( configs.node.SCHEME_RELAY == config::_loose )
		return capacityBuffer;
	else
		return 0;
}
