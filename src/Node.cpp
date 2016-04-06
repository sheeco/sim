#include "Node.h"
#include "Sink.h"
#include "Ctrl.h"
//#include "Epidemic.h"
#include "SortHelper.h"
#include "FileHelper.h"
#include <typeinfo.h>
#include "Prophet.h"
#include "MacProtocol.h"

int CNode::COUNT_ID = 0;  //从1开始，数值等于当前实例总数

int CNode::encounterAtHotspot = 0;
//int CNode::encounterActiveAtHotspot = 0;
//int CNode::encounterActive = 0;
int CNode::encounter = 0;
int CNode::visiterAtHotspot = 0;
int CNode::visiter = 0;
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

double CNode::INIT_DELIVERY_PRED = 0.75;  //参考值 0.75
double CNode::RATIO_PRED_DECAY = 0.98;  //参考值 0.98(/s)
double CNode::RATIO_PRED_TRANS = 0.25;  //参考值 0.25



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
			(*inode)->die( currentTime, true );  //因节点能量耗尽而死亡的节点，仍可回收
			death = true;
		}
		++inode;
	}
	ClearDeadNodes();
	if(death)
		flash_cout << "####  [ Node ]  " << CNode::getNodes().size() << "                                     " << endl;

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

	//完成本次监听之后再休眠
	if( state > SLOT_LISTEN )
		state = SLOT_LISTEN;
}

void CNode::checkDataByAck(vector<CData> ack)
{
	if( MODE_SEND == _dump )
		RemoveFromList(buffer, ack);
}

CPackage* CNode::sendRTSWithCapacityAndPred(int currentTime)
{
	vector<CGeneralData*> contents;
	if( MODE_RECEIVE == _selfish 
		&& ( ! buffer.empty() ) )
		contents.push_back( new CCtrl(ID, capacityBuffer - buffer.size(), currentTime, SIZE_CTRL, CCtrl::_capacity) );
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

vector<CData> CNode::removeDataByCapacity(vector<CData> datas, int capacity)
{
	if( capacity < datas.size() )
	{
		if( MODE_QUEUE == _fifo )
			datas = vector<CData>(datas.begin(), datas.begin() + capacity);
		else
			datas = vector<CData>(datas.rbegin(), datas.rbegin() + capacity);
	}

	return datas;
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
//		//如果超出MAX_DATA_RELAY
//		if(otherData.size() > CEpidemic::MAX_DATA_RELAY)
//			otherData = vector<CData>( otherData.end() - CEpidemic::MAX_DATA_RELAY, otherData.end() );
//		myData.insert( myData.begin(), otherData.begin(), otherData.end() );
//		//如果总长度溢出
//		if( myData.size() > capacityBuffer )
//		{
//			//flash_cout << "####  ( Node " << this->ID << " drops " << myData.size() - capacityBuffer << " data )                    " << endl;
//			myData = vector<CData>( myData.end() - CAPACITY_BUFFER, myData.end() );
//		}		
//	}
//	else :

	myData = buffer;
	myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByTimeBirth);
	//如果总长度溢出
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
//	vector<CData> overflow_1 = dropOverdueData(currentTime);
	vector<CData> overflow = dropDataIfOverflow();
//	overflow_2.insert( overflow.end(), overflow_1.begin(), overflow_1.end() );

	return overflow;
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
	//优先恢复之前被删除的节点
	// TODO: 恢复时重新充满能量？
	for(int i = nodes.size(); i < nodes.size() + n; ++i)
	{
		if( deletedNodes.empty() )
			break;

		CNode::nodes.push_back(deletedNodes[0]);
		CNode::idNodes.push_back( deletedNodes[0]->getID() );
		--n;
	}
	//如果仍不足数，构造新的节点
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

	//更新工作状态
	int newState = ( state + SLOT_SLEEP + timeIncre ) % SLOT_TOTAL - SLOT_SLEEP;

	//如果遇到邻居节点发现时槽，在此时槽上暂停
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

	//计算监听和休眠能耗
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

	//生成数据
	if( currentTime <= DATATIME )
		generateData(currentTime);


	/**************************************  Prophet  *************************************/
	if( ROUTING_PROTOCOL == _prophet 
		&& (currentTime % SLOT_MOBILITYMODEL) == 0 )
		decayDeliveryPreds(currentTime);

	//更新坐标
	CCoordinate location;
	if( ! CFileHelper::getLocationFromFile(ID, currentTime, location) )
	{
		die(currentTime, false);  //因 trace 信息终止而死亡的节点，无法回收
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

int CNode::getCapacityForward()
{
	int capacity = capacityBuffer - buffer.size();
	if( capacity < 0 )
		capacity = 0;

	if( MODE_RECEIVE == _selfish )
		return capacity;
	else if( MODE_RECEIVE == _loose )
		return capacityBuffer;
	else
		return 0;
}

int CNode::ChangeNodeNumber()
{
	int delta = 0;
	float bet = 0;
	do
	{
		bet = RandomFloat(-1, 1);
		if(bet > 0)
			bet = 0.2 + bet / 2;  //更改比例至少 0.2
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
