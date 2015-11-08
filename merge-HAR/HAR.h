#pragma once

#include "GlobalParameters.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Node.h"
#include "Sink.h"
#include "MANode.h"
#include "Route.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"

extern bool DO_IHAR;
extern bool DO_COMP;
extern bool HEAT_RATIO_EXP;
extern bool HEAT_RATIO_LN;

extern int currentTime;
extern int startTimeForHotspotSelection;
extern double CO_HOTSPOT_HEAT_A1;
extern double CO_HOTSPOT_HEAT_A2;
extern double BETA;
extern int MAX_MEMORY_TIME;
extern int NUM_NODE;
extern double PROB_DATA_FORWARD;
extern int DATATIME;
extern int RUNTIME;

extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

extern string logInfo;
extern ofstream debugInfo;

class HAR
{
private:
	CSink* m_sink;
	vector<CNode> m_nodes;
	vector<CHotspot *> m_hotspots;
	vector<CRoute> m_routes;  //即hotspot class
	vector<CMANode> m_MANodes;

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
	double getHotspotHeat(CHotspot *hotspot);
	double getWaitingTime(CHotspot *hotspot);
	double getSumGenerationRate(vector<int> nodes);  //计算ge的sum，同一个node不应重复计算
	double getSumGenerationRate(vector<int> nodes_a, vector<int> nodes_b);
	double getTimeIncrementForInsertion(CRoute route, int front, CHotspot *hotspot);
	double calculateRatioForInsertion(CRoute route, int front, CHotspot *hotspot);
	//对一条route进行优化（TSP 最近邻居算法）
	void OptimizeRoute(CRoute &route);
	//计算相关统计数据
	double calculateEDTime();

	//用于最终debug结果的统计和记录
	double getAverageHotspotCost()
	{
		if( HOTSPOT_COST_COUNT == 0 )
			return -1;
		else
			return (double)HOTSPOT_COST_SUM / (double)HOTSPOT_COST_COUNT;
	}
	double getAverageMACost()
	{
		if( MA_COST_COUNT == 0 )
			return -1;
		else
			return (double)MA_COST_SUM / (double)MA_COST_COUNT;
	}
	double getAverageMAWaypoint()
	{
		if( MA_WAYPOINT_COUNT == 0 )
			return -1;
		else
			return MA_WAYPOINT_SUM / MA_WAYPOINT_COUNT;
	}
	double getAverageMergePercent()
	{
		if( MERGE_PERCENT_COUNT == 0 )
			return -1;
		else
			return MERGE_PERCENT_SUM / MERGE_PERCENT_COUNT;
	}
	double getAverageOldPercent()
	{
		if( OLD_PERCENT_COUNT == 0 )
			return -1;
		else
			return OLD_PERCENT_SUM / OLD_PERCENT_COUNT;
	}
	double getAverageSimilarityRatio()
	{
		if( SIMILARITY_RATIO_COUNT == 0 )
			return -1;
		else
			return SIMILARITY_RATIO_SUM / SIMILARITY_RATIO_COUNT;
	}	

public:
	HAR(void)
	{
		for(int i = 0; i < NUM_NODE; i++)
		{
			double generationRate = RATE_DATA_GENERATE;
			if(i % 5 == 0)
				generationRate *= 5;
			CNode node(generationRate, BUFFER_CAPACITY_NODE);
			node.setID(i);
			m_nodes.push_back(node);
		}
		CNode sink(0, BUFFER_CAPACITY_SINK);
		sink.setID(SINK_ID);
		sink.setLocation(SINK_X, SINK_Y, 0);
		m_sink = CSink::getSink();
	}

	~HAR(void)
	{}

	inline int getNClass()
	{
		return m_routes.size();
	}

	//在限定范围内随机增删一定数量的node
	void ChangeNodeNumber();
	//更新所有node的位置（而不是position）
	void UpdateNodeLocations();
	//执行热点选取
	void HotspotSelection();
	//根据选取出的hotspot集合得到hotspot class的集合
	void HotspotClassification();
	//对初步得到的hotspot class进行路径优化（TSP 最近邻居算法）
	void MANodeRouteDesign();
	//在特定时槽上产生数据
	void GenerateData();
	//在特定时槽上发送数据
	void SendData();
	//打印相关信息到文件
	void PrintInfo();

	//比较此次热点选取的结果与上一次选取结果之间的相似度
	void CompareWithOldHotspots();

};

