#include "DutyCycle.h"
#include "MacProtocol.h"
#include "RoutingProtocol.h"

int CDutyCycle::SLOT_TOTAL = 0;
double CDutyCycle::DEFAULT_DUTY_CYCLE = 0;


CDutyCycle::CDutyCycle()
{
	init();
}

CDutyCycle::~CDutyCycle()
{
}

// TODO: test call 4 this func
void CDutyCycle::init()
{
	CBasicNode::init();

	dutyCycle = DEFAULT_DUTY_CYCLE;
	SLOT_WAKE = int(dutyCycle * SLOT_TOTAL);
	SLOT_SLEEP = SLOT_TOTAL - SLOT_WAKE;
	timerCarrierSense = UNVALID;
	discovering = false;
	sumTimeAwake = 0;
	sumTimeAlive = 0;
	if( SLOT_WAKE == 0 )
	{
		state = _asleep;
		timerWake = UNVALID;
		if( CMacProtocol::SYNC_DC )
			timerSleep = SLOT_SLEEP;
		else
			timerSleep = RandomInt(1, SLOT_SLEEP);
	}
	else
	{
		state = _awake;
		timerSleep = UNVALID;
		if( CMacProtocol::SYNC_DC )
			timerWake = SLOT_WAKE;
		else
			timerWake = RandomInt(1, SLOT_WAKE);
		timerCarrierSense = RandomInt(0, CRoutingProtocol::getTimeWindowTrans());
	}
}

void CDutyCycle::Overhear()
{
	CGeneralNode::Overhear();

	//继续载波侦听
	//时间窗内随机值 / 立即休眠？
	delayDiscovering(CRoutingProtocol::getTimeWindowTrans());
}

void CDutyCycle::Wake()
{
	//Always On
	if( SLOT_WAKE <= 0 )
	{
		Sleep();
		return;
	}
	state = _awake;
	timerSleep = UNVALID;
	timerWake = SLOT_WAKE;
	timerCarrierSense = RandomInt(0, CRoutingProtocol::getTimeWindowTrans());
}

void CDutyCycle::Sleep()
{
	//Always On
	if( SLOT_SLEEP <= 0 )
	{
		Wake();
		return;
	}
	state = _asleep;
	timerWake = UNVALID;
	timerCarrierSense = UNVALID;
	timerSleep = SLOT_SLEEP;
}

void CDutyCycle::updateStatus(int currentTime)
{
	if( this->time == currentTime )
		return;

	int newTime = this->time;

	//计算能耗、更新工作状态
	while( ( newTime + SLOT ) <= currentTime )
	{
		newTime += SLOT;

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
			throw string("CDutyCycle::updateStatus() : timerSleep : " + STRING(timerSleep) + ", timerWake : " + STRING(timerWake));
		}

		switch( state )
		{
			case _awake:
				consumeEnergy(CONSUMPTION_WAKE * SLOT);
				updateTimerWake(newTime);

				break;
			case _asleep:
				consumeEnergy(CONSUMPTION_SLEEP * SLOT);
				updateTimerSleep(newTime);

				break;
			default:
				break;
		}

		//updateTimerOccupied(newTime);

	}

	CBasicNode::updateStatus(currentTime);
}

void CDutyCycle::updateTimerWake(int time)
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

void CDutyCycle::updateTimerSleep(int time)
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

