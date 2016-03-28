#include "Node.h"
#include "Sink.h"
#include "Ctrl.h"
#include "Epidemic.h"
#include "SortHelper.h"
#include "FileHelper.h"
#include <typeinfo.h>
#include "Prophet.h"
#include "MacProtocol.h"

int CNode::COUNT_ID = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������

int CNode::encounterAtHotspot = 0;
int CNode::encounterActiveAtHotspot = 0;
int CNode::encounterActive = 0;
int CNode::encounter = 0;
int CNode::visiterAtHotspot = 0;
int CNode::visiterOnRoute = 0;
int CNode::transmitSuccessful = 0;
int CNode::transmit = 0;

vector<CNode*> CNode::nodes;
vector<int> CNode::idNodes;
vector<CNode*> CNode::deadNodes;
vector<CNode *> CNode::deletedNodes;

int CNode::MIN_NUM_NODE = 0;
int CNode::MAX_NUM_NODE = 0;
int CNode::INIT_NUM_NODE = 0;

int CNode::SLOT_TOTAL = 0;
double CNode::DEFAULT_DUTY_CYCLE = 0;
double CNode::HOTSPOT_DUTY_CYCLE = 0; 
int CNode::DEFAULT_DISCOVER_CYCLE = 0; 

double CNode::DEFAULT_DATA_RATE = 0;
int CNode::SIZE_DATA = 0;

int CNode::CAPACITY_BUFFER = 0;
int CNode::CAPACITY_ENERGY = 0;
int CNode::LIFETIME_SPOKEN_CACHE = 0;
CGeneralNode::_RECEIVE CNode::MODE_RECEIVE = _loose;
CGeneralNode::_SEND CNode::MODE_SEND = _dump;
CGeneralNode::_QUEUE CNode::MODE_QUEUE = _fifo;

/**************************************  Prophet  *************************************/

double CNode::INIT_DELIVERY_PRED = 0.75;  //�ο�ֵ 0.75
double CNode::RATIO_PRED_DECAY = 0.98;  //�ο�ֵ 0.98(/s)
double CNode::RATIO_PRED_TRANS = 0.25;  //�ο�ֵ 0.25



void CNode::init() 
{
	dataRate = 0;
	atHotspot = nullptr;
	SLOT_DISCOVER = DEFAULT_DISCOVER_CYCLE;
	discovering = false;
	timeData = 0;
	timeDeath = 0;
	recyclable = true;
	capacityBuffer = CAPACITY_BUFFER;
	sumBufferRecord = 0;
	countBufferRecord = 0;
	dutyCycle = DEFAULT_DUTY_CYCLE;
	SLOT_LISTEN = dutyCycle * SLOT_TOTAL;
	SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;
	if( CMacProtocol::RANDOM_STATE_INIT )
		state = RandomInt(-SLOT_SLEEP, SLOT_LISTEN);
	else
		state = 0;
}

CNode::CNode() 
{
	init();
}

CNode::CNode(double dataRate) 
{
	init();
	this->dataRate = dataRate;
}

CNode::~CNode()
{
}

void CNode::initNodes() {
	if( nodes.empty() && deadNodes.empty() )
	{
		for(int i = 0; i < INIT_NUM_NODE; ++i)
		{
			double dataRate = DEFAULT_DATA_RATE;
			if(i % 5 == 0)
				dataRate *= 5;
			CNode* node = new CNode(dataRate);
			node->generateID();
			node->initDeliveryPreds();
			CNode::nodes.push_back(node);
			CNode::idNodes.push_back( node->getID() );
		}
	}
}

void CNode::generateData(int currentTime) {
	int timeDataIncre = currentTime - timeData;
	int nData = timeDataIncre * dataRate;
	if(nData > 0)
	{
		for(int i = 0; i < nData; ++i)
		{
			CData data(ID, currentTime, SIZE_DATA);
			buffer.push_back(data);
		}
		timeData = currentTime;
	}
	updateBufferStatus(currentTime);
}


vector<CNode*>& CNode::getNodes() 
{
	if( SLOT_TOTAL == 0 || ( ZERO( DEFAULT_DUTY_CYCLE ) && ZERO( HOTSPOT_DUTY_CYCLE ) ) )
	{
		cout << endl << "Error @ CNode::getNodes() : SLOT_TOTAL || ( DEFAULT_DUTY_CYCLE && HOTSPOT_DUTY_CYCLE ) = 0" << endl;
		_PAUSE_;
	}

	if( nodes.empty() && deadNodes.empty() )
		initNodes();
	return nodes;
}

