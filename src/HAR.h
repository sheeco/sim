#pragma once

#include "Hotspot.h"
#include "MANode.h"
#include "Route.h"
#include "RoutingProtocol.h"


// TODO: rewrite
class HAR : 
	public CRoutingProtocol
{
private:

	static vector<CHotspot *> m_hotspots;
	static vector<CRoute> m_routes;  //��hotspot class
	//vector<CMANode> m_MANodes;

	//���ڼ�������MA��������ʷƽ��ֵ
	static int SUM_MA_COST;
	static int COUNT_MA_COST;
	//���ڼ���MA��ƽ��·�㣨�ȵ㣩��������ʷƽ��ֵ
	static double SUM_WAYPOINT_PER_MA;
	static int COUNT_WAYPOINT_PER_MA;
	/** �������� **/

	//����hotspot classification
	static double getHotspotHeat(CHotspot *hotspot);
	static double getWaitingTime(int currentTime, CHotspot *hotspot);
	static double getSumDataRate(vector<int> nodes);  //����ge��sum��ͬһ��node��Ӧ�ظ�����
	static double getTimeIncreForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot);
	//��һ��route�����Ż���TSP ����ھ��㷨��
	static void OptimizeRoute(CRoute &route);
	//�������ͳ������
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


	//����ѡȡ����hotspot���ϵõ�hotspot class�ļ���
	static void HotspotClassification(int currentTime);
	//�Գ����õ���hotspot class����·���Ż���TSP ����ھ��㷨��
	static void MANodeRouteDesign(int currentTime);
//	//���ض�ʱ���Ϸ�������
//	static void SendData(int currentTime);

//	//��ÿһ��̰��ѡ��֮ǰ���ã�����CHotspot::oldSelectedHotspots��Ѱ��Ͷ�ݼ���Ϊ0���ȵ�ɾ������CHotspot::deletedHotspots
//	//��ɾ�����Ӧ������position����CPosition::deletedPositions
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

	//���²�Э�鴫��Ŀ���/���ݰ�
	// sink <- MA 
	static vector<CGeneralData*> receiveContents(CSink* sink, CMANode* fromMA, vector<CGeneralData*> contents, int time);
	// MA <- sink 
	static vector<CGeneralData*> receiveContents(CMANode* MA, CSink* fromSink, vector<CGeneralData*> contents, int time);
	// Node <- MA 
	static vector<CGeneralData*> receiveContents(CNode* node, CMANode* fromMA, vector<CGeneralData*> contents, int time);
	// MA <- Node 
	static vector<CGeneralData*> receiveContents(CMANode* MA, CNode* fromNode, vector<CGeneralData*> contents, int time);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

	static bool Operate(int currentTime);

};

