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

//int CNode::encounterActiveAtHotspot = 0;
//int CNode::encounterActive = 0;
int CNode::encounter = 0;
int CNode::visiter = 0;

vector<CNode*> CNode::allNodes;
vector<int> CNode::idNodes;


void CNode::init() 
{
	trace = nullptr;
	dataRate = 0;
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

inline bool CNode::setNodes(vector<CNode*> nodes)
{
	allNodes = nodes;
	allNodes = CSortHelper::mergeSort(allNodes, CSortHelper::ascendByID);
	idNodes.clear();
	for(CNode * inode : allNodes)
		idNodes.push_back(inode->getID());
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


vector<CNode*>& CNode::getAllNodes() 
{
	return allNodes;
}

int CNode::getNodeCount() 
{
	return allNodes.size();
}

vector<int> CNode::getIdNodes() 
{
	return idNodes;
}

bool CNode::finiteEnergy() 
{
	return getConfig<int>("node", "energy") > 0;
}

bool CNode::ifNodeExists(int id)
{
	for(vector<CNode *>::iterator inode = allNodes.begin(); inode != allNodes.end(); ++inode)
	{
		if((*inode)->getID() == id)
			return true;
	}
	return false;
}

CNode* CNode::getNodeByID(int id) 
{
	for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
	{
		if((*inode)->getID() == id)
			return *inode;
	}
	return nullptr;
}

double CNode::getSumEnergyConsumption() 
{
	double sumEnergyConsumption = 0;
	auto allNodes = getAllNodes();
	for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		sumEnergyConsumption += (*inode)->getEnergyConsumption();
	return sumEnergyConsumption;
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

vector<CNode*> CNode::restoreNodes(vector<CNode*> removedNodes, int n)
{
	vector<CNode*> restoredNodes;
	if(removedNodes.empty())
		return restoredNodes;

	// TODO: 恢复时重新充满能量？
	for(int i = allNodes.size(); i < allNodes.size() + n; ++i)
	{
		restoredNodes.push_back(removedNodes[0]);
		--n;
	}
	return restoredNodes;
}

vector<CNode*> CNode::removeNodes(int n)
{
	//FIXME: Random selected ?
	vector<CNode *>::iterator start = allNodes.begin();
	vector<CNode *>::iterator end = allNodes.end();
	vector<CNode *>::iterator fence = allNodes.begin();
	fence += allNodes.size() - n;
	vector<CNode *> leftNodes(start, fence);

	//Remove invalid positoins belonging to the deleted nodes
	vector<CNode *> removed(fence, end);
	return removed;
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