int CNode::getNNodes() 
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

vector<int>& CNode::getIdNodes() 
{
	if( nodes.empty() && deadNodes.empty() )
		initNodes();
	return idNodes;
}

bool CNode::finiteEnergy() 
{
	return CAPACITY_ENERGY > 0;
}

bool CNode::hasNodes(int currentTime) 
{
	if( nodes.empty() && deadNodes.empty() )
	{
		initNodes();
		return true;
	}
	else
		idNodes.clear();
		
	bool death = false;
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); )
	{
		if( (*inode)->isAlive() )
		{
			idNodes.push_back( (*inode)->getID() );
		}
		else
		{
			(*inode)->die( currentTime, true );  //��ڵ������ľ��������Ľڵ㣬�Կɻ���
			death = true;
		}
		++inode;
	}
	ClearDeadNodes();
	if(death)
		cout << "####  [ Node ]  " << CNode::getNodes().size() << endl << endl;

	return ( ! nodes.empty() );
}

void CNode::ClearDeadNodes() 
{
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); )
	{
		if((*inode)->timeDeath > 0)
		{
			deadNodes.push_back( *inode );
			inode = nodes.erase( inode );
		}
		else
			++inode;
	}
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
	if( ZERO( this->dutyCycle - HOTSPOT_DUTY_CYCLE ) )
		return;

	if( state < 0 )
		state = 0;
	dutyCycle = HOTSPOT_DUTY_CYCLE;
	SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
	SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;

	if( state < -SLOT_LISTEN )
		state = -SLOT_LISTEN;
}

void CNode::resetDutyCycle() 
{
	if( ZERO( this->dutyCycle - DEFAULT_DUTY_CYCLE ) )
		return;

	dutyCycle = DEFAULT_DUTY_CYCLE;
	SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
	SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;

	//��ɱ��μ���֮��������
	if( state > SLOT_LISTEN )
		state = SLOT_LISTEN;
}

//CPackage CNode::sendCTSWithIndex(CNode* dst, int currentTime)
//{
//	CCtrl cts(ID, dst->getID(), currentTime, SIZE_CTRL, CCtrl::_cts);
//	CCtrl index(ID, dst->getID(), deliveryPreds, updateSummaryVector(), currentTime, SIZE_CTRL, CCtrl::_index);
//	CPackage package(this, cts, index);
//	energyConsumption += package.getSize() * CONSUMPTION_BYTE_SEND;
//	return package;
//}

//CPackage CNode::sendDataWithIndex(CNode* dst, vector<CData> datas, int currentTime)
//{
//	CCtrl index(ID, deliveryPreds, updateSummaryVector(), currentTime, SIZE_CTRL, CCtrl::_index);
//	CPackage package( *this, *dst, index, datas);
//	energyConsumption += package.getSize() * CONSUMPTION_BYTE_SEND;
//	return package;
//}

void CNode::checkDataByAck(vector<CData> ack)
{
	if( MODE_SEND == _dump )
		RemoveFromList(buffer, ack);
}


