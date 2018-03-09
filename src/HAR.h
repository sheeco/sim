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


class CHARRoute : 
	virtual public CRoute
{
protected:
	friend class CHARMANode;
	friend class HAR;

	vector<int> coveredNodes;
	int timeCreation;
	int timeExpiration;
	bool overdue;  //�Ƿ����

	void init()
	{
		CRoute::init();
		timeCreation = INVALID;
		timeExpiration = INVALID;
		overdue = false;
		this->setStartPoint(CSink::getSink());
	}
	int getTimeCreation() const
	{
		return timeCreation;
	}
	int getTimeExpiration() const
	{
		return timeExpiration;
	}
	CHARRoute(int timeCreation, int timeExpiration): timeCreation(timeCreation), timeExpiration(timeExpiration)
	{
		this->init();
	}

	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}

	inline bool isOverdue() const
	{
		return overdue;
	}
	inline void setOverdue(bool overdue)
	{
		this->overdue = overdue;
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

class CHARNode : 
	virtual public CNode
{
private:
	void init()
	{
	}
protected:
	friend class HAR;
	CHARNode()
	{
		init();
	}

public:
	~CHARNode()
	{
	}

};

class CHARMANode : 
	virtual public CMANode
{
protected:
	friend class HAR;
	friend class CMacProtocol;

	CHARMANode(int now)
	{
		init();
		setTime(now);
	}

	inline CHARRoute* getHARRoute() const
	{
		CHARRoute* res = dynamic_cast< CHARRoute* >( this->route );
		return res;
	}
	inline void updateRoute(CHARRoute *route, int now)
	{
		CMANode::updateRoute(route);
		CPrintHelper::PrintContent(now, this->getName() + " is assigned with route " + route->format() + ".");
	}
	inline CHotspot* getAtHotspot() const
	{
		return dynamic_cast<CHotspot*>( getAtWaypoint() );
	}
	inline bool routeIsOverdue() const
	{
		return getHARRoute()->isOverdue();
	}
	inline void setRouteOverdue(bool overdue)
	{
		this->getHARRoute()->setOverdue(overdue);
	}

	void updateStatus(int time);

	vector<CData> bufferData(int time, vector<CData> datas)
	{
		vector<CData> ack = datas;
		RemoveFromList(datas, this->buffer);

		bool atPoint = isAtHotspot();
		if( atPoint )
		{
			this->getAtHotspot()->recordCountDelivery(datas.size());
			CData::deliverAtWaypoint(datas.size());
		}
		else
			CData::deliverOnRoute(datas.size());

		for( auto idata = datas.begin(); idata != datas.end(); ++idata )
		{
			////��Ϊ���� MA �ڵ㼴���� sink
			//idata->arriveSink(time);
			this->buffer.push_back(*idata);
		}

		return ack;
	}


};

class HAR :
	virtual public CRoutingProtocol
{
private:

	static vector<CHotspot *> hotspots;
	static vector<CHARRoute *> maRoutes;  //��hotspot class
	static vector<CHARRoute *> oldRoutes;
	static int indexRoute;
	//vector<CHARMANode> m_MANodes;
	//static vector<CHARRoute> m_newRoutes;

	static vector<CHARNode*> allNodes;
	static vector<CHARNode*> aliveNodes;
	static vector<CHARNode*> deadNodes;

	static vector<CHARMANode *> allMAs;
	static vector<CHARMANode *> busyMAs;
	static vector<CHARMANode *> freeMAs;

	static int INIT_NUM_MA;
	static int MAX_NUM_MA;


	//init based on newly loaded CNode
	static void initNodes(vector<CNode> nodes, int now)
	{
		for(CNode node : nodes)
		{
			CHARNode* harNode = new CHARNode();
			CNode* pBase = dynamic_cast< CNode* >( harNode );
			*pBase = node;

			allNodes.push_back(harNode);
		}
		aliveNodes = allNodes;
		deadNodes.clear();
		CNode::setNodes(CNode::upcast<CHARNode>(allNodes));
	}
	static bool hasNodes()
	{
		return !aliveNodes.empty();
	}

	static bool UpdateNodeStatus(int now)
	{
		//update basic status
		vector<CHARNode *> nodes = aliveNodes;
		for(CHARNode * node : nodes)
			node->updateStatus(now);

		CNode::ClearDeadNodes<CHARNode>(aliveNodes, deadNodes, now);

		return hasNodes();
	}
	static bool newMANode(int now)
	{
		if(allMAs.size() >= MAX_NUM_MA)
			return false;
		else
		{
			CHARMANode* ma = new CHARMANode(now);
			allMAs.push_back(ma);
			freeMAs.push_back(ma);
			CPrintHelper::PrintContent(ma->getName() + " is created. (" + STRING(allMAs.size()) + " in total)");
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
	static void initMANodes(int now)
	{
		INIT_NUM_MA = getConfig<int>("ma", "init_num_ma");
		MAX_NUM_MA = getConfig<int>("ma", "max_num_ma");
		newMANode(INIT_NUM_MA, now);

		freeMAs = allMAs;
	}
	static void turnFree(CHARMANode * ma)
	{
		AddToListUniquely(freeMAs, ma);
		RemoveFromList(busyMAs, ma);
	}
	static void turnBusy(CHARMANode * ma)
	{
		RemoveFromList(freeMAs, ma);
		AddToListUniquely(busyMAs, ma);
	}

	//ȡ���µ�·�߼���
	static inline void updateRoutes(vector<CHARRoute*> newRoutes)
	{
		oldRoutes.insert(oldRoutes.end(), maRoutes.begin(), maRoutes.end());
		maRoutes = newRoutes;
		indexRoute = 0;
	}
	static inline vector<CHARRoute*> getRoutes()
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
	static double getTimeIncreForInsertion(int now, CHARRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int now, CHARRoute route, int front, CHotspot *hotspot);
	//��һ��route�����Ż���TSP ����ھ��㷨��
	static void OptimizeRoute(CHARRoute *route);
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

	static vector<CHARNode*> getAllHARNodes()
	{
		return allNodes;
	}
	static vector<CHARMANode*> getAllMAs()
	{
		return allMAs;
	}

	static inline bool hasRoutes()
	{
		return ! maRoutes.empty();
	}
	//�����ȵ���hasRoutes�ж�
	static inline CHARRoute* popRoute()
	{
		CHARRoute* result = maRoutes[indexRoute];
		indexRoute = ( indexRoute + 1 ) % maRoutes.size();
		return new CHARRoute(*result);
	}

	static void atMAReturn(CHARMANode* ma, int now)
	{
		if( hasRoutes() )
			ma->updateRoute(HAR::popRoute(), now);
		else
		{
			ma->endRoute();
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
			CHARMANode *ma = freeMAs.front();
			if( !hasRoutes() )
				break;
			ma->updateRoute(popRoute(), now);
			turnBusy(ma);
		}

		auto MAs = busyMAs;  //local copy to avoid iterator problem after erasing
		for( vector<CHARMANode *>::iterator iMA = MAs.begin(); iMA != MAs.end(); ++iMA )
			( *iMA )->updateStatus(now);
	}

	static vector<CGeneralNode*> findNeighbors(CGeneralNode& src)
	{
		vector<CGeneralNode*> neighbors;

		/************************************************ Sensor Node *******************************************************/

		vector<CHARNode*> nodes = aliveNodes;
		for( vector<CHARNode*>::iterator idstNode = nodes.begin(); idstNode != nodes.end(); ++idstNode )
		{
			CNode* dstNode = *idstNode;
			//skip itself
			if( ( dstNode )->getID() == src.getID() )
				continue;

			if( CBasicEntity::withinRange(src, *dstNode, getConfig<int>("trans", "range_trans")) )
			{
				//ͳ��sink�ڵ����������
				if( typeid( src ) == typeid( CSink ) )
					CSink::encount();

				if( dstNode->isAwake() )
				{
					//ͳ��sink�ڵ����������
					if( typeid( src ) == typeid( CSink ) )
						CSink::encountActive();

					neighbors.push_back(dstNode);
				}
			}
		}

		/*************************************************** Sink **********************************************************/

		CSink* sink = CSink::getSink();
		if( CBasicEntity::withinRange(src, *sink, getConfig<int>("trans", "range_trans"))
		   && sink->getID() != src.getID() )
		{
			neighbors.push_back(sink);
			CSink::encount();
			CSink::encountActive();
		}


		/**************************************************** MA ***********************************************************/

		for( vector<CHARMANode*>::iterator iMA = busyMAs.begin(); iMA != busyMAs.end(); ++iMA )
		{
			//skip itself
			if( ( *iMA )->getID() == src.getID() )
				continue;

			if( CBasicEntity::withinRange(src, **iMA, getConfig<int>("trans", "range_trans"))
			   && ( *iMA )->isAwake() )
			{
				neighbors.push_back(*iMA);
			}
		}
		return neighbors;
		// TODO: sort by distance with src node ?
	}
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now);
	//���²�Э�鴫��Ŀ���/���ݰ�
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// sink <- MA 
	static vector<CPacket*> receivePackets(CSink* sink, CHARMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- sink 
	static vector<CPacket*> receivePackets(CHARMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time);
	// Node <- MA 
	static vector<CPacket*> receivePackets(CNode* node, CHARMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- Node 
	static vector<CPacket*> receivePackets(CHARMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time);

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

		vector<CHARMANode*> MAs = busyMAs;

		// xHAR: MAs => nodes
		for( vector<CHARMANode*>::iterator srcMA = MAs.begin(); srcMA != MAs.end(); ++srcMA )
		{
			// skip discover if buffer is full && _selfish is used
			if( ( *srcMA )->getCapacityForward() > 0 )
				transmitFrame(**srcMA, ( *srcMA )->sendRTSWithCapacity(now), now);
		}

		vector<CHARNode*> nodes = aliveNodes;
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
		initNodes(CNode::loadNodesFromFile(), now);
		CHotspotSelect::Init();
		return true;
	}
	static bool Operate(int now)
	{
		// ������ xHAR ʹ�� HDC ��Ϊ MAC Э��
		//if( config.MAC_PROTOCOL == config::_hdc )
		//	hasNodes = CHDC::Prepare(now);
		//else 
		if(getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac)
			CSMac::Prepare(now);
		else
			throw string("HAR::Operate(): Only SMac is allowed as MAC protocol for HAR.");

		vector<CNode*> aliveList = CNode::upcast<CHARNode>(aliveNodes);

		if(! UpdateNodeStatus(now))
			return false;

		CHotspotSelect::RemovePositionsForDeadNodes(CNode::getIdNodes(CNode::upcast<CHARNode>(deadNodes)), now);
		CHotspotSelect::CollectNewPositions(now, aliveList);

		hotspots = CHotspotSelect::HotspotSelect(CNode::getIdNodes(aliveList), now);

		//���ڵ������ȵ�����
		CHotspot::UpdateAtHotspotForNodes(aliveList, hotspots, now);

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
		CommunicateBetweenNeighbors(now);

		PrintInfo(now);

		return true;
	}

};

#endif // __HAR_H__
