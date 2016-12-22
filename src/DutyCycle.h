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

	int SLOT_SLEEP;  //��SLOT_TOTAL��DC����õ�
	int SLOT_WAKE;  //��SLOT_TOTAL��DC����õ�

	//��ʱ����UNVALID(-1) ��ʾ��ǰ�����ڴ�״̬��0 ��ʾ��״̬��������
	int timerSleep;
	int timerWake;
	int timerCarrierSense;  //�����ز������������ھӽڵ㷢�ֿ�ʼ��ʣ��ʱ��
	bool discovering;  //���ڱ���Ƿ����ڽ����ھӽڵ㷢�֣����η�����Ͻ���false

	int sumTimeAwake;
	int sumTimeAlive;


protected:

	CDutyCycle();

	void init() override;


public:

	static int SLOT_TOTAL;
	static double DEFAULT_DUTY_CYCLE;  //��ʹ��HDC������HDC�в����ȵ�������ʱ��ռ�ձ�

	virtual ~CDutyCycle() = 0;

	void updateTimerWake(int time);
	void updateTimerSleep(int time);

	void Wake() override;
	void Sleep() override;

	void updateStatus(int currentTime) override;

	bool isAwake() const override
	{
		//�� timerOccupied ������Ҳ��Ϊ�ǻ��ѵ�
		return state == _awake /*|| timerOccupied == 0*/;
	}

	//�ж��Ƿ����ھӽڵ㷢��״̬
	//ע�⣺���ô˺���֮ǰ����ȷ����updateStatus
	bool isDiscovering() const
	{
		return discovering == true;
	}

	//��ǿ�ʼ�ھӽڵ㷢��
	void startDiscovering()
	{
		this->discovering = true;
		this->timerCarrierSense = UNVALID;
	}

	//��Ǳ����ھӽڵ㷢���Ѿ����
	void finishDiscovering()
	{
		this->discovering = false;
	}

	//���ֹ���֮���ӳ��ز�����
	void delayDiscovering(int timeDelay)
	{
		if( timeDelay <= 0 )
			return;

		this->timerCarrierSense += timeDelay;
		this->discovering = false;
		//�ӳٺ󳬳�����ʱ�ޣ�����������
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
		//DC ���ʱ
		if( oldSlotWake < SLOT_WAKE )
		{
			//����״̬�£��ӳ�����ʱ��
			if( isAwake() )
				timerWake += SLOT_WAKE - oldSlotWake;
			//����״̬�£���������
			else
				Wake();
		}

	}

	void Overhear() override;

	//ͳ�ƻ���ʱ��
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