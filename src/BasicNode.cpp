#include "BasicNode.h"
#include "Sink.h"
#include "Ctrl.h"
//#include "Epidemic.h"
#include "SortHelper.h"
#include "FileHelper.h"
#include <typeinfo.h>
#include "MacProtocol.h"
#include "Trace.h"

int CBasicNode::COUNT_ID = 0;  //从1开始，数值等于当前实例总数

int CBasicNode::encounterAtHotspot = 0;
//int CBasicNode::encounterActiveAtHotspot = 0;
//int CBasicNode::encounterActive = 0;
int CBasicNode::encounter = 0;
int CBasicNode::visiterAtHotspot = 0;
int CBasicNode::visiter = 0;

int CBasicNode::MIN_NUM_NODE = 0;
int CBasicNode::MAX_NUM_NODE = 0;
int CBasicNode::INIT_NUM_NODE = 0;

int CBasicNode::DEFAULT_SLOT_CARRIER_SENSE = 0;  //不使用占空比工作时，默认等于 0

double CBasicNode::DEFAULT_DATA_RATE = 0;
int CBasicNode::SIZE_DATA = 0;

int CBasicNode::CAPACITY_BUFFER = 0;
int CBasicNode::CAPACITY_ENERGY = 0;
int CBasicNode::SPEED_TRANS = 0;
int CBasicNode::LIFETIME_SPOKEN_CACHE = 0;
CGeneralNode::_RECEIVE CBasicNode::MODE_RECEIVE = _loose;
CGeneralNode::_SEND CBasicNode::MODE_SEND = _dump;
CGeneralNode::_QUEUE CBasicNode::MODE_QUEUE = _fifo;


void CBasicNode::init() 
{
	trace = nullptr;
	dataRate = 0;
	atHotspot = nullptr;
	//timerCarrierSense = DEFAULT_SLOT_CARRIER_SENSE;
	//discovering = false;
	timeLastData = 0;
	timeDeath = 0;
	recyclable = true;
	capacityBuffer = CAPACITY_BUFFER;
	sumBufferRecord = 0;
	countBufferRecord = 0;

	generateID();
	loadTrace();
}

CBasicNode::CBasicNode() 
{
	init();
}

CBasicNode::CBasicNode(double dataRate) 
{
	init();
	this->dataRate = dataRate;
}

CBasicNode::~CBasicNode()
{
	if( trace != nullptr )
	{
		delete trace;
		trace = nullptr;
	}
}

//void CBasicNode::initNodes() 
//{
//	if( nodes.empty() && deadNodes.empty() )
//	{
//		for(int i = 0; i < INIT_NUM_NODE; ++i)
//		{
//			double dataRate = DEFAULT_DATA_RATE;
//			if(i % 5 == 0)
//				dataRate *= 5;
//			CBasicNode* node = new CBasicNode(dataRate);
//			node->init();
//		}
//		for(vector<CBasicNode &>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
//			CProphet::initDeliveryPreds(*inode);
//	}
//}

void CBasicNode::generateData(int currentTime) 
{
	int timeDataIncre = currentTime - timeLastData;
	int nData = int( timeDataIncre * dataRate / SIZE_DATA);
	if(nData > 0)
	{
		for(int i = 0; i < nData; ++i)
		{
			CData data(ID, currentTime, SIZE_DATA);
			this->buffer = CSortHelper::insertIntoSortedList(this->buffer, data, CSortHelper::ascendByTimeBirth, CSortHelper::descendByTimeBirth);
		}
		timeLastData = currentTime;
	}
	updateBufferStatus(currentTime);
}

bool CBasicNode::finiteEnergy() 
{
	return CAPACITY_ENERGY > 0;
}

void CBasicNode::checkDataByAck(vector<CData> ack)
{
	if( MODE_SEND == _dump )
		RemoveFromList(buffer, ack);
}

bool CBasicNode::hasSpokenRecently(CBasicNode & node, int currentTime) 
{
	if( CBasicNode::LIFETIME_SPOKEN_CACHE == 0 )
		return false;

	map<CBasicNode &, int>::iterator icache = spokenCache.find( node );
	if( icache == spokenCache.end() )
		return false;
	else if( ( currentTime - icache->second ) == 0
		|| ( currentTime - icache->second ) < CBasicNode::LIFETIME_SPOKEN_CACHE )
		return true;
	else
		return false;
}

