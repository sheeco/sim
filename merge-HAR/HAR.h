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

class HAR
{
private:
	CSink* m_sink;
	vector<CNode> m_nodes;
	vector<CHotspot *> m_hotspots;
	vector<CRoute> m_routes;  //��hotspot class
	vector<CMANode> m_MANodes;

	/** �������� **/

	//����hotspot classification
	double getHotspotHeat(CHotspot *hotspot);
	double getWaitingTime(CHotspot *hotspot);
	double getSumGenerationRate(vector<int> nodes);  //����ge��sum��ͬһ��node��Ӧ�ظ�����
	double getSumGenerationRate(vector<int> nodes_a, vector<int> nodes_b);
	double getTimeIncrementForInsertion(CRoute route, int front, CHotspot *hotspot);
	double calculateRatioForInsertion(CRoute route, int front, CHotspot *hotspot);
	//��һ��route�����Ż���TSP ����ھ��㷨��
	void OptimizeRoute(CRoute &route);
	//�������ͳ������
	double getAverageEnergyConsumption()
	{
		if(CData::getDataArrivalCount() == 0)
			return 0;
		else
			return ( CNode::getEnergyConsumption() + CMANode::getEnergyConsumption() ) / CData::getDataArrivalCount();
	}
	double calculateEDTime();


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
	
	//���޶���Χ�������ɾһ��������node
	void ChangeNodeNumber();
	//��������node��λ�ã�������position��
	void UpdateNodeLocations();
	//ִ���ȵ�ѡȡ
	void HotspotSelection();
	//����ѡȡ����hotspot���ϵõ�hotspot class�ļ���
	void HotspotClassification();
	//�Գ����õ���hotspot class����·���Ż���TSP ����ھ��㷨��
	void MANodeRouteDesign();
	//���ض�ʱ���ϲ�������
	void GenerateData();
	//���ض�ʱ���Ϸ�������
	void SendData();
	//��ӡ�����Ϣ���ļ�
	void PrintInfo();

	//�Ƚϴ˴��ȵ�ѡȡ�Ľ������һ��ѡȡ���֮������ƶ�
	void CompareWithOldHotspots();

};

