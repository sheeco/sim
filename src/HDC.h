#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"


//HDCЭ��(�����ȵ��Duty Cycle)
class CHDC :
	virtual public CMacProtocol
{
private:

	static double HOTSPOT_DUTY_RATE;
	CHDC();

	//�������Node�����λ���ȵ����򣬸���ռ�ձ�
	//ע�⣺�����ڵ��� UpdateAtHotspotForNodes() ֮�����
	static void UpdateDutyCycleForNodes(int now);
	static bool isUsingHotspotDutyCycle(CNode* node);


public:

	~CHDC();

	static bool Init();
	static bool Prepare(int now);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int now);
	static void PrintFinal(int now);

};

#endif // __HDC_H__
