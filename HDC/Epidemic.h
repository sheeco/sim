#pragma once

#include "RoutingProtocol.h"
#include "GlobalParameters.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Node.h"
#include "Sink.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "HDC.h"

extern int NUM_NODE;
extern double PROB_DATA_FORWARD;
extern int DATATIME;
extern int RUNTIME;

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
	Epidemic(void)
	{

	}

	~Epidemic(void){};

	static void Operate(int currentTime)
	{
		if( currentTime % SLOT_MOBILITYMODEL == 0 )
			UpdateNodeStatus(currentTime);
		
		if( currentTime % SLOT_DATA_GENERATE == 0 )
			GenerateData(currentTime);

		if( currentTime % SLOT_DATA_SEND == 0 )
			SendData(currentTime);

		if( currentTime % SLOT_RECORD_INFO )
			PrintInfo(currentTime);
	}

};

