#include "Node.h"
#include "Preprocessor.h"

int CNode::ID_COUNT = 0;  //��1��ʼ����ֵ���ڽڵ�����
int CNode::BUFFER_CAPACITY = BUFFER_CAPACITY_NODE;
double CNode::DEFAULT_DUTY_CYCLE = 0;
double CNode::HOTSPOT_DUTY_CYCLE = 0; 
int CNode::SLOT_TOTAL = 0;
double CNode::ENERGY = 0;
double CNode::SUM_ENERGY_CONSUMPTION = 0;
vector<CNode*> CNode::nodes;
vector<int> CNode::idNodes;
vector<CNode*> CNode::deadNodes;

/**************************************  Prophet  *************************************/
double CNode::INIT_DELIVERY_PRED = 0.70;  //0.75
double CNode::DECAY_RATIO = 0.90;  //0.98(/s)
double CNode::TRANS_RATIO = 0.20;  //0.25


void CNode::dropDataIfOverflow(int currentTime)
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
	//otherData = CPreprocessor::mergeSort(otherData, CPreprocessor::ascendByData);
	//myData = CPreprocessor::mergeSort(myData, CPreprocessor::ascendByData);
	////�������MAX_QUEUE_SIZE
	//if(otherData.size() > Epidemic::MAX_QUEUE_SIZE)
	//	otherData = vector<CData>( otherData.end() - Epidemic::MAX_QUEUE_SIZE, otherData.end() );
	//myData.insert( myData.begin(), otherData.begin(), otherData.end() );
	////����ܳ������
	//if( myData.size() > bufferCapacity )
	//{
	//	cout << endl << "####  ( Node " << this->ID << " drops " << myData.size() - bufferCapacity << " data )" << endl;
	//	myData = vector<CData>( myData.end() - BUFFER_CAPACITY, myData.end() );
	//}		

	vector<CData> myData = buffer;
	myData = CPreprocessor::mergeSort(myData, CPreprocessor::ascendByData);
	//����ܳ������
	if( myData.size() > bufferCapacity )
	{
		flash_cout << "####  ( Node " << this->ID << " drops " << myData.size() - bufferCapacity << " data )     " ;
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
	SUM_ENERGY_CONSUMPTION += timeListen * CONSUMPTION_LISTEN + timeSleep * CONSUMPTION_SLEEP;

	/**************************************  Prophet  *************************************/

	decayDeliveryPreds(currentTime);

	//��������
	double x = 0, y = 0;
	CFileParser::getPositionFromFile(ID, currentTime, x, y);
	moveTo(x, y, currentTime);

	return state >= 0;
}