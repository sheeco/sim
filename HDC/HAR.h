#pragma once

#include "GlobalParameters.h"
#include "Hotspot.h"
#include "Route.h"
#include "RoutingProtocol.h"
#include "Node.h"

using namespace std;

extern bool TEST_DYNAMIC_NUM_NODE;
extern bool TEST_HOTSPOT_SIMILARITY;
extern int NUM_NODE;
extern int RUNTIME;
extern int DATATIME;
extern double PROB_DATA_FORWARD;
extern int MAX_MEMORY_TIME;
extern _RoutingProtocol ROUTING_PROTOCOL;

/***************************** xHAR ********************************/
extern int startTimeForHotspotSelection;
extern double BETA;
extern int MIN_WAITING_TIME;
extern double CO_HOTSPOT_HEAT_A1;
extern double CO_HOTSPOT_HEAT_A2;

extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;
extern bool TEST_LEARN;

extern string INFO_LOG;
extern ofstream debugInfo;


class HAR : 
	public CRoutingProtocol
{
private:

	static vector<CHotspot *> m_hotspots;
	static vector<CRoute> m_routes;  //即hotspot class
	//vector<CMANode> m_MANodes;

	//用于计算最终取出的热点总数的历史平均值
	static int HOTSPOT_COST_SUM;
	static int HOTSPOT_COST_COUNT;
	//用于计算所需MA个数的历史平均值
	static int MA_COST_SUM;
	static int MA_COST_COUNT;
	//用于计算MA的平均路点（热点）个数的历史平均值
	static double MA_WAYPOINT_SUM;
	static int MA_WAYPOINT_COUNT;
	//用于计算最终选取出的热点集合中，merge和old热点的比例的历史平均值
	static double MERGE_PERCENT_SUM;
	static int MERGE_PERCENT_COUNT;
	static double OLD_PERCENT_SUM;
	static int OLD_PERCENT_COUNT;
	//用于计算热点前后相似度的历史平均值
	static double SIMILARITY_RATIO_SUM;
	static int SIMILARITY_RATIO_COUNT;

	/** 辅助函数 **/

	//用于hotspot classification
	static double getHotspotHeat(CHotspot *hotspot);
	static double getWaitingTime(CHotspot *hotspot);
	static double getSumGenerationRate(vector<int> nodes);  //计算ge的sum，同一个node不应重复计算
	static double getSumGenerationRate(vector<int> nodes_a, vector<int> nodes_b);
	static double getTimeIncrementForInsertion(CRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(CRoute route, int front, CHotspot *hotspot);
	//对一条route进行优化（TSP 最近邻居算法）
	static void OptimizeRoute(CRoute &route);
	//计算相关统计数据
	static double calculateEDTime();

	static inline double getAverageMACost()
	{
		if( MA_COST_COUNT == 0 )
			return -1;
		else
			return double(MA_COST_SUM) / double(MA_COST_COUNT);
	}

	static inline double getAverageMAWaypoint()
	{
		if( MA_WAYPOINT_COUNT == 0 )
			return -1;
		else
			return MA_WAYPOINT_SUM / MA_WAYPOINT_COUNT;
	}


	//在每一次贪婪选择之前调用，将从CHotspot::oldSelectedHotspots中寻找投递计数为0的热点删除放入CHotspot::deletedHotspots
	//并删除其对应的所有position放入CPosition::deletedPositions
	static void DecayPositionsWithoutDeliveryCount(int currentTime);
	//根据选取出的hotspot集合得到hotspot class的集合
	static void HotspotClassification(int currentTime);
	//对初步得到的hotspot class进行路径优化（TSP 最近邻居算法）
	static void MANodeRouteDesign(int currentTime);
	//在特定时槽上发送数据
	static void SendData(int currentTime);

	//比较此次热点选取的结果与上一次选取结果之间的相似度
	static void CompareWithOldHotspots(int currentTime);


public:

	HAR(){};
	~HAR(){};


	//用于最终debug结果的统计和记录
	static inline double getAverageHotspotCost()
	{
		if( HOTSPOT_COST_COUNT == 0 )
			return -1;
		else
			return double(HOTSPOT_COST_SUM) / double(HOTSPOT_COST_COUNT);
	}

	static inline double getAverageMergePercent()
	{
		if( MERGE_PERCENT_COUNT == 0 )
			return 0.0;
		else
			return MERGE_PERCENT_SUM / MERGE_PERCENT_COUNT;
	}

	static inline double getAverageOldPercent()
	{
		if( OLD_PERCENT_COUNT == 0 )
			return 0.0;
		else
			return OLD_PERCENT_SUM / OLD_PERCENT_COUNT;
	}

	static inline double getAverageSimilarityRatio()
	{
		if( SIMILARITY_RATIO_COUNT == 0 )
			return -1;
		else
			return SIMILARITY_RATIO_SUM / SIMILARITY_RATIO_COUNT;
	}	

	//执行热点选取
	static void HotspotSelection(int currentTime);
	//打印相关信息到文件
	static void PrintInfo(int currentTime);

	static bool Operate(int currentTime)
	{
		if( ROUTING_PROTOCOL != _har )
			return false;

		//Node Number Test:
		if( TEST_DYNAMIC_NUM_NODE )
			ChangeNodeNumber(currentTime);

		if( ! CNode::hasNodes(currentTime) )
			return false;

		UpdateNodeStatus(currentTime);

		HotspotSelection(currentTime);

		SendData(currentTime);

		PrintInfo(currentTime);

		return true;
	}

};

