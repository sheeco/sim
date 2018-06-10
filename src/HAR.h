#pragma once

#ifndef __HAR_H__
#define __HAR_H__

#include "RoutingProtocol.h"
#include "Hotspot.h"
#include "MANode.h"
#include "Route.h"
#include "SMac.h"


//����xHARЭ���MA·����
class CHarRoute : 
	virtual public CRoute
{
protected:
	friend class CHarMANode;
	friend class HAR;

	vector<int> coveredNodes;
	int timeCreate;
	int timeExpire;
	bool overdue;  //�Ƿ����

	void init()
	{
		CRoute::init();
		timeCreate = INVALID;
		timeExpire = INVALID;
		overdue = false;
		this->setStartPoint(CSink::getSink());
	}
	int getTimeCreation() const
	{
		return timeCreate;
	}
	int getTimeExpiration() const
	{
		return timeExpire;
	}
	CHarRoute(int timeCreate, int timeExpire)
	{
		this->init();
		this->timeCreate = timeCreate;
		this->timeExpire = timeExpire;
		this->setStartPoint(CSink::getSink());
	}

	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}

	inline bool isOverdue(int now) const
	{
		return this->getTimeExpiration() <= now;
	}
	//��������Ԫ�طŵ�waypoint�б�����
	void AddHotspot(CBasicEntity *hotspot);
	//������hotspot���뵽·���и�����λ��
	void AddHotspot(int front, CBasicEntity *hotspot);


};

//װ��xHARЭ���MA�ڵ���
class CHarMANode : 
	virtual public CMANode
{
protected:
	friend class HAR;
	friend class CMacProtocol;

	CHarMANode(int now)
	{
		setTime(now);
	}

	inline CHarRoute* getHARRoute() const
	{
		CHarRoute* res = dynamic_cast< CHarRoute* >( this->route );
		return res;
	}
	inline void updateRoute(CHarRoute *route, int now);
	inline CHotspot* getAtHotspot() const
	{
		return dynamic_cast<CHotspot*>( getAtWaypoint() );
	}
	inline bool routeHasExpired(int now) const
	{
		if( !this->hasRoute() )
			throw string("CHarMANode::routeHasExpired(): " + this->getName() + " has no route.");
		return getHARRoute()->isOverdue(now);
	}
	vector<CData> bufferData(int time, vector<CData> datas)
	{
		vector<CData> ack = CMANode::bufferData(time, datas);

		if( isAtWaypoint() )
			this->getAtHotspot()->recordCountDelivery(ack.size());

		return ack;
	}
	void updateStatus(int time);

};

//xHAR·��
//ͨ��ָ�����ò���simulation.hotspot_select��ѡ������HAR(_original)/iHAR(_ihs)/mergeHAR(_mhs)
class HAR :
	virtual public CRoutingProtocol
{
private:

	static vector<CHotspot *> hotspots;
	static vector<CHarRoute *> maRoutes;  //��hotspot class
	static vector<CHarRoute *> oldRoutes;
	static int indexRoute;
	//vector<CHarMANode> m_MANodes;
	//static vector<CHarRoute> m_newRoutes;

	static map<int, double> mapDataCountRates;

	static vector<CHarMANode *> allMAs;
	static vector<CHarMANode *> busyMAs;
	static vector<CHarMANode *> freeMAs;


	static bool newMANode(int now);
	static bool newMANode(int n, int now);
	static void initNodeInfo();
	static void initMANodes(int now);
	static void turnFree(CHarMANode * ma);
	static void turnBusy(CHarMANode * ma);

	//ȡ���µ�·�߼���
	static inline void updateRoutes(vector<CHarRoute*> newRoutes);
	static inline vector<CHarRoute*> getRoutes();

	//���ڼ�������MA��������ʷƽ��ֵ
	static int SUM_MA_COST;
	static int COUNT_MA_COST;
	//���ڼ���MA��ƽ��·�㣨�ȵ㣩��������ʷƽ��ֵ
	static double SUM_WAYPOINT_PER_MA;
	static int COUNT_WAYPOINT_PER_MA;
	/** �������� **/

	//����hotspot classification
	static double getHotspotHeat(CHotspot *hotspot);
	static double getSumDataRate(vector<int> nodes);  //����ge��sum��ͬһ��node��Ӧ�ظ�����
	static double getTimeIncreForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot);
	//��һ��route�����Ż���TSP ����ھ��㷨��
	static void OptimizeRoute(CHarRoute *route);
	//�������ͳ������
	static double calculateEDTime(int now);

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
	static void HotspotClassification(int now);
	//�Գ����õ���hotspot class����·���Ż���TSP ����ھ��㷨��
	static void MANodeRouteDesign(int now);
//	//���ض�ʱ���Ϸ�������
//	static void SendData(int now);


	HAR(){};

	static inline bool hasRoutes()
	{
		return ! maRoutes.empty();
	}
	//�����ȵ���hasRoutes�ж�
	static inline CHarRoute* popRoute();

	static void atMAReturn(CHarMANode* ma, int now);

	static inline double getSumEnergyConsumption();

	//�������� MA �����ꡢ�ȴ�ʱ��
	//ע�⣺��������һ���ȵ�ѡȡ֮�����
	static void UpdateMANodeStatus(int now);

	static vector<CGeneralNode*> findNeighbors(CGeneralNode& src);
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now);
	//���²�Э�鴫��Ŀ���/���ݰ�
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// sink <- MA 
	static vector<CPacket*> receivePackets(CSink* sink, CHarMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- sink 
	static vector<CPacket*> receivePackets(CHarMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time);
	// Node <- MA 
	static vector<CPacket*> receivePackets(CNode* node, CHarMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- Node 
	static vector<CPacket*> receivePackets(CHarMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time);

	static void CommunicateBetweenNeighbors(int now);


public:

	~HAR(){};

	//����ʱ����ȵ㣬����ȴ�ʱ��
	static double calculateWaitingTime(int now, CHotspot *hotspot);
	static vector<CHarMANode*> getAllMAs()
	{
		return allMAs;
	}

	static bool Init(int now);
	static bool Operate(int now);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int now);
	static void PrintFinal(int now);

};

#endif // __HAR_H__
