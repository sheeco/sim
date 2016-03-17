#include "Node.h"
#include "Sink.h"
#include "Ctrl.h"
#include "Epidemic.h"
#include "SortHelper.h"
#include "FileHelper.h"
#include <typeinfo.h>

int CNode::ID_COUNT = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������

int CNode::encounterAtHotspot = 0;
int CNode::encounterActive = 0;
int CNode::encounter = 0;
int CNode::visiterAtHotspot = 0;
int CNode::visiterOnRoute = 0;

vector<CNode*> CNode::nodes;
vector<int> CNode::idNodes;
vector<CNode*> CNode::deadNodes;
vector<CNode *> CNode::deletedNodes;

int CNode::NUM_NODE_MIN = 0;
int CNode::NUM_NODE_MAX = 0;
int CNode::NUM_NODE_INIT = 0;

int CNode::SLOT_TOTAL = 0;
double CNode::DEFAULT_DUTY_CYCLE = 0;
double CNode::HOTSPOT_DUTY_CYCLE = 0; 
int CNode:: DEFAULT_SLOT_WAIT = 0; 

double CNode::DEFAULT_DATA_RATE = 0;
int CNode::DATA_SIZE = 0;
int CNode::CTRL_SIZE = 0;

int CNode::BUFFER_CAPACITY = 0;
int CNode::ENERGY = 0;
CGeneralNode::_RECEIVE CNode::RECEIVE_MODE = _loose;
CGeneralNode::_SEND CNode::SEND_MODE = _dump;
CGeneralNode::_QUEUE CNode::QUEUE_MODE = _fifo;

/**************************************  Prophet  *************************************/

double CNode::INIT_DELIVERY_PRED = 0.70;  //0.75
double CNode::DECAY_RATIO = 0.90;  //0.98(/s)
double CNode::TRANS_RATIO = 0.20;  //0.25


void CNode::init() 
{
	generationRate = 0;
	atHotspot = nullptr;
	SLOT_LISTEN = 0;
	SLOT_SLEEP = 0;
	state = 0;
	timeData = 0;
	timeDeath = 0;
	recyclable = true;
	bufferCapacity = BUFFER_CAPACITY;
	bufferSizeSum = 0;
	bufferChangeCount = 0;
	dutyCycle = DEFAULT_DUTY_CYCLE;
	energy = ENERGY;
}

CNode::CNode() 
{
	init();
}

CNode::CNode(double generationRate) 
{
	init();
	this->generationRate = generationRate;
}

void CNode::initNodes() {
	if( nodes.empty() && deadNodes.empty() )
	{
		for(int i = 0; i < NUM_NODE_INIT; i++)
		{
			double generationRate = DEFAULT_DATA_RATE;
			if(i % 5 == 0)
				generationRate *= 5;
			CNode* node = new CNode(generationRate);
			node->generateID();
			node->initDeliveryPreds();
			CNode::nodes.push_back(node);
			CNode::idNodes.push_back( node->getID() );
		}
	}
}

