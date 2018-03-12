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

int CNode::COUNT_ID = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������

vector<CNode*> CNode::allNodes;
vector<CNode*> CNode::aliveNodes;
vector<CNode*> CNode::deadNodes;
vector<int> CNode::idNodes;
map<int, CNode*> CNode::mapAllNodes;

int CNode::LIFETIME_COMMUNICATION_HISROTY = INVALID;
double CNode::DEFAULT_DATA_RATE = INVALID;
int CNode::DEFAULT_CAPACITY_BUFFER = INVALID;
int CNode::DEFAULT_CAPACITY_ENERGY = INVALID;
int CNode::WORK_CYCLE = INVALID;
double CNode::DEFAULT_DUTY_RATE = INVALID;

int CNode::encounter = 0;
int CNode::visiter = 0;


void CNode::init() 
{
	fifo = getConfig<config::EnumQueueScheme>("node", "scheme_queue") == config::_fifo;
	trace = nullptr;
	dataRate = INVALID;
	//timerCarrierSense = getConfig<int>("mac", "cycle_carrier_sense");
	//discovering = false;
	timeLastData = INVALID;
	timeDeath = INVALID;
	capacityBuffer = DEFAULT_CAPACITY_BUFFER;
	capacityEnergy = DEFAULT_CAPACITY_ENERGY;
	sumTimeAwake = 0;
	sumTimeAlive = 0;
	sumBufferRecord = 0;
	countBufferRecord = 0;
	dutyCycle = DEFAULT_DUTY_RATE;
	SLOT_WAKE = int( dutyCycle * WORK_CYCLE );
	SLOT_SLEEP = WORK_CYCLE - SLOT_WAKE;
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
	dropDataIfOverflow();
}

int CNode::getCountAliveNodes() 
{
	return allNodes.size();
}

bool CNode::finiteEnergy() 
{
	return DEFAULT_CAPACITY_ENERGY > 0;
}

double CNode::getSumEnergyConsumption() 
{
	double sumEnergyConsumption = 0;
	auto allNodes = getAllNodes();
	for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		sumEnergyConsumption += (*inode)->getEnergyConsumption();
	return sumEnergyConsumption;
}

void CNode::dropDataByAck(vector<CData> ack)
{
	RemoveFromList(buffer, ack);
}

void CNode::Overhear(int now)
{
	CGeneralNode::Overhear(now);

	//�����ز�����
	//ʱ�䴰�����ֵ / �������ߣ�
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

CFrame* CNode::sendRTSWithCapacityAndIndex(int now)
{
	vector<CPacket*> packets;
	
	packets.push_back( new CCtrl(ID, now, getConfig<int>("data", "size_ctrl"), CCtrl::_rts) );
	packets.push_back( new CCtrl(ID, this->getBufferVacancy(), now, getConfig<int>("data", "size_ctrl"), CCtrl::_capacity) );
	packets.push_back( new CCtrl(ID, now, getConfig<int>("data", "size_ctrl"), CCtrl::_index) );
	CFrame* frame = new CFrame(*this, packets);

	return frame;	
}

bool CNode::hasCommunicatedRecently(int nodeId, int now)
{
	if( LIFETIME_COMMUNICATION_HISROTY <= 0 )
		return false;

	map<int, int>::iterator icache = communicationHistory.find(nodeId);
	if( icache == communicationHistory.end() )
		return false;
	else if( ( now - icache->second ) < LIFETIME_COMMUNICATION_HISROTY )
		return true;
	else
		return false;
}

void CNode::updateCommunicationHistory(int nodeId, int now)
{
	communicationHistory.insert(pair<int, int>(nodeId, now));
}

vector<CNode*> CNode::restoreNodes(vector<CNode*> removedNodes, int n)
{
	vector<CNode*> restoredNodes;
	if(removedNodes.empty())
		return restoredNodes;

	// TODO: �ָ�ʱ���³���������
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

	//�����ܺġ����¹���״̬
	while( ( newTime + getConfig<int>("simulation", "slot") ) <= now )
	{
		newTime += getConfig<int>("simulation", "slot");

		// 0 ʱ��ʱ��ʼ��
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

	//��������
	if( now <= getConfig<int>("simulation", "datatime") )
		generateData(now);

	//�������꼰ʱ���
	if( ! trace->isValid(now) )
	{
		die(now);
		CPrintHelper::PrintBrief(now, this->getName() + " dies of trace exhaustion.");
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

