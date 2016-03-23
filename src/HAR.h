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
	static int SUM_HOTSPOT_COST;
	static int COUNT_HOTSPOT_COST;
	//用于计算所需MA个数的历史平均值
	static int SUM_MA_COST;
	static int COUNT_MA_COST;
	//用于计算MA的平均路点（热点）个数的历史平均值
	static double SUM_WAYPOINT_PER_MA;
	static int COUNT_WAYPOINT_PER_MA;
	//用于计算最终选取出的热点集合中，merge和old热点的比例的历史平均值
	static double SUM_PERCENT_MERGE;
	static int COUNT_PERCENT_MERGE;
	static double SUM_PERCENT_OLD;
	static int COUNT_PERCENT_OLD;
	//用于计算热点前后相似度的历史平均值
	static double SUM_SIMILARITY_RATIO;
	static int COUNT_SIMILARITY_RATIO;

	/** 辅助函数 **/

	//用于hotspot classification
	static double getHotspotHeat(CHotspot *hotspot);
	static double getWaitingTime(int currentTime, CHotspot *hotspot);
	static double getSumDataRate(vector<int> nodes);  //计算ge的sum，同一个node不应重复计算
	static double getTimeIncreForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot);
	//对一条route进行优化（TSP 最近邻居算法）
	static void OptimizeRoute(CRoute &route);
	//计算相关统计数据
	static double calculateEDTime(int currentTime);

	static inline double getAverageMACost()
	{
		if( COUNT_MA_COST == 0 )
			return -1;
		else
			return double(SUM_MA_COST) / double(COUNT_MA_COST);
	}

	static inline double getAverageMAWaypoint()
	{
		if( COUNT_WAYPOINT_PER_MA == 0 )
			return -1;
		else
			return SUM_WAYPOINT_PER_MA / COUNT_WAYPOINT_PER_MA;
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
	static int LIFETIME_POSITION;

	/********************************* merge-HAR ***********************************/

	static int MIN_WAITING_TIME;  //add minimum waiting time to each hotspot
	static bool TEST_BALANCED_RATIO;
//	static bool TEST_LEARN;
//	static double MIN_POSITION_WEIGHT;

	HAR(){};
	~HAR(){};


	//用于最终final结果的统计和记录
	static inline double getAverageHotspotCost()
	{
		if( COUNT_HOTSPOT_COST == 0 )
			return -1;
		else
			return double(SUM_HOTSPOT_COST) / double(COUNT_HOTSPOT_COST);
	}

	static inline double getAveragePercentMerge()
	{
		if( COUNT_PERCENT_MERGE == 0 )
			return 0.0;
		else
			return SUM_PERCENT_MERGE / COUNT_PERCENT_MERGE;
	}

	static inline double getAveragePercentOld()
	{
		if( COUNT_PERCENT_OLD == 0 )
			return 0.0;
		else
			return SUM_PERCENT_OLD / COUNT_PERCENT_OLD;
	}

	static inline double getAverageSimilarityRatio()
	{
		if( COUNT_SIMILARITY_RATIO == 0 )
			return -1;
		else
			return SUM_SIMILARITY_RATIO / COUNT_SIMILARITY_RATIO;
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

