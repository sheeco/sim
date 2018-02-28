#include "Node.h"
#include "Sink.h"
#include "Ctrl.h"
#include "SortHelper.h"
#include "FileHelper.h"
#include <typeinfo.h>
#include "Prophet.h"
#include "MacProtocol.h"
#include "Trace.h"
#include "PrintHelper.h"
#include "Configuration.h"

int CNode::COUNT_ID = 0;  //从1开始，数值等于当前实例总数

int CNode::encounterAtWaypoint = 0;
//int CNode::encounterActiveAtHotspot = 0;
//int CNode::encounterActive = 0;
int CNode::encounter = 0;
int CNode::visiterAtWaypoint = 0;
int CNode::visiter = 0;

vector<CNode*> CNode::nodes;
vector<CNode*> CNode::deadNodes;
vector<CNode *> CNode::deletedNodes;


void CNode::init() 
{
	trace = nullptr;
	dataRate = 0;
	atHotspot = nullptr;
	//timerCarrierSense = getConfig<int>("mac", "cycle_carrier_sense");
	//discovering = false;
	timeLastData = 0;
	timeDeath = 0;
	capacityBuffer = getConfig<int>("node", "buffer");
	sumTimeAwake = 0;
	sumTimeAlive = 0;
	sumBufferRecord = 0;
	countBufferRecord = 0;
	dutyCycle = getConfig<double>("mac", "duty_rate");
	SLOT_WAKE = int( dutyCycle * getConfig<int>("mac", "cycle") );
	SLOT_SLEEP = getConfig<int>("mac", "cycle") - SLOT_WAKE;
	timerCarrierSense = INVALID;
	discovering = false;
	if( SLOT_WAKE == 0 )
	{
		state = _asleep;
		timerWake = INVALID;
		if( getConfig<bool>("mac", "sync_cycle") )
			timerSleep = SLOT_SLEEP;
		else
			timerSleep = RandomInt(1, SLOT_SLEEP);
	}
	else
	{
		state = _awake;
		timerSleep = INVALID;
		if( getConfig<bool>("mac", "sync_cycle") )
			timerWake = SLOT_WAKE;
		else
			timerWake = RandomInt(1, SLOT_WAKE);
		timerCarrierSense = RandomInt(0, CMacProtocol::getMaxTransmissionDelay());
	}
}

CNode::CNode() 
{
	init();
}

CNode::CNode(double dataRate) 
{
	init();
	this->setDataByteRate(dataRate);
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
		vector<string> filepaths = CFileHelper::ListDirectory(getConfig<string>("trace", "path"));
		filepaths = CFileHelper::FilterByExtension(filepaths, getConfig<string>("trace", "extension_trace_file"));

		if( filepaths.empty() )
			throw string("CNode::initNodes(): Cannot find any trace files under \"" + getConfig<string>("trace", "path")
						  + "\".");

		for(int i = 0; i < filepaths.size(); ++i)
		{
			double dataRate = getConfig<double>("node", "default_data_rate");
			if(i % 5 == 0)
				dataRate *= 5;
			CNode* node = new CNode(dataRate);
			node->generateID();
			node->loadTrace(filepaths[i]);
			CNode::nodes.push_back(node);
		}
	}
}

void CNode::generateData(int now) {
	int timeDataIncre = now - timeLastData;
	int nData = int( timeDataIncre * dataRate / getConfig<int>("data", "size_data"));
	if(nData > 0)
	{
		for(int i = 0; i < nData; ++i)
		{
			CData data(ID, now, getConfig<int>("data", "size_data"));
			this->buffer = CSortHelper::insertIntoSortedList(this->buffer, data, CSortHelper::ascendByTimeBirth, CSortHelper::descendByTimeBirth);
		}
		timeLastData = now;
	}
	updateBufferStatus(now);
}