void CBasicNode::addToSpokenCache(CBasicNode & node, int currentTime) 
{
	spokenCache.insert( pair<CBasicNode &, int>(node, currentTime) );
}

//vector<CData> CBasicNode::dropOverdueData(int currentTime) {
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


//手动将数据压入 buffer，不伴随其他任何操作
//注意：必须在调用此函数之后手动调用 updateBufferStatus() 检查溢出

void CBasicNode::pushIntoBuffer(vector<CData> datas)
{
	this->buffer = CSortHelper::insertIntoSortedList(this->buffer, datas, CSortHelper::ascendByTimeBirth, CSortHelper::descendByTimeBirth);
}

vector<CData> CBasicNode::removeDataByCapacity(vector<CData> &datas, int capacity, bool fromLeft)
{
	vector<CData> overflow;
	if( datas.size() <= capacity )
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

vector<CData> CBasicNode::dropDataIfOverflow()
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
//			//flash_cout << "######  ( Node " << this->ID << " drops " << myData.size() - capacityBuffer << " data )                    " << endl;
//			myData = vector<CData>( myData.end() - CAPACITY_BUFFER, myData.end() );
//		}		
//	}
//	else :

	myData = buffer;
	//myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByTimeBirth);
	overflow = removeDataByCapacity(myData, capacityBuffer, true);

	buffer = myData;
	return overflow;
}

vector<CData> CBasicNode::updateBufferStatus(int currentTime) 
{
	vector<CData> overflow = dropDataIfOverflow();

	return overflow;
}

vector<int> CBasicNode::updateSummaryVector() 
{
	if( buffer.size() > CAPACITY_BUFFER )
	{
		throw string("CBasicNode::updateSummaryVector() : Buffer isn't clean !");
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

void CBasicNode::updateStatus(int currentTime)
{
	if( this->time == currentTime )
		return;

	/****************  移入 CDutyCycle::updateStatus()  ****************/

	//int newTime = this->time;

	////计算能耗、更新工作状态
	//while( ( newTime + SLOT ) <= currentTime )
	//{
	//	newTime += SLOT;

	//	// 0 时间时初始化
	//	if( newTime <= 0 )
	//	{
	//		newTime = 0;
	//		this->time = 0;
	//		break;
	//	}

	//	if( timerSleep <= 0
	//	   && timerWake <= 0 )
	//	{
	//		throw string("CBasicNode::updateStatus() : timerSleep : " + STRING(timerSleep) + ", timerWake : " + STRING(timerWake) );
	//	}

	//	switch( state )
	//	{
	//		case _awake:
	//			consumeEnergy(CONSUMPTION_WAKE * SLOT);
	//			updateTimerWake(newTime);

	//			break;
	//		case _asleep:
	//			consumeEnergy(CONSUMPTION_SLEEP * SLOT);
	//			updateTimerSleep(newTime);

	//			break;
	//		default:
	//			break;
	//	}
	//	
	//	updateTimerOccupied(newTime);

	//}

	if( !isAlive() )
	{
		die(currentTime, false);
		return;
	}
	//sumTimeAlive += newTime - this->time;

	//生成数据
	if( currentTime <= DATATIME )
		generateData(currentTime);


	/**************************************  移入 CProphetNode  *************************************/

	//if( ROUTING_PROTOCOL == _prophet 
	//	 && (currentTime % CCTrace::SLOT_TRACE) == 0  )
	//	CProphet::decayDeliveryPreds(*this, currentTime);

	//更新坐标及时间戳
	if( ! trace->isValid(currentTime) )
	{
		die(currentTime, false);  //因 trace 信息终止而死亡的节点，无法回收
		return;
	}
	setLocation( trace->getLocation(currentTime) );
	setTime(currentTime);

}

void CBasicNode::recordBufferStatus() 
{
	if( timeDeath > 0 )
		return;

	sumBufferRecord += buffer.size();
	++countBufferRecord;
}

vector<CData> CBasicNode::getDataByRequestList(vector<int> requestList) const
{
	if( requestList.empty() || buffer.empty() )
		return vector<CData>();

	vector<CData> result;
	result = CData::GetItemsByID(buffer, requestList);
	return result;
}

int CBasicNode::getCapacityForward()
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

