#pragma once

#include "GlobalParameters.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Node.h"
#include "Sink.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "MacProtocol.h"

using namespace std;

extern bool DO_IHAR;
extern bool TEST_HOTSPOT_SIMILARITY;

extern int startTimeForHotspotSelection;
extern double CO_HOTSPOT_HEAT_A1;
extern double CO_HOTSPOT_HEAT_A2;
extern double BETA;
extern int MAX_MEMORY_TIME;
extern int NUM_NODE;
extern int DATATIME;
extern int RUNTIME;

extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

//extern string logInfo;
//extern ofstream debugInfo;

class CHDC :
	public CMacProtocol
{
private:

	//用于计算最终取出的热点总数的历史平均值
	static int HOTSPOT_COST_SUM;
	static int HOTSPOT_COST_COUNT;
	//用于计算最终选取出的热点集合中，merge和old热点的比例的历史平均值
	static double MERGE_PERCENT_SUM;
	static int MERGE_PERCENT_COUNT;
	static double OLD_PERCENT_SUM;
	static int OLD_PERCENT_COUNT;
	//用于计算热点前后相似度的历史平均值
	static double SIMILARITY_RATIO_SUM;
	static int SIMILARITY_RATIO_COUNT;

	////更新所有node的位置（而不是position）
	//void UpdateNodeLocations();
	//执行热点选取
	static void HotspotSelection(int currentTime);
	//比较此次热点选取的结果与上一次选取结果之间的相似度
	static void CompareWithOldHotspots(int currentTime);
	//打印相关信息到文件
	static void PrintInfo(int currentTime);


public:

	CHDC(void);
	~CHDC(void);

	//检查所有Node，如果位于热点区域，更新占空比
	static void UpdateDutyCycleForNodes(int currentTime);

	static void Operate(int currentTime)
	{
		if( currentTime % SLOT_HOTSPOT_UPDATE )
			HotspotSelection(currentTime);

		if( currentTime >= startTimeForHotspotSelection && currentTime % SLOT_RECORD_INFO )
			PrintInfo(currentTime);
	}

};

