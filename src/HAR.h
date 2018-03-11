#pragma once

#ifndef __HAR_H__
#define __HAR_H__

#include "Hotspot.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "MANode.h"
#include "Route.h"
#include "RoutingProtocol.h"
#include "SMac.h"


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
	void AddHotspot(CBasicEntity *hotspot)
	{
		//TODO: rfct
		AddWaypoint(hotspot, getConfig<int>("har", "min_waiting_time"));
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>( hotspot )->getCoveredNodes());
	}
	//������hotspot���뵽·���и�����λ��
	void AddHotspot(int front, CBasicEntity *hotspot)
	{
		AddWaypoint(front, hotspot, getConfig<int>("har", "min_waiting_time"));
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>( hotspot )->getCoveredNodes());
	}


};

class CHarMANode : 
	virtual public CMANode
{
protected:
	friend class HAR;
	friend class CMacProtocol;

	CHarMANode(int now)
	{
		init();
		setTime(now);
	}

	inline CHarRoute* getHARRoute() const
	{
		CHarRoute* res = dynamic_cast< CHarRoute* >( this->route );
		return res;
	}
	inline void updateRoute(CHarRoute *route, int now)
	{
		CMANode::updateRoute(route);
		CPrintHelper::PrintBrief(now, this->getName() + " is assigned with route " + route->format() + ".");
	}
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


	static bool newMANode(int now)
	{
		if(allMAs.size() >= CMANode:: MAX_NUM_MA)
			return false;
		else
		{
			CHarMANode* ma = new CHarMANode(now);
			allMAs.push_back(ma);
			freeMAs.push_back(ma);
			CPrintHelper::PrintBrief(ma->getName() + " is created. (" + STRING(allMAs.size()) + " in total)");
			return true;
		}
	}
	static bool newMANode(int n, int now)
	{
		for(int i = 0; i < n; ++i)
		{
			if(!newMANode(now))
				return false;
		}
		return true;
	}
	static void initNodeInfo()
	{
		for( CNode* pnode : CNode::getAllNodes() )
			mapDataCountRates[pnode->getID()] = pnode->getDataCountRate();
	}
	static void initMANodes(int now)
	{
		newMANode(CMANode::INIT_NUM_MA, now);

		freeMAs = allMAs;
	}
	static void turnFree(CHarMANode * ma)
	{
		ma->endRoute();
		ma->setBusy(false);
		AddToListUniquely(freeMAs, ma);
		RemoveFromList(busyMAs, ma);
	}
	static void turnBusy(CHarMANode * ma)
	{
		ma->setBusy(true);
		RemoveFromList(freeMAs, ma);
		AddToListUniquely(busyMAs, ma);
	}

	//ȡ���µ�·�߼���
	static inline void updateRoutes(vector<CHarRoute*> newRoutes)
	{
		oldRoutes.insert(oldRoutes.end(), maRoutes.begin(), maRoutes.end());
		maRoutes = newRoutes;
		indexRoute = 0;
	}
	static inline vector<CHarRoute*> getRoutes()
	{
		return maRoutes;
	}

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


public:

	HAR(){};
	~HAR(){};

	static vector<CHarMANode*> getAllMAs()
	{
		return allMAs;
	}

	static inline bool hasRoutes()
	{
		return ! maRoutes.empty();
	}
	//�����ȵ���hasRoutes�ж�
	static inline CHarRoute* popRoute()
	{
		CHarRoute* result = maRoutes[indexRoute];
		indexRoute = ( indexRoute + 1 ) % maRoutes.size();
		return new CHarRoute(*result);
	}

	static void atMAReturn(CHarMANode* ma, int now)
	{
		if( !CBasicEntity::withinRange(*ma, *CSink::getSink(), getConfig<int>("trans", "range_trans")) )
			throw string("CHarMANode::atMAReturn(): " + ma->getName() + " is not around Sink.");

		if( hasRoutes() )
			ma->updateRoute(HAR::popRoute(), now);
		else
		{
			turnFree(ma);
		}
	}

	static inline double getSumEnergyConsumption()
	{
		double sumEnergyConsumption = 0;
		for( auto iMANode = busyMAs.begin(); iMANode != busyMAs.end(); ++iMANode )
			sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();
		for( auto iMANode = freeMAs.begin(); iMANode != freeMAs.end(); ++iMANode )
			sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();

		return sumEnergyConsumption;
	}
	//����ʱ����ȵ㣬����ȴ�ʱ��
	static double calculateWaitingTime(int now, CHotspot *hotspot);

	//�������� MA �����ꡢ�ȴ�ʱ��
	//ע�⣺��������һ���ȵ�ѡȡ֮�����
	static void UpdateMANodeStatus(int now)
	{
		//Ϊ���е�MA����·��
		while( !freeMAs.empty() )
		{
			CHarMANode *ma = freeMAs.front();
			if( !hasRoutes() )
				break;
			ma->updateRoute(popRoute(), now);
			turnBusy(ma);
		}

		for( CHarMANode *pMA: allMAs )
			pMA->updateStatus(now);
	}

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

	static void CommunicateBetweenNeighbors(int now)
	{
		static bool print = false;
		if( now == 0
		   || print )
		{
			CPrintHelper::PrintHeading(now, "DATA DELIVERY");
			print = false;
		}

		// TODO: sink receive RTS / send by slot ?
		// xHAR: sink => MAs
		CSink* sink = CSink::getSink();
		transmitFrame(*sink, sink->sendRTS(now), now);

		vector<CHarMANode*> MAs = allMAs;
		// xHAR: MAs => nodes
		for( CHarMANode* pMA : busyMAs )
		{
			// skip discover if buffer is full
			if( pMA->isBusy()
			   && pMA->getBufferVacancy() > 0 )
				transmitFrame(*pMA, pMA->sendRTSWithCapacity(now), now);
		}

		// xHAR: no forward between nodes

		if( ( now + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0 )
		{
			CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
			CPrintHelper::PrintNewLine();
			print = true;
		}
	}

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int now);
	static void PrintFinal(int now);

	static bool Init(int now)
	{
		CMANode::Init();
		CHotspotSelect::Init();

		initNodeInfo();
		return true;
	}
	static bool Operate(int now)
	{
		if(getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac)
			CSMac::Prepare(now);
		// ������ xHAR ʹ�� HDC ��Ϊ MAC Э��
		else
			throw string("HAR::Operate(): Only SMac is allowed as MAC protocol for HAR.");

		if(! CNode::UpdateNodeStatus(now))
			return false;

		CHotspotSelect::RemovePositionsForDeadNodes(CNode::getIdNodes(CNode::getDeadNodes()), now);
		CHotspotSelect::CollectNewPositions(now, CNode::getAliveNodes());

		hotspots = CHotspotSelect::HotspotSelect(CNode::getIdNodes(CNode::getAliveNodes()), now);

		//���ڵ������ȵ�����
		CHotspot::UpdateAtHotspotForNodes(CNode::getAliveNodes(), hotspots, now);

		if(now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT
		   && now <= CHotspotSelect::STARTTIME_HOTSPOT_SELECT + getConfig<int>("simulation", "slot"))
			initMANodes(now);

		UpdateMANodeStatus(now);

		HotspotClassification(now);

		MANodeRouteDesign(now);

		// ������ xHAR ʹ�� HDC ��Ϊ MAC Э��
		//if( config.MAC_PROTOCOL == config::_hdc )
		//	CHDC::Operate(now);
		//else 
		if(now < getConfig<int>("simulation", "runtime") )
			CommunicateBetweenNeighbors(now);

		PrintInfo(now);

		return true;
	}

};

#endif // __HAR_H__
