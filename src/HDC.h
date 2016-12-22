#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"
#include "DutyCycle.h"


class CHDC :
	virtual public CMacProtocol
{
private:

	CHDC();

	//���ȵ㴦��� dc
	static void raiseDutyCycle(CDutyCycle* node);
	//�ڷ��ȵ㴦���� dc
	static void resetDutyCycle(CDutyCycle* node);
	//�������Node�����λ���ȵ����򣬸���ռ�ձ�
	//ע�⣺�����ڵ��� UpdateAtHotspotForNodes() ֮�����
	static void UpdateDutyCycleForNodes(int currentTime);


public:

	static double HOTSPOT_DUTY_CYCLE;  //HDC���ȵ������ڵ�ռ�ձ�

	~CHDC();

	static bool Prepare(int currentTime);
	static bool Operate(int currentTime);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

#endif // __HDC_H__
