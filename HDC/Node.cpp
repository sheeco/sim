#include "Node.h"
#include "Preprocessor.h"

int CNode::ID_COUNT = 0;
int CNode::BUFFER_CAPACITY = BUFFER_CAPACITY_NODE;
int CNode::MAX_QUEUE_SIZE = CNode::BUFFER_CAPACITY;
int CNode::SPOKEN_MEMORY = 0;
double CNode::DEFAULT_DUTY_CYCLE = 0;
double CNode::HOTSPOT_DUTY_CYCLE = 0; 
int CNode::SLOT_TOTAL = 0;
//int CNode::STATE_WORK = 1;
//int CNode::STATE_REST = 0;
double CNode::SUM_ENERGY_CONSUMPTION = 0;
vector<CNode*> CNode::nodes;
vector<int> CNode::idNodes;

void CNode::dropDataIfOverflow(int currentTime)
{
	if( buffer.empty() )
		return;

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

bool CNode::updateStatus(int currentTime)
{
	int timeIncre = currentTime - time;
	state = ( state + SLOT_SLEEP + timeIncre ) % SLOT_TOTAL - SLOT_SLEEP;

	double x = 0, y = 0;
	CFileParser::getPositionFromFile(ID, currentTime, x, y);
	moveTo(x, y, currentTime);

	return state >= 0;
}