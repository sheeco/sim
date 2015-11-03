#pragma once

#include "GlobalParameters.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Node.h"
#include "Sink.h"
#include "MANode.h"
#include "Route.h"

extern bool DO_IHAR;
extern int startTimeForHotspotSelection;
extern double CO_HOTSPOT_HEAT_A1;
extern double CO_HOTSPOT_HEAT_A2;
extern double BETA;
extern int NUM_NODE;
extern int DATATIME;
extern int RUNTIME;

extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

extern string logInfo;

class HAR
{
private:
	CSink* m_sink;
	vector<CNode> m_nodes;
	vector<CHotspot *> m_hotspots;
	vector<CRoute> m_routes;  //即hotspot class
	vector<CMANode> m_MANodes;

	/** 辅助函数 **/

	//用于hotspot classification
	double getHotspotHeat(CHotspot *hotspot);
	double getWaitingTime(CHotspot *hotspot, int time);
	double getSumGenerationRate(vector<int> nodes);  //计算ge的sum，同一个node不应重复计算
	double getSumGenerationRate(vector<int> nodes_a, vector<int> nodes_b);
	double getTimeIncrementForInsertion(CRoute route, int front, CHotspot *hotspot, int time);
	double calculateRatioForInsertion(CRoute route, int front, CHotspot *hotspot, int time);
	//对一条route进行优化（TSP 最近邻居算法）
	void OptimizeRoute(CRoute &route);
	//计算相关统计数据
	double getAverageEnergyConsumption()
	{
		if(CData::getDataArrivalCount() == 0)
			return 0;
		else
			return ( CNode::getEnergyConsumption() + CMANode::getEnergyConsumption() ) / CData::getDataArrivalCount();
	}
	double calculateEDTime(int time);


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

//	void HotspotSelection(int time);

	inline int getNClass()
	{
		return m_routes.size();
	}
	
	//在限定范围内随机增删一定数量的node
	void ChangeNodeNumber(int time);
	//更新所有node的位置（而不是position）
	void UpdateNodeLocations(int time);
	//根据所给hotspot集合得到hotspot class的集合
	void HotspotClassification(int time, vector<CHotspot *> hotspots);
	//对初步得到的hotspot class进行路径优化（TSP 最近邻居算法）
	void MANodeRouteDesign(int time);
	//在特定时槽上产生数据
	void GenerateData(int time);
	//在特定时槽上发送数据
	void SendData(int time);
	//打印相关信息到文件
	void PrintInfo(int time);

};

