#pragma once

#include "RoutingProtocol.h"

class Prophet :
	public CRoutingProtocol
{
private:

	//static void updatePredictability(CNode a, CNode b);
	//static void decayPredictability(CNode node);

	//���ض�ʱ���Ϸ�������
	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
	static void SendData(int currentTime);


public:

	Prophet(void){}
	~Prophet(void){};

	static bool Operate(int currentTime);
};