void CNode::generateData(int currentTime) {
	int timeDataIncre = currentTime - timeData;
	int nData = timeDataIncre * generationRate;
	if(nData > 0)
	{
		for(int i = 0; i < nData; ++i)
		{
			CData data(ID, currentTime, DATA_SIZE);
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
	return ENERGY > 0;
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
		cout << "####  [ Node ]  " << CNode::getNodes().size() << endl;

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
	auto allNodes = CNode::getAllNodes(false);
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

CPackage CNode::sendRTS(int currentTime) 
{
	CCtrl rts(ID, currentTime, CTRL_SIZE, CCtrl::_rts);
	CPackage package(this, rts);
	energyConsumption += package.getSize() * CONSUMPTION_BYTE_SEND;
	return package;
}

// TODO: 
void CNode::receiveRTS(CPackage package)
{
	energyConsumption += package.getSize() * CONSUMPTION_BYTE_RECIEVE;
}

CPackage CNode::sendCTS(CNode* dst, int currentTime)
{
	CCtrl cts(ID, dst->getID(), currentTime, CTRL_SIZE, CCtrl::_cts);
	if( ROUTING_PROTOCOL == _prophet )
	{
		CCtrl preds(ID, dst->getID(), currentTime, deliveryPreds, CTRL_SIZE, CCtrl::_pred);
		CPackage package(this, cts, preds);
		energyConsumption += package.getSize() * CONSUMPTION_BYTE_SEND;
		return package;
	}
	else
	{
		CPackage package(this, cts);
		energyConsumption += package.getSize() * CONSUMPTION_BYTE_SEND;
		return package;
	}
}

// TODO: 
void CNode::receivePackage(CPackage package)
{
	vector<CGeneralData*> contents = package.getContent();

	for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); ++icontent)
	{
		if( typeid(**icontent) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*icontent);
			switch( ctrl->getType() )
			{
			case CCtrl::_rts:
				break;
			case CCtrl::_cts:
				break;
			case CCtrl::_pred:
				break;
			case CCtrl::_ack:
				break;
			default:
				break;
			}
		}
		else if( typeid(**icontent) == typeid(CData) )
		{
			CData* data = dynamic_cast<CData*>(*icontent);
			
		}
	}
}

vector<CNode*> CNode::broadcastPackage(CPackage package)
{
	vector<CNode*> rcvNodes;
	for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		if( (*inode)->getID() == package.getNode()->getID() )
			continue;
		if( (*inode)->isListening() 
			&& CBasicEntity::getDistance(*(package.getNode()), **inode) <= TRANS_RANGE
			&& Bet(PROB_DATA_FORWARD) )
		{
			rcvNodes.push_back(*inode);
			(*inode)->receivePackage(package);
		}
	}

	// TODO: sort by distance with src node ?
	return rcvNodes;
}

// TODO: 
void CNode::transmitPackage(CPackage package)
{
	
}

void CNode::dropOverdueData(int currentTime) {
	if( buffer.empty() )
		return;
	if( CData::useHOP() )
		return;

	for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
	{
		idata->updateStatus(currentTime);
		//���TTL���ڣ�����
		if( idata->isOverdue() )
			idata = buffer.erase( idata );
		else
			++idata;
	}
}

void CNode::dropDataIfOverflow()
{
	if( buffer.empty() )
		return;

	vector<CData> myData;

	if( CEpidemic::MAX_QUEUE_SIZE > 0 )
	{
		vector<CData> otherData;
		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); ++idata)
		{
			if( idata->getNode() == this->ID )
				myData.push_back( *idata );
			else
				otherData.push_back( *idata );
		}
		otherData = CSortHelper::mergeSort(otherData, CSortHelper::ascendByData);
		myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByData);
		//�������MAX_QUEUE_SIZE
		if(otherData.size() > CEpidemic::MAX_QUEUE_SIZE)
			otherData = vector<CData>( otherData.end() - CEpidemic::MAX_QUEUE_SIZE, otherData.end() );
		myData.insert( myData.begin(), otherData.begin(), otherData.end() );
		//����ܳ������
		if( myData.size() > bufferCapacity )
		{
			//flash_cout << "####  ( Node " << this->ID << " drops " << myData.size() - bufferCapacity << " data )                    " << endl;
			myData = vector<CData>( myData.end() - BUFFER_CAPACITY, myData.end() );
		}		
	}
	else
	{
		myData = buffer;
		myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByData);
		//����ܳ������
		if( myData.size() > bufferCapacity )
		{
			//flash_cout << "####  ( Node " << this->ID << " drops " << myData.size() - bufferCapacity << " data )                 " ;
			myData = vector<CData>( myData.end() - BUFFER_CAPACITY, myData.end() );
		}
	}

	buffer = myData;
}

void CNode::updateBufferStatus(int currentTime) 
{
	dropOverdueData(currentTime);
	dropDataIfOverflow();
}

