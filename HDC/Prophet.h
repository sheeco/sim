#pragma once

#include "RoutingProtocol.h"

using namespace std;


class Prophet :
	public CRoutingProtocol
{
private:

	//���ض�ʱ���Ϸ�������
	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
	static void SendData(int currentTime);


public:

	Prophet(){};
	~Prophet(){};

	static bool Operate(int currentTime);

};

