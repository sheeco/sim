#pragma once

#ifndef __DUTY_CYCLE_H__
#define __DUTY_CYCLE_H__

#include "BasicNode.h"

class CDutyCycle :
	virtual public CBasicNode
{
private:

	double dutyCycle;

	//  [ ----------SLEEP----------> | ----WAKE----> )

	int SLOT_SLEEP;  //由SLOT_TOTAL和DC计算得到
	int SLOT_WAKE;  //由SLOT_TOTAL和DC计算得到

	//计时器：UNVALID(-1) 表示当前不处于此状态；0 表示此状态即将结束
	int timerSleep;
	int timerWake;
	int timerCarrierSense;  //距离载波侦听结束、邻居节点发现开始的剩余时间
	bool discovering;  //用于标记是否正在进行邻居节点发现，本次发现完毕将置false

	int sumTimeAwake;
	int sumTimeAlive;


protected:

	CDutyCycle();

	void init() override;


public:

	static int SLOT_TOTAL;
	static double DEFAULT_DUTY_CYCLE;  //不使用HDC，或者HDC中不在热点区域内时的占空比

	virtual ~CDutyCycle() = 0;

	void updateTimerWake(int time);
	void updateTimerSleep(int time);

	void Wake() override;
	void Sleep() override;

	void updateStatus(int currentTime) override;

	bool isAwake() const override
	{
		//将 timerOccupied 的下沿也认为是唤醒的
		return state == _awake /*|| timerOccupied == 0*/;
	}

	//判断是否处于邻居节点发现状态
	//注意：调用此函数之前必须确保已updateStatus
	bool isDiscovering() const
	{
		return discovering == true;
	}

	//标记开始邻居节点发现
	void startDiscovering()
	{
		this->discovering = true;
		this->timerCarrierSense = UNVALID;
	}

	//标记本次邻居节点发现已经完成
	void finishDiscovering()
	{
		this->discovering = false;
	}

	//发现过听之后，延迟载波侦听
	void delayDiscovering(int timeDelay)
	{
		if( timeDelay <= 0 )
			return;

		this->timerCarrierSense += timeDelay;
		this->discovering = false;
		//延迟后超出唤醒时限，则立即休眠
		if( timerCarrierSense >= timerWake )
			Sleep();
	}

	void delaySleep(int timeDelay)
	{
		if( timeDelay <= 0 )
			return;
		if( state != _awake )
			return;

		this->timerWake += timeDelay;
	}

	inline bool ifUseDutyCycle(double dc)
	{
		return EQUAL(dutyCycle, dc);
	}
	inline void setDutyCycle(double dc)
	{
		this->dutyCycle = dc;

		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(SLOT_TOTAL * dutyCycle);
		SLOT_SLEEP = SLOT_TOTAL - SLOT_WAKE;
		//DC 提高时
		if( oldSlotWake < SLOT_WAKE )
		{
			//唤醒状态下，延长唤醒时间
			if( isAwake() )
				timerWake += SLOT_WAKE - oldSlotWake;
			//休眠状态下，立即唤醒
			else
				Wake();
		}

	}

	void Overhear() override;

	//统计唤醒时间
	int getSumTimeAwake() const
	{
		return sumTimeAwake;
	}
	double getPercentTimeAwake() const
	{
		if( sumTimeAlive == 0 )
			return 0;
		return double(sumTimeAwake) / double(sumTimeAlive);
	}

};

#endif // __DUTY_CYCLE_H__