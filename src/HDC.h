#pragma once

#include "MacProtocol.h"


class CHDC :
	public CMacProtocol
{
private:

	CHDC();

	//�������Node�����λ���ȵ����򣬸���ռ�ձ�
	static void UpdateDutyCycleForNodes(int currentTime);


public:

	~CHDC();

	static bool Operate(int currentTime);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

