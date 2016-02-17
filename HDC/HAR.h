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
	static vector<CRoute> m_routes;  //��hotspot class
	//vector<CMANode> m_MANodes;

	//���ڼ�������ȡ�����ȵ���������ʷƽ��ֵ
	static int HOTSPOT_COST_SUM;
	static int HOTSPOT_COST_COUNT;
	//���ڼ�������MA��������ʷƽ��ֵ
	static int MA_COST_SUM;
	static int MA_COST_COUNT;
	//���ڼ���MA��ƽ��·�㣨�ȵ㣩��������ʷƽ��ֵ
	static double MA_WAYPOINT_SUM;
	static int MA_WAYPOINT_COUNT;
	//���ڼ�������ѡȡ�����ȵ㼯���У�merge��old�ȵ�ı�������ʷƽ��ֵ
	static double MERGE_PERCENT_SUM;
	static int MERGE_PERCENT_COUNT;
	static double OLD_PERCENT_SUM;
	static int OLD_PERCENT_COUNT;
	//���ڼ����ȵ�ǰ�����ƶȵ���ʷƽ��ֵ
	static double SIMILARITY_RATIO_SUM;
	static int SIMILARITY_RATIO_COUNT;

	/** �������� **/

	//����hotspot classification
	static double getHotspotHeat(CHotspot *hotspot);
	static double getWaitingTime(CHotspot *hotspot);
	static double getSumGenerationRate(vector<int> nodes);  //����ge��sum��ͬһ��node��Ӧ�ظ�����
	static double getSumGenerationRate(vector<int> nodes_a, vector<int> nodes_b);
	static double getTimeIncrementForInsertion(CRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(CRoute route, int front, CHotspot *hotspot);
	//��һ��route�����Ż���TSP ����ھ��㷨��
	static void OptimizeRoute(CRoute &route);
	//�������ͳ������
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


	//��ÿһ��̰��ѡ��֮ǰ���ã�����CHotspot::oldSelectedHotspots��Ѱ��Ͷ�ݼ���Ϊ0���ȵ�ɾ������CHotspot::deletedHotspots
	//��ɾ�����Ӧ������position����CPosition::deletedPositions
	static void DecayPositionsWithoutDeliveryCount(int currentTime);
	//����ѡȡ����hotspot���ϵõ�hotspot class�ļ���
	static void HotspotClassification(int currentTime);
	//�Գ����õ���hotspot class����·���Ż���TSP ����ھ��㷨��
	static void MANodeRouteDesign(int currentTime);
	//���ض�ʱ���Ϸ�������
	static void SendData(int currentTime);

	//�Ƚϴ˴��ȵ�ѡȡ�Ľ������һ��ѡȡ���֮������ƶ�
	static void CompareWithOldHotspots(int currentTime);


public:
	HAR(void){};

	~HAR(void){};


	//��������debug�����ͳ�ƺͼ�¼
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

	//ִ���ȵ�ѡȡ
	static void HotspotSelection(int currentTime);
	//��ӡ�����Ϣ���ļ�
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

