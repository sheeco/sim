#pragma once

#ifndef __HAR_H__
#define __HAR_H__

#include "Hotspot.h"
#include "MANode.h"
#include "Route.h"
#include "RoutingProtocol.h"


// TODO: rewrite
class HAR : 
	virtual public CRoutingProtocol
{
private:

	static vector<CHotspot *> m_hotspots;
	static vector<CRoute> m_routes;  //即hotspot class
	//vector<CMANode> m_MANodes;

	//用于计算所需MA个数的历史平均值
	static int SUM_MA_COST;
	static int COUNT_MA_COST;
	//用于计算MA的平均路点（热点）个数的历史平均值
	static double SUM_WAYPOINT_PER_MA;
	static int COUNT_WAYPOINT_PER_MA;
	/** 辅助函数 **/

	//用于hotspot classification
	static double getHotspotHeat(CHotspot *hotspot);
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

//	//在每一次贪婪选择之前调用，将从CHotspot::oldSelectedHotspots中寻找投递计数为0的热点删除放入CHotspot::deletedHotspots
//	//并删除其对应的所有position放入CPosition::deletedPositions
//	static void DecayPositionsWithoutDeliveryCount(int currentTime);


public:

	static double BETA;  //ratio for true hotspot
//	static double GAMMA;  //ratio for HotspotsAboveAverage
	static double CO_HOTSPOT_HEAT_A1;
	static double CO_HOTSPOT_HEAT_A2;

	/********************************* merge-HAR ***********************************/

	static int MIN_WAITING_TIME;  //add minimum waiting time to each hotspot
	static bool TEST_BALANCED_RATIO;
//	static bool TEST_LEARN;
//	static double MIN_POSITION_WEIGHT;

	HAR(){};
	~HAR(){};

	//根据时间和热点，计算等待时间
	static double calculateWaitingTime(int currentTime, CHotspot *hotspot);

	//从下层协议传入的控制/数据包
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// sink <- MA 
	static vector<CPacket*> receivePackets(CSink* sink, CMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- sink 
	static vector<CPacket*> receivePackets(CMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time);
	// Node <- MA 
	static vector<CPacket*> receivePackets(CNode* node, CMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- Node 
	static vector<CPacket*> receivePackets(CMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time);

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

	static bool Operate(int currentTime);

};

#endif // __HAR_H__
