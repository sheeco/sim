#pragma once

#include "RoutingProtocol.h"

class Prophet :
	public CRoutingProtocol
{
private:

	//在特定时槽上发送数据
	//注意：必须在调用UpdateNodeStatus之后调用此函数
	static void SendData(int currentTime);


public:

	Prophet(void){}
	~Prophet(void){};

	static bool Operate(int currentTime);
};

