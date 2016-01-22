#pragma once

#include "RoutingProtocol.h"

class Prophet :
	public CRoutingProtocol
{
private:

	//static void updatePredictability(CNode a, CNode b);
	//static void decayPredictability(CNode node);

	//��������node�����ꡢռ�ձȺ͹���״̬
	static void UpdateNodeStatus(int currentTime);
	//���ض�ʱ���ϲ�������
	static void GenerateData(int currentTime);
	//���ض�ʱ���Ϸ�������
	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
	static void SendData(int currentTime);
	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);


public:

	Prophet(void){}
	~Prophet(void){};

	static bool Operate(int currentTime);
};

