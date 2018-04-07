#include "MANode.h"
#include "HAR.h"
#include "Configuration.h"


int CMANode::COUNT_ID = 0;  //从getConfig<int>("ma", "base_id")开始，差值等于当前实例总数
int CMANode::INIT_NUM_MA = INVALID;
int CMANode::MAX_NUM_MA = INVALID;
int CMANode::CAPACITY_BUFFER = INVALID;
double CMANode::SPEED = INVALID;
int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;

void CMANode::init()
{
	if(COUNT_ID == 0)
		COUNT_ID = getConfig<int>("ma", "base_id");

	fifo = getConfig<config::EnumQueueScheme>("node", "scheme_queue") == config::_fifo;
	setLocation(CSink::getSink()->getLocation());  //初始化 MA 位置在 sink 处
	speed = getConfig<int>("ma", "speed");
	atPoint = nullptr;
	waitingWindow = INVALID;
	waitingState = INVALID;
	capacityBuffer = getConfig<int>("ma", "buffer");
	returningToSink = false;
	busy = false;
	route = nullptr;
	atPoint = nullptr;
	time = time;
	generateID();
	setName("MA #" + STRING(this->getID()));
}

void CMANode::Init()
{
	INIT_NUM_MA = getConfig<int>("ma", "init_num");
	MAX_NUM_MA = getConfig<int>("ma", "max_num");
	CAPACITY_BUFFER = getConfig<int>("ma", "buffer");
	SPEED = getConfig<int>("ma", "speed");
}

CFrame * CMANode::sendRTSWithCapacity(int now)
{
	vector<CPacket*> packets;
	packets.push_back(new CCtrl(ID, now, CCtrl::_rts));
	packets.push_back(new CCtrl(ID, this->getBufferVacancy(), now, CCtrl::_capacity));

	CFrame* frame = new CFrame(*this, packets);

	return frame;
}
