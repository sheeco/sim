#pragma once

#include "RoutingProtocol.h"

using namespace std;


class Prophet :
	public CRoutingProtocol
{
private:

	//在特定时槽上发送数据
	//注意：必须在调用UpdateNodeStatus之后调用此函数
	static void SendData(int currentTime);


public:

	Prophet(){};
	~Prophet(){};

	static bool Operate(int currentTime);

};

