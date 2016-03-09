#include "Node.h"
#include "SortHelper.h"
#include "FileHelper.h"
#include "Sink.h"

extern _ROUTING_PROTOCOL ROUTING_PROTOCOL;

int CNode::ID_COUNT = 0;  //从1开始，数值等于当前实例总数

int CNode::SLOT_TOTAL = 0;
double CNode::DEFAULT_DUTY_CYCLE = 0;
double CNode::HOTSPOT_DUTY_CYCLE = 0; 

double CNode::DEFAULT_DATA_RATE = 0;
int CNode::DATA_SIZE = 0;
int CNode::CTRL_SIZE = 0;

int CNode::BUFFER_CAPACITY = 0;
int CNode::ENERGY = 0;
CGeneralNode::_RECEIVE CNode::RECEIVE_MODE = _loose;
CGeneralNode::_SEND CNode::SEND_MODE = _dump;
CGeneralNode::_QUEUE CNode::QUEUE_MODE = _fifo;

int CNode::encounterAtHotspot = 0;
int CNode::encounterOnRoute = 0;
int CNode::visiterAtHotspot = 0;
int CNode::visiterOnRoute = 0;

vector<CNode*> CNode::nodes;
vector<int> CNode::idNodes;
vector<CNode*> CNode::deadNodes;
vector<CNode *> CNode::deletedNodes;

int CNode::NUM_NODE_MIN = 14;
int CNode::NUM_NODE_MAX = 34;
int CNode::NUM_NODE_INIT = 29;

/**************************************  Prophet  *************************************/
double CNode::INIT_DELIVERY_PRED = 0.70;  //0.75
double CNode::DECAY_RATIO = 0.90;  //0.98(/s)
double CNode::TRANS_RATIO = 0.20;  //0.25


vector<CNode *> CNode::getAllNodes(bool sort)
{
	vector<CNode *> allNodes = CNode::getNodes();
	allNodes.insert(allNodes.end(), deadNodes.begin(), deadNodes.end());
	allNodes.insert(allNodes.end(), deletedNodes.begin(), deletedNodes.end());
	if( sort )
		allNodes = CSortHelper::mergeSort(allNodes, CSortHelper::ascendByID);
	return allNodes;
}

void CNode::dropDataIfOverflow()
{
	if( buffer.empty() )
		return;

	//vector<CData> myData;
	//vector<CData> otherData;
	//for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); idata++)
	//{
	//	if( idata->getNode() == this->ID )
	//		myData.push_back( *idata );
	//	else
	//		otherData.push_back( *idata );
	//}
	//otherData = CSortHelper::mergeSort(otherData, CSortHelper::ascendByData);
	//myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByData);
	////如果超出MAX_QUEUE_SIZE
	//if(otherData.size() > CEpidemic::MAX_QUEUE_SIZE)
	//	otherData = vector<CData>( otherData.end() - CEpidemic::MAX_QUEUE_SIZE, otherData.end() );
	//myData.insert( myData.begin(), otherData.begin(), otherData.end() );
	////如果总长度溢出
	//if( myData.size() > bufferCapacity )
	//{
	//	cout << endl << "####  ( Node " << this->ID << " drops " << myData.size() - bufferCapacity << " data )" << endl;
	//	myData = vector<CData>( myData.end() - BUFFER_CAPACITY, myData.end() );
	//}		

	vector<CData> myData = buffer;
	myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByData);
	//如果总长度溢出
	if( myData.size() > bufferCapacity )
	{
		//flash_cout << "####  ( Node " << this->ID << " drops " << myData.size() - bufferCapacity << " data )                 " ;
		myData = vector<CData>( myData.end() - BUFFER_CAPACITY, myData.end() );
	}
	buffer = myData;
}

bool CNode::updateStatus(int currentTime)
{
	int oldState = state;
	int timeIncre = currentTime - this->time;
	int nCycle = timeIncre / SLOT_TOTAL;
	int timeListen = 0;
	int timeSleep = 0;

	//更新工作状态
	state = ( state + SLOT_SLEEP + timeIncre ) % SLOT_TOTAL - SLOT_SLEEP;

	//计算监听和休眠能耗
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

	//生成数据
	generateData(currentTime);


	/**************************************  Prophet  *************************************/
	if( ROUTING_PROTOCOL == _prophet )
		decayDeliveryPreds(currentTime);

	//更新坐标
	double x = 0, y = 0;
	CFileHelper::getPositionFromFile(ID, currentTime, x, y);
	moveTo(x, y, currentTime);

	return state >= 0;
}

void CNode::updateDeliveryPredsWithSink()
{
	double oldPred = deliveryPreds[CSink::getSink()->getID()];
	deliveryPreds[CSink::getSink()->getID()] = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;
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
			bet = 0.2 + bet / 2;  //更改比例至少 0.2
		else
			bet = -0.2 + bet / 2;
		delta = ROUND( bet * (NUM_NODE_MAX - NUM_NODE_MIN) );
	}while(delta != 0);

	if(delta < NUM_NODE_MIN - nodes.size())
	{
		delta = NUM_NODE_MIN - nodes.size();
		removeNodes(delta);
	}
	else if(delta > NUM_NODE_MAX - nodes.size())
	{
		delta = NUM_NODE_MAX - nodes.size();
		newNodes(delta);
	}

	return delta;
}