//void CNode::receivePackage(CPackage* package, int currentTime)
//{
//	energyConsumption += package->getSize() * CONSUMPTION_BYTE_RECEIVE;
//
//	vector<CGeneralData*> contents = package->getContents();
//	CGeneralNode* dst = package->getSrcNode();
//	CCtrl* ctrlToSend = nullptr;
//	CCtrl* ctrlPiggback = nullptr;
//	vector<CData> dataToSend;
//
//	for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); )
//	{
//		if( typeid(**icontent) == typeid(CCtrl) )
//		{
//			CCtrl* ctrl = dynamic_cast<CCtrl*>(*icontent);
//			switch( ctrl->getType() )
//			{
//			case CCtrl::_rts:
//				//CTS
//				ctrlToSend = new CCtrl(ID, currentTime, SIZE_CTRL, CCtrl::_cts);
//
//				//if RTS is from sink, send CTS & datas
//				if( ctrl->getNode() == CSink::SINK_ID )
//				{
//					dataToSend = buffer;
//				}
//				//skip if has spoken recently
//				else if( hasSpokenRecently(dynamic_cast<CNode*>(dst), currentTime) )
//				{
//					return;
//				}
//				else
//				{
//					//piggyback with data index otherwise
//					ctrlPiggback = new CCtrl(ID, deliveryPreds, updateSummaryVector(), currentTime, SIZE_CTRL, CCtrl::_index);
//				}
//
//				// TODO: mark skipRTS ?
//				// TODO: connection established ?
//				break;
//
//			case CCtrl::_cts:
//				//data index
//				ctrlToSend = new CCtrl(ID, deliveryPreds, updateSummaryVector(), currentTime, SIZE_CTRL, CCtrl::_index);
//
//				// TODO: connection established ?
//				break;
//			
//			case CCtrl::_index:
//				//update preds
//				updateDeliveryPredsWith( dst->getID(), ctrl->getPred() );
//
//				//select data to sent based on preds
//				if( CProphet::MAX_DATA_TRANS > 0 )
//					dataToSend = getDataByPredsAndSV( ctrl->getPred(), ctrl->getSV(), CProphet::MAX_DATA_TRANS );
//				else
//					dataToSend = getDataByPredsAndSV( ctrl->getPred(), ctrl->getSV() );
//				break;
//			
//			case CCtrl::_ack:
//				addToSpokenCache( (CNode*)(&dst), currentTime );
//				//clear data with ack
//				if( MODE_SEND == _SEND::_dump )
//					checkDataByAck( ctrl->getACK() );
//				break;
//
//			default:
//				break;
//			}
//
//			++icontent;
//		}
//		else if( typeid(**icontent) == typeid(CData) )
//		{
//			//extract data content
//			vector<CData> datas;
//			do
//			{
//				datas.push_back( *dynamic_cast<CData*>(*icontent) );
//				++icontent;
//			} while( icontent != contents.end() );
//			
//			//accept data into buffer
//			vector<CData> ack;
//			ack = bufferData(currentTime, datas);
//			//ACK
//			if( ! ack.empty() )
//				ctrlToSend = new CCtrl(ID, ack, currentTime, SIZE_CTRL, CCtrl::_ack);
//		}
//	}
//
//	free(package);
//	CPackage* packageToSend = nullptr;
//	if( ctrlToSend != nullptr )
//	{
//		if( ctrlPiggback != nullptr )
//		{
//			packageToSend = new CPackage(*this, *dst, *ctrlToSend, *ctrlPiggback);		
//			free(ctrlPiggback);
//		}
//		else if( ! dataToSend.empty() )
//		{
//			packageToSend = new CPackage(*this, *dst, *ctrlToSend, dataToSend);		
//		}
//		else
//		{
//			packageToSend = new CPackage(*this, *dst, *ctrlToSend);		
//		}
//		
//		free(ctrlToSend);
//	}
//	else
//	{
//		// TODO: connection closed ?
//		return;
//	}
//	energyConsumption += packageToSend->getSize() * CONSUMPTION_BYTE_SEND;
//	CMacProtocol::transmitPackage( packageToSend, dst, currentTime );
//	
//}

//vector<CData> CNode::bufferData(int time, vector<CData> datas) 
//{
//	if( datas.empty() )
//		return datas;
//
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
//		idata->arriveAnotherNode(time);
//	buffer.insert(buffer.begin(), datas.begin(), datas.end() );
//	
//	vector<CData> overflow = updateBufferStatus(time);
//	vector<CData> ack = datas;
//	RemoveFromList(ack, overflow);
//
//	return ack;
//}

CPackage* CNode::sendRTSWithPred(int currentTime)
{
	vector<CGeneralData*> contents;
	contents.push_back( new CCtrl(ID, currentTime, SIZE_CTRL, CCtrl::_rts) );
	contents.push_back( new CCtrl(ID, deliveryPreds, currentTime, SIZE_CTRL, CCtrl::_index) );
	CPackage* package = new CPackage(*this, CGeneralNode(), contents);

	return package;	
}

bool CNode::hasSpokenRecently(CNode* node, int currentTime) 
{
	map<CNode *, int>::iterator icache = spokenCache.find( node );
	if( icache == spokenCache.end() )
		return false;
	else if( ( currentTime - icache->second ) == 0
		|| ( currentTime - icache->second ) < CNode::LIFETIME_SPOKEN_CACHE )
		return true;
	else
		return false;
}

