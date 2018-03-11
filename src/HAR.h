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
	int timeCreation;
	int timeExpiration;
	bool overdue;  //是否过期

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
	CHarRoute(int timeCreation, int timeExpiration): timeCreation(timeCreation), timeExpiration(timeExpiration)
	{
		this->init();
	}

	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}

	inline bool isOverdue(int now) const
	{
		return this->getTimeExpiration() <= now;
	}
	//将给定的元素放到waypoint列表的最后
	void AddHotspot(CBasicEntity *hotspot)
	{
		//TODO: rfct
		AddWaypoint(hotspot, getConfig<int>("har", "min_waiting_time"));
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>( hotspot )->getCoveredNodes());
	}
	//将给定hotspot插入到路径中给定的位置
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
	void updateStatus(int time) override;

};

class HAR :
	virtual public CRoutingProtocol
{
private:

	static vector<CHotspot *> hotspots;
	static vector<CHarRoute *> maRoutes;  //即hotspot class
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

	//取得新的路线集合
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
	static double getTimeIncreForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot);
	//对一条route进行优化（TSP 最近邻居算法）
	static void OptimizeRoute(CHarRoute *route);
	//计算相关统计数据
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

	//根据选取出的hotspot集合得到hotspot class的集合
	static void HotspotClassification(int now);
	//对初步得到的hotspot class进行路径优化（TSP 最近邻居算法）
	static void MANodeRouteDesign(int now);
//	//在特定时槽上发送数据
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
	//必须先调用hasRoutes判断
	static inline CHarRoute* popRoute()
	{
		CHarRoute* result = maRoutes[indexRoute];
		indexRoute = ( indexRoute + 1 ) % maRoutes.size();
		return new CHarRoute(*result);
	}

	static void atMAReturn(CHarMANode* ma, int now)
	{
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
	//根据时间和热点，计算等待时间
	static double calculateWaitingTime(int now, CHotspot *hotspot);

	//更新所有 MA 的坐标、等待时间
	//注意：必须在新一轮热点选取之后调用
	static void UpdateMANodeStatus(int now)
	{
		//为空闲的MA分配路线
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
	//从下层协议传入的控制/数据包
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

		vector<CHarMANode*> MAs = busyMAs;

		// xHAR: MAs => nodes
		for( vector<CHarMANode*>::iterator srcMA = MAs.begin(); srcMA != MAs.end(); ++srcMA )
		{
			// skip discover if buffer is full
			if( ( *srcMA )->getBufferVacancy() > 0 )
				transmitFrame(**srcMA, ( *srcMA )->sendRTSWithCapacity(now), now);
		}

		// xHAR: no forward between nodes

		if( ( now + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0 )
		{
			CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
			CPrintHelper::PrintNewLine();
			print = true;
		}
	}

	//打印相关信息到文件
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
		// 不允许 xHAR 使用 HDC 作为 MAC 协议
		else
			throw string("HAR::Operate(): Only SMac is allowed as MAC protocol for HAR.");

		if(! CNode::UpdateNodeStatus(now))
			return false;

		CHotspotSelect::RemovePositionsForDeadNodes(CNode::getIdNodes(CNode::getDeadNodes()), now);
		CHotspotSelect::CollectNewPositions(now, CNode::getAliveNodes());

		hotspots = CHotspotSelect::HotspotSelect(CNode::getIdNodes(CNode::getAliveNodes()), now);

		//检测节点所在热点区域
		CHotspot::UpdateAtHotspotForNodes(CNode::getAliveNodes(), hotspots, now);

		if(now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT
		   && now <= CHotspotSelect::STARTTIME_HOTSPOT_SELECT + getConfig<int>("simulation", "slot"))
			initMANodes(now);

		UpdateMANodeStatus(now);

		HotspotClassification(now);

		MANodeRouteDesign(now);

		// 不允许 xHAR 使用 HDC 作为 MAC 协议
		//if( config.MAC_PROTOCOL == config::_hdc )
		//	CHDC::Operate(now);
		//else 
		CommunicateBetweenNeighbors(now);

		PrintInfo(now);

		return true;
	}

};

#endif // __HAR_H__
