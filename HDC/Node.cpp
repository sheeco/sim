#include "Node.h"
#include "Preprocessor.h"

int CNode::ID_COUNT = 0;
int CNode::BUFFER_CAPACITY = BUFFER_CAPACITY_NODE;
int CNode::MAX_QUEUE_SIZE = CNode::BUFFER_CAPACITY;
int CNode::SPOKEN_MEMORY = 0;
int CNode::STATE_WORK = 1;
int CNode::STATE_REST = 0;
double CNode::SUM_ENERGY_CONSUMPTION = 0;
vector<CNode> CNode::nodes;
vector<int> CNode::idNodes;

CNode::~CNode(void)
{
}

void CNode::dropDataIfOverflow(int currentTime)
{
	vector<CData> myData;
	vector<CData> otherData;
	for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); idata++)
	{
		if( idata->getID() == this->ID )
			myData.push_back( *idata );
		else
			otherData.push_back( *idata );
	}
	CPreprocessor::mergeSort(otherData, CPreprocessor::ascendByData);
	CPreprocessor::mergeSort(myData, CPreprocessor::ascendByData);
	//如果超出MAX_QUEUE_SIZE
	if(otherData.size() > MAX_QUEUE_SIZE)
		otherData = vector<CData>( otherData.end() - MAX_QUEUE_SIZE, otherData.end() );
	myData.insert( myData.begin(), otherData.begin(), otherData.end() );
	//如果总长度溢出
	if( myData.size() > BUFFER_CAPACITY )
	{
		myData = vector<CData>( myData.end() - BUFFER_CAPACITY, myData.end() );
	}			
	buffer = myData;
}


//vector<CData> CNode::sendData(int num)
//{
//	if( buffer.empty() )
//	{
//		vector<CData> data;
//		return data;
//	}
//	//FIFO
//	if(num > buffer.size())
//		num = buffer.size();
//	vector<CData>::iterator begin = buffer.begin();
//	vector<CData> data(begin, begin + num);
//	buffer.erase(begin, begin + num);
//	//Consume Energy
//	energyConsumption += num * (CONSUMPTION_DATA_SEND * 3 + CONSUMPTION_DATA_RECIEVE * 4);
//	return data;
//}

//void CNode::failSendData()
//{
//	energyConsumption += buffer.size() * (CONSUMPTION_DATA_SEND * 3 + CONSUMPTION_DATA_RECIEVE * 4);
//}