void CNode::addToSpokenCache(CNode* node, int currentTime) 
{
	spokenCache.insert( pair<CNode*, int>(node, currentTime) );
}

vector<CData> CNode::dropOverdueData(int currentTime) {
	if( buffer.empty() )
		return vector<CData>();
	if( ! CData::useTTL() )
		return vector<CData>();

	vector<CData> overflow;
	for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
	{
		idata->updateStatus(currentTime);
		//���TTL���ڣ�����
		if( idata->isOverdue() )
		{
			overflow.push_back( *idata );
			idata = buffer.erase( idata );
		}
		else
			++idata;
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
//			//flash_cout << "####  ( Node " << this->ID << " drops " << myData.size() - capacityBuffer << " data )                    " << endl;
//			myData = vector<CData>( myData.end() - CAPACITY_BUFFER, myData.end() );
//		}		
//	}
//	else :

	myData = buffer;
	myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByTimeBirth);
	//����ܳ������
	if( myData.size() > capacityBuffer )
	{
		//flash_cout << "####  ( Node " << this->ID << " drops " << myData.size() - capacityBuffer << " data )                 " ;
		if( CNode::MODE_QUEUE == CGeneralNode::_fifo )
		{
			overflow = 	vector<CData>( buffer.begin() + CAPACITY_BUFFER,  buffer.end() );
			myData = vector<CData>( buffer.begin(), buffer.begin() + CAPACITY_BUFFER );
		}
		else
		{
			overflow = 	vector<CData>( buffer.begin(),  buffer.end() - CAPACITY_BUFFER );
			myData = vector<CData>( buffer.end() - CAPACITY_BUFFER, buffer.end() );
		}
	}

	buffer = myData;
	return overflow;
}

vector<CData> CNode::updateBufferStatus(int currentTime) 
{
	vector<CData> overflow_1 = dropOverdueData(currentTime);
	vector<CData> overflow_2 = dropDataIfOverflow();
	overflow_2.insert( overflow_2.end(), overflow_1.begin(), overflow_1.end() );

	return overflow_2;
}

