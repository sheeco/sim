#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"


class CHDC :
	virtual public CMacProtocol
{
private:

	CHDC();

	//�������Node�����λ���ȵ����򣬸���ռ�ձ�
	//ע�⣺�����ڵ��� UpdateAtHotspotForNodes() ֮�����
	static void UpdateDutyCycleForNodes(int now);


public:

	~CHDC();

	static bool Prepare(int now);
	static bool Operate(int now);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int now);
	static void PrintFinal(int now);

};

#endif // __HDC_H__
