#pragma once

#include "Hotspot.h"
#include "Route.h"
#include "RoutingProtocol.h"


// TODO: rewrite
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
	static double getWaitingTime(int currentTime, CHotspot *hotspot);
	static double getSumGenerationRate(vector<int> nodes);  //计算ge的sum，同一个node不应重复计算
	static double getTimeIncrementForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot);
	//对一条route进行优化（TSP 最近邻居算法）
	static void OptimizeRoute(CRoute &route);
	//计算相关统计数据
	static double calculateEDTime(int currentTime);

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


	//根据选取出的hotspot集合得到hotspot class的集合
	static void HotspotClassification(int currentTime);
	//对初步得到的hotspot class进行路径优化（TSP 最近邻居算法）
	static void MANodeRouteDesign(int currentTime);
//	//在特定时槽上发送数据
//	static void SendData(int currentTime);

	//比较此次热点选取的结果与上一次选取结果之间的相似度
	static void CompareWithOldHotspots(int currentTime);

//	//在每一次贪婪选择之前调用，将从CHotspot::oldSelectedHotspots中寻找投递计数为0的热点删除放入CHotspot::deletedHotspots
//	//并删除其对应的所有position放入CPosition::deletedPositions
//	static void DecayPositionsWithoutDeliveryCount(int currentTime);


public:

	static double BETA;  //ratio for true hotspot
//	static double GAMMA;  //ratio for HotspotsAboveAverage
	static double CO_HOTSPOT_HEAT_A1;
	static double CO_HOTSPOT_HEAT_A2;

	/************************************ IHAR ************************************/

	static double LAMBDA;
	static int MAX_MEMORY_TIME;

	/********************************* merge-HAR ***********************************/

	static int MIN_WAITING_TIME;  //add minimum waiting time to each hotspot
	static bool TEST_BALANCED_RATIO;
//	static bool TEST_LEARN;
//	static double MIN_POSITION_WEIGHT;

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
	//打印热点选取相关信息到文件
	static void PrintHotspotInfo(int currentTime);
	//打印相关信息到文件
	static void PrintInfo(int currentTime);

	static void PrintFinal(int currentTime);

	static bool Operate(int currentTime);

};