vector<int> CNode::updateSummaryVector() 
{
	if( buffer.size() > CAPACITY_BUFFER )
	{
		cout << endl << "Error @ CNode::updateSummaryVector() : Buffer isn't clean !" << endl;
		_PAUSE_;
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

void CNode::newNodes(int n) 
{
	//���Ȼָ�֮ǰ��ɾ���Ľڵ�
	// TODO: �ָ�ʱ���³���������
	for(int i = nodes.size(); i < nodes.size() + n; ++i)
	{
		if( deletedNodes.empty() )
			break;

		CNode::nodes.push_back(deletedNodes[0]);
		CNode::idNodes.push_back( deletedNodes[0]->getID() );
		--n;
	}
	//����Բ������������µĽڵ�
	for(int i = nodes.size(); i < nodes.size() + n; ++i)
	{
		double dataRate = DEFAULT_DATA_RATE;
		if(i % 5 == 0)
			dataRate *= 5;
		CNode* node = new CNode(dataRate);
		node->generateID();
		node->initDeliveryPreds();
		CNode::nodes.push_back(node);
		CNode::idNodes.push_back( node->getID() );
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
	idNodes.clear();
	for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
		idNodes.push_back( (*inode)->getID() );
	CNode::deletedNodes.insert(CNode::deletedNodes.end(), deletedNodes.begin(), deletedNodes.end());
}

void CNode::initDeliveryPreds() 
{
	if( ! deliveryPreds.empty() )
		return;

	for(int id = 0; id <= nodes.size(); ++id)
	{
		if( id != ID )
			deliveryPreds[id] = INIT_DELIVERY_PRED;
	}
}

void CNode::decayDeliveryPreds(int currentTime) 
{
	// TODO: refine decay ratio ?
	for(map<int, double>::iterator imap = deliveryPreds.begin(); imap != deliveryPreds.end(); ++imap)
		deliveryPreds[ imap->first ] = imap->second * pow( RATIO_PRED_DECAY, ( currentTime - time ) / SLOT_MOBILITYMODEL );
}

// TODO: check LISTEN cons calculation if *move
// TODO: move state update into mac layer
bool CNode::updateStatus(int currentTime)
{
	if( time == currentTime )
		return true;

	int oldState = state;
	int timeIncre = currentTime - this->time;
	int nCycle = timeIncre / SLOT_TOTAL;
	int timeListen = 0;
	int timeSleep = 0;

	//���¹���״̬
	int newState = ( state + SLOT_SLEEP + timeIncre ) % SLOT_TOTAL - SLOT_SLEEP;

	//��������ھӽڵ㷢��ʱ�ۣ��ڴ�ʱ������ͣ
	if( oldState < SLOT_DISCOVER
		&& newState >= SLOT_DISCOVER )
	{
		state = SLOT_DISCOVER;
		discovering = true;
		timeIncre = SLOT_DISCOVER - oldState;
		currentTime = time + timeIncre;
	}
	else
		state = newState;

	//��������������ܺ�
	timeListen += nCycle * SLOT_LISTEN;
	if( oldState != state )
	{
		//  [			------------|=====>     ]

		if( oldState <= 0 && state > 0 )
			timeListen += state;

		//  [				        |  =========]
		//  [---------------->      |           ]

		else if( oldState >= 0 && state <= 0 )
			timeListen += SLOT_LISTEN - oldState;

		//  [				        |  ===>     ]

		else if( oldState > 0 && state > 0 && oldState < state )
			timeListen += state - oldState;

		//  [				        |      =====]
		//  [-----------------------|==>       ]

		else if( state > 0 && oldState > 0 && state < oldState )
			timeListen += SLOT_LISTEN - ( oldState - state );
	}
	timeSleep = timeIncre - timeListen;
	consumeEnergy( timeListen * CONSUMPTION_LISTEN + timeSleep * CONSUMPTION_SLEEP );

	//��������
	if( currentTime <= DATATIME )
		generateData(currentTime);


	/**************************************  Prophet  *************************************/
	if( ROUTING_PROTOCOL == _prophet 
		&& (currentTime % SLOT_MOBILITYMODEL) == 0 )
		decayDeliveryPreds(currentTime);

	//��������
	CCoordinate location;
	if( ! CFileHelper::getLocationFromFile(ID, currentTime, location) )
	{
		die(currentTime, false);  //�� trace ��Ϣ��ֹ�������Ľڵ㣬�޷�����
		return false;
	}
	setLocation(location, currentTime);

	return true;
}

bool CNode::isListening() const 
{
	if( ZERO(dutyCycle) )
		return false;

	return state >= 0;
}

void CNode::recordBufferStatus() 
{
	if( timeDeath > 0 )
		return;

	sumBufferRecord += buffer.size();
	++countBufferRecord;
}

//vector<CData> CNode::sendAllData(_SEND mode) 
//{
//	return CGeneralNode::sendAllData(mode);
//}

//vector<CData> CNode::sendData(int n) 
//{
//	if( n >= buffer.size() )
//		return sendAllData(_dump);
//
//	double bet = RandomFloat(0, 1);
//	if(bet > PROB_TRANS)
//	{
//		energyConsumption += n * SIZE_DATA * CONSUMPTION_BYTE_SEND;
//		return vector<CData>();
//	}
//
//	vector<CData> data;
//	if( MODE_QUEUE == _fifo )
//	{
//		data.insert(data.begin(), buffer.begin(), buffer.begin() + n);
//	}
//	else if( MODE_QUEUE == _lifo )
//	{
//		data.insert(data.begin(), buffer.end() - n, buffer.end());
//	}
//	return data;
//}

//bool CNode::receiveData(int time, vector<CData> datas) 
//{
//	if( datas.empty() )
//		return false;
//	if( RandomFloat(0, 1) > PROB_TRANS )
//		return false;
//
//	energyConsumption += CONSUMPTION_BYTE_RECEIVE * SIZE_DATA * datas.size();
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
//		idata->arriveAnotherNode(time);
//	buffer.insert(buffer.begin(), datas.begin(), datas.end() );
//	updateBufferStatus(time);
//		
//	return true;
//}

//vector<int> CNode::sendSummaryVector() 
//{
//	updateSummaryVector();
//	energyConsumption += CONSUMPTION_BYTE_SEND * SIZE_CTRL;
//	return summaryVector;
//}

//vector<int> CNode::receiveSummaryVector(vector<int> sv) 
//{
//	if( RandomFloat(0, 1) > PROB_TRANS )
//		return vector<int>();
//
//	energyConsumption += CONSUMPTION_BYTE_RECEIVE * SIZE_CTRL;
//	return sv;	
//}

//vector<int> CNode::sendRequestList(vector<int> sv) 
//{
//	if( sv.empty() )
//		return vector<int>();
//
//	updateSummaryVector();
//	RemoveFromList(sv, summaryVector);
//	//������
//	if( CEpidemic::MAX_DATA_RELAY > 0  &&  sv.size() > CEpidemic::MAX_DATA_RELAY )
//	{
//		vector<int>::iterator id = sv.begin();
//		for(int count = 0; id != sv.end() &&  count < CEpidemic::MAX_DATA_RELAY ; )
//		{
//			if( CData::getNodeByMask( *id ) != this->ID )
//			{
//				++count;
//				++id;
//			}
//		}
//		sv = vector<int>( sv.begin(), id );
//	}
//
//	energyConsumption += CONSUMPTION_BYTE_SEND * SIZE_CTRL;
//	return sv;
//}

//vector<int> CNode::receiveRequestList(vector<int> req) 
//{
//	if( req.empty() )
//		return vector<int>();
//	if( RandomFloat(0, 1) > PROB_TRANS )
//		return vector<int>();
//
//	energyConsumption += CONSUMPTION_BYTE_RECEIVE * SIZE_CTRL;
//	return req;
//}

vector<CData> CNode::getDataByRequestList(vector<int> requestList) const
{
	if( requestList.empty() || buffer.empty() )
		return vector<CData>();

	vector<CData> result;
	result = CData::GetItemsByID(buffer, requestList);
	return result;
}

void CNode::updateDeliveryPredsWith(int node, map<int, double> preds) 
{
	double oldPred = 0, transPred = 0, dstPred = 0;
	if( deliveryPreds.find(node) == deliveryPreds.end() )
		deliveryPreds[ node ] = INIT_DELIVERY_PRED;

	oldPred = this->deliveryPreds[ node ];
	deliveryPreds[ node ] = transPred = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;

	for(map<int, double>::iterator imap = preds.begin(); imap != preds.end(); ++imap)
	{
		int dst = imap->first;
		if( dst == this->ID )
			continue;
		if( deliveryPreds.find(node) == deliveryPreds.end() )
			deliveryPreds[ node ] = INIT_DELIVERY_PRED;

		oldPred = this->deliveryPreds[ dst ];
		dstPred = imap->second;
		deliveryPreds[ dst ] = oldPred + ( 1 - oldPred ) * transPred * dstPred * RATIO_PRED_TRANS;
	}
		
}

void CNode::updateDeliveryPredsWithSink()
{
	double oldPred = deliveryPreds[CSink::getSink()->getID()];
	deliveryPreds[CSink::getSink()->getID()] = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;
}

//map<int, double> CNode::sendDeliveryPreds() 
//{
//	energyConsumption += CONSUMPTION_BYTE_SEND * SIZE_CTRL;
//		
//	return deliveryPreds;
//}
//
//map<int, double> CNode::receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv) 
//{
//	if( preds.empty() )
//		return map<int, double>();
//	if( RandomFloat(0, 1) > PROB_TRANS )
//		return map<int, double>();
//
//	energyConsumption += CONSUMPTION_BYTE_RECEIVE * SIZE_CTRL;
//	return preds;
//}

int CNode::ChangeNodeNumber()
{
	int delta = 0;
	float bet = 0;
	do
	{
		bet = RandomFloat(-1, 1);
		if(bet > 0)
			bet = 0.2 + bet / 2;  //���ı������� 0.2
		else
			bet = -0.2 + bet / 2;
		delta = ROUND( bet * (MAX_NUM_NODE - MIN_NUM_NODE) );
	}while(delta != 0);

	if(delta < MIN_NUM_NODE - nodes.size())
	{
		delta = nodes.size() - MIN_NUM_NODE;
		removeNodes(delta);
	}
	else if(delta > MAX_NUM_NODE - nodes.size())
	{
		delta = MAX_NUM_NODE - nodes.size();
		newNodes(delta);
	}

	return delta;
}
