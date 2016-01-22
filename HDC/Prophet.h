#pragma once

#include "RoutingProtocol.h"

class Prophet :
	public CRoutingProtocol
{
private:

	//static void updatePredictability(CNode a, CNode b);
	//static void decayPredictability(CNode node);

	//更新所有node的坐标、占空比和工作状态
	static void UpdateNodeStatus(int currentTime);
	//在特定时槽上产生数据
	static void GenerateData(int currentTime);
	//在特定时槽上发送数据
	//注意：必须在调用UpdateNodeStatus之后调用此函数
	static void SendData(int currentTime);
	//打印相关信息到文件
	static void PrintInfo(int currentTime);


public:

	Prophet(void){}
	~Prophet(void){};

	static bool Operate(int currentTime);
};