void CNode::updateSummaryVector() 
{
	if( buffer.size() > BUFFER_CAPACITY )
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
}

void CNode::newNodes(int n) 
{
	//���Ȼָ�֮ǰ��ɾ���Ľڵ�
	// TODO: �ָ�ʱ���³���������
	for(int i = nodes.size(); i < nodes.size() + n; i++)
	{
		if( deletedNodes.empty() )
			break;

		CNode::nodes.push_back(deletedNodes[0]);
		CNode::idNodes.push_back( deletedNodes[0]->getID() );
		--n;
	}
	//����Բ������������µĽڵ�
	for(int i = nodes.size(); i < nodes.size() + n; i++)
	{
		double generationRate = DEFAULT_DATA_RATE;
		if(i % 5 == 0)
			generationRate *= 5;
		CNode* node = new CNode(generationRate);
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

	for(int id = 0; id <= nodes.size(); id++)
	{
		if( id != ID )
			deliveryPreds[id] = INIT_DELIVERY_PRED;
	}
}

void CNode::decayDeliveryPreds(int currentTime) 
{
	for(map<int, double>::iterator imap = deliveryPreds.begin(); imap != deliveryPreds.end(); ++imap)
		deliveryPreds[ imap->first ] = imap->second * pow( DECAY_RATIO, ( currentTime - time ) / SLOT_MOBILITYMODEL );
}

bool CNode::updateStatus(int currentTime)
{
	int oldState = state;
	int timeIncre = currentTime - this->time;
	int nCycle = timeIncre / SLOT_TOTAL;
	int timeListen = 0;
	int timeSleep = 0;

	//���¹���״̬
	state = ( state + SLOT_SLEEP + timeIncre ) % SLOT_TOTAL - SLOT_SLEEP;

	//��������������ܺ�
	timeListen += nCycle * SLOT_LISTEN;
	if( oldState != state )
	{
		if( oldState <= 0 && state > 0 )
			timeListen += state;

		else if( oldState >= 0 && state <= 0 )
			timeListen += SLOT_LISTEN - oldState;

		else if( oldState > 0 && state > 0 && oldState < state )
			timeListen += state - oldState;

		else if( state > 0 && oldState > 0 && state < oldState )
			timeListen += SLOT_LISTEN - ( oldState - state );
	}
	timeSleep = timeIncre - timeListen;
	energyConsumption += timeListen * CONSUMPTION_LISTEN + timeSleep * CONSUMPTION_SLEEP;

	//��������
	generateData(currentTime);


	/**************************************  Prophet  *************************************/
	if( ROUTING_PROTOCOL == _prophet )
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
	bufferSizeSum += buffer.size();
	bufferChangeCount++;
}

vector<CData> CNode::sendAllData(_SEND mode) 
{
	return CGeneralNode::sendAllData(mode);
}

vector<CData> CNode::sendData(int n) 
{
	if( n >= buffer.size() )
		return sendAllData(_dump);

	double bet = RandomFloat(0, 1);
	if(bet > PROB_DATA_FORWARD)
	{
		energyConsumption += n * DATA_SIZE * CONSUMPTION_BYTE_SEND;
		return vector<CData>();
	}

	vector<CData> data;
	if( QUEUE_MODE == _fifo )
	{
		data.insert(data.begin(), buffer.begin(), buffer.begin() + n);
	}
	else if( QUEUE_MODE == _lifo )
	{
		data.insert(data.begin(), buffer.end() - n, buffer.end());
	}
	return data;
}

bool CNode::receiveData(int time, vector<CData> datas) 
{
	if( datas.empty() )
		return false;
	if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
		return false;

	energyConsumption += CONSUMPTION_BYTE_RECIEVE * DATA_SIZE * datas.size();
	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
		idata->arriveAnotherNode(time);
	buffer.insert(buffer.begin(), datas.begin(), datas.end() );
	updateBufferStatus(time);
		
	return true;
}

bool CNode::hasSpokenRecently(CNode* node, int currentTime) 
{
	map<CNode *, int>::iterator icache = spokenCache.find( node );
	if( icache == spokenCache.end() )
		return false;
	else if( ( currentTime - icache->second ) < CEpidemic::SPOKEN_MEMORY )
		return true;
	else
		return false;
}

void CNode::addToSpokenCache(CNode* node, int t) 
{
	spokenCache.insert( pair<CNode*, int>(node, t) );
}

vector<int> CNode::sendSummaryVector() 
{
	updateSummaryVector();
	energyConsumption += CONSUMPTION_BYTE_SEND * CTRL_SIZE;
	return summaryVector;
}

vector<int> CNode::receiveSummaryVector(vector<int> sv) 
{
	if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
		return vector<int>();

	energyConsumption += CONSUMPTION_BYTE_RECIEVE * CTRL_SIZE;
	return sv;	
}

vector<int> CNode::sendRequestList(vector<int> sv) 
{
	if( sv.empty() )
		return vector<int>();

	updateSummaryVector();
	RemoveFromList(sv, summaryVector);
	//������
	if( CEpidemic::MAX_QUEUE_SIZE > 0  &&  sv.size() > CEpidemic::MAX_QUEUE_SIZE )
	{
		vector<int>::iterator id = sv.begin();
		for(int count = 0; id != sv.end() &&  count < CEpidemic::MAX_QUEUE_SIZE ; )
		{
			if( CData::getNodeByMask( *id ) != this->ID )
			{
				count++;
				++id;
			}
		}
		sv = vector<int>( sv.begin(), id );
	}

	energyConsumption += CONSUMPTION_BYTE_SEND * CTRL_SIZE;
	return sv;
}

vector<int> CNode::receiveRequestList(vector<int> req) 
{
	if( req.empty() )
		return vector<int>();
	if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
		return vector<int>();

	energyConsumption += CONSUMPTION_BYTE_RECIEVE * CTRL_SIZE;
	return req;
}

vector<CData> CNode::sendDataByRequestList(vector<int> requestList) 
{
	if( requestList.empty() )
		return vector<CData>();

	vector<CData> result;
	result = GetItemsByID(buffer, requestList);
	energyConsumption += CONSUMPTION_BYTE_SEND * DATA_SIZE * result.size();
		
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
		deliveryPreds[ dst ] = oldPred + ( 1 - oldPred ) * transPred * dstPred * TRANS_RATIO;
	}
		
}

