#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"


class CHDC :
	public CMacProtocol
{
private:

	CHDC();

	//�������Node�����λ���ȵ����򣬸���ռ�ձ�
	//ע�⣺�����ڵ��� UpdateAtHotspotForNodes() ֮�����
	static void UpdateDutyCycleForNodes(int currentTime);


public:

	~CHDC();

	static bool Prepare(int currentTime);
	static bool Operate(int currentTime);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

#endif // __HDC_H__
