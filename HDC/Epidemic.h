#pragma once

#include "GlobalParameters.h"
#include "RoutingProtocol.h"

extern int NUM_NODE;
extern double PROB_DATA_FORWARD;
extern int DATATIME;
extern int RUNTIME;
extern bool DO_HDC;

extern string logInfo;
extern ofstream debugInfo;

class Epidemic :
	public CRoutingProtocol
{
private:

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

	static int MAX_QUEUE_SIZE;  //同意存储的来自其他节点的data的最大总数，超过该数目将丢弃（是否在Request之前检查？）默认值等于buffer容量
	static int SPOKEN_MEMORY;  //在这个时间内交换过数据的节点暂时不再交换数据

	Epidemic(void)
	{

	}

	~Epidemic(void){};

	static bool Operate(int currentTime);

};