void CNode::updateDeliveryPredsWithSink()
{
	double oldPred = deliveryPreds[CSink::getSink()->getID()];
	deliveryPreds[CSink::getSink()->getID()] = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;
}

map<int, double> CNode::sendDeliveryPreds() 
{
	energyConsumption += CONSUMPTION_BYTE_SEND * CTRL_SIZE;
		
	return deliveryPreds;
}

map<int, double> CNode::receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv) 
{
	if( preds.empty() )
		return map<int, double>();
	if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
		return map<int, double>();

	energyConsumption += CONSUMPTION_BYTE_RECIEVE * CTRL_SIZE;
	return preds;
}

vector<CData> CNode::sendDataByPredsAndSV(map<int, double> preds, vector<int> &sv)
{
	if( preds.empty() )
		return vector<CData>();

	if( preds.find(CSink::getSink()->getID())->second > this->deliveryPreds.find(CSink::getSink()->getID())->second )
	{		
		vector<int> req = summaryVector;
		RemoveFromList(req, sv);
		return sendDataByRequestList( req );
	}
	else
		return vector<CData>();
}

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
		delta = ROUND( bet * (NUM_NODE_MAX - NUM_NODE_MIN) );
	}while(delta != 0);

	if(delta < NUM_NODE_MIN - nodes.size())
	{
		delta = nodes.size() - NUM_NODE_MIN;
		removeNodes(delta);
	}
	else if(delta > NUM_NODE_MAX - nodes.size())
	{
		delta = NUM_NODE_MAX - nodes.size();
		newNodes(delta);
	}

	return delta;
}