vector<CNode*>& CNode::getNodes() 
{
	if( getConfig<int>("mac", "cycle") == 0 || ( ZERO( getConfig<double>("mac", "duty_rate") ) && ZERO( getConfig<double>("hdc", "hotspot_duty_rate") ) ) )
	{
		throw string("CNode::getNodes() : cycle_total = " + STRING(getConfig<int>("mac", "cycle")) + ", duty_rate = " + STRING(getConfig<double>("mac", "duty_rate")) + ", hotspot_duty_rate = " + STRING(getConfig<double>("hdc", "hotspot_duty_rate")) + ".");
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
	return getConfig<int>("node", "energy") > 0;
}

bool CNode::hasNodes(int now) 
{
	if( nodes.empty() && deadNodes.empty() )
	{
		initNodes();
		return true;
	}
		
	return ( ! nodes.empty() );
}

bool CNode::ClearDeadNodes(int now) 
{
	return ClearDeadNodes(nodes, deadNodes, now);
}

// TODO: change to the new implementation below

inline bool CNode::ClearDeadNodes(vector<CNode*>& aliveList, vector<CNode*>& deadList, int now)
{
	bool death = false;
	for( vector<CNode *>::iterator ipNode = aliveList.begin(); ipNode != aliveList.end(); )
	{
		if( !( *ipNode )->isAlive() )
		{
			death = true;
			deadNodes.push_back(*ipNode);
			ipNode = aliveList.erase(ipNode);
		}
		else
			++ipNode;
	}

	// TODO: more detailed feedback
	if( death )
	{
		ofstream death(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_death"), ios::app);
		if( now == 0 )
		{
			death << endl << getConfig<string>("log", "info_log") << endl;
			death << getConfig<string>("log", "info_death") << endl;
		}
		death << now << TAB << aliveList.size() << TAB << CData::getCountDelivery()
			<< TAB << CData::getDeliveryRatio() << endl;
		death.close();

		CPrintHelper::PrintAttribute("Node Count", aliveList.size());
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

	dutyCycle = getConfig<double>("hdc", "hotspot_duty_rate");
	int oldSlotWake = SLOT_WAKE;
	SLOT_WAKE = int( getConfig<int>("mac", "cycle") * dutyCycle );
	SLOT_SLEEP = getConfig<int>("mac", "cycle") - SLOT_WAKE;
	//唤醒状态下，延长唤醒时间
	if( isAwake() )
		timerWake += SLOT_WAKE - oldSlotWake;
	//休眠状态下，立即唤醒
	else
		Wake();
}

void CNode::resetDutyCycle() 
{
	if( useDefaultDutyCycle() )
		return;

	dutyCycle = getConfig<double>("hdc", "hotspot_duty_rate");
	int oldSlotWake = SLOT_WAKE;
	SLOT_WAKE = int(getConfig<int>("mac", "cycle") * dutyCycle);
	SLOT_SLEEP = getConfig<int>("mac", "cycle") - SLOT_WAKE;
}

void CNode::checkDataByAck(vector<CData> ack)
{
	if( getConfig<CConfiguration::EnumForwardScheme>("node", "scheme_forward") == config::_dump )
		RemoveFromList(buffer, ack);
}

void CNode::Overhear(int now)
{
	CGeneralNode::Overhear(now);

	//继续载波侦听
	//时间窗内随机值 / 立即休眠？
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
	timerSleep = INVALID;
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
	timerWake = INVALID;
	timerCarrierSense = INVALID;
	timerSleep = SLOT_SLEEP;
}

CFrame* CNode::sendRTSWithCapacityAndPred(int now)
{
	vector<CPacket*> packets;
	if( getConfig<CConfiguration::EnumRelayScheme>("node", "scheme_relay") == config::_selfish
		&& ( ! buffer.empty() ) )
		packets.push_back( new CCtrl(ID, capacityBuffer - buffer.size(), now, getConfig<int>("data", "size_ctrl"), CCtrl::_capacity) );
	packets.push_back( new CCtrl(ID, now, getConfig<int>("data", "size_ctrl"), CCtrl::_rts) );
	packets.push_back( new CCtrl(ID, deliveryPreds, now, getConfig<int>("data", "size_ctrl"), CCtrl::_index) );
	CFrame* frame = new CFrame(*this, packets);

	return frame;	
}

bool CNode::hasSpokenRecently(CNode* node, int now) 
{
	if( getConfig<int>("node", "lifetime_spoken_cache") == 0 )
		return false;

	map<CNode *, int>::iterator icache = spokenCache.find( node );
	if( icache == spokenCache.end() )
		return false;
	else if( ( now - icache->second ) == 0
		|| ( now - icache->second ) < getConfig<int>("node", "lifetime_spoken_cache") )
		return true;
	else
		return false;
}

void CNode::addToSpokenCache(CNode* node, int now) 
{
	spokenCache.insert( pair<CNode*, int>(node, now) );
}

//vector<CData> CNode::dropOverdueData(int now) {
//	if( buffer.empty() )
//		return vector<CData>();
////	if( ! CData::useTTL() )
////		return vector<CData>();
//
//	vector<CData> overflow;
//	for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
//	{
//		idata->updateStatus(now);
//		//如果TTL过期，丢弃
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


//手动将数据压入 buffer，不伴随其他任何操作
//注意：必须在调用此函数之后手动调用 updateBufferStatus() 检查溢出

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

	myData = buffer;
	//myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByTimeBirth);
	overflow = removeDataByCapacity(myData, capacityBuffer, true);

	buffer = myData;
	return overflow;
}

vector<CData> CNode::updateBufferStatus(int now) 
{
	vector<CData> overflow = dropDataIfOverflow();

	return overflow;
}

vector<int> CNode::updateSummaryVector() 
{
	if( buffer.size() > getConfig<int>("node", "buffer") )
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
	// TODO: 恢复时重新充满能量？
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

void CNode::updateStatus(int now)
{
	if( this->time == now )
		return;

	int newTime = this->time;

	//计算能耗、更新工作状态
	while( ( newTime + getConfig<int>("simulation", "slot") ) <= now )
	{
		newTime += getConfig<int>("simulation", "slot");

		// 0 时间时初始化
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
				consumeEnergy(getConfig<double>("trans", "consumption_wake") * getConfig<int>("simulation", "slot"), now);
				updateTimerWake(newTime);

				break;
			case _asleep:
				consumeEnergy(getConfig<double>("trans", "consumption_sleep") * getConfig<int>("simulation", "slot"), now);
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

	//生成数据
	if( now <= getConfig<int>("simulation", "datatime") )
		generateData(now);


	/**************************************  Prophet  *************************************/
	if( getConfig<CConfiguration::EnumRoutingProtocolScheme>("simulation", "routing_protocol") == config::_prophet 
		 && (now % getConfig<int>("trace", "interval")) == 0  )
		CProphet::decayDeliveryPreds(this, now);

	//更新坐标及时间戳
	if( ! trace->isValid(now) )
	{
		die(now);
		CPrintHelper::PrintDetail(now, this->getName() + " dies of trace exhaustion.");
		return;
	}
	setLocation(trace->getLocation(now));
	setTime(now);

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

	if( getConfig<CConfiguration::EnumRelayScheme>("node", "scheme_relay") == config::_selfish )
		return capacity;
	else if( getConfig<CConfiguration::EnumRelayScheme>("node", "scheme_relay") == config::_loose )
		return capacityBuffer;
	else
		return 0;
}
