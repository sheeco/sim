#pragma once

#ifndef __HAR_H__
#define __HAR_H__

#include "Hotspot.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "MANode.h"
#include "Route.h"
#include "RoutingProtocol.h"


class CHARRoute : 
	virtual public CRoute
{
protected:
	friend class CHARMANode;
	friend class HAR;

	vector<int> coveredNodes;
	bool overdue;  //是否过期

	void init()
	{
		overdue = false;
	}
	CHARRoute()
	{
		this->init();
		this->setStartPoint(CSink::getSink());
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

class CHARNode : 
	virtual public CNode
{
	//UNDONE:
};

class CHARMANode : 
	virtual public CMANode
{
protected:
	friend class HAR;
	friend class CMacProtocol;

	CHARMANode(CHARRoute *route, int now)
	{
		this->updateRoute(route, now);
	}

	inline CHARRoute* getHARRoute() const
	{
		CHARRoute* res = dynamic_cast< CHARRoute* >( this->route );
		return res;
	}
	inline void updateRoute(CHARRoute *route, int now)
	{
		CMANode::updateRoute(route);
		CPrintHelper::PrintDetail(now, this->getName() + " is assigned with route " + route->format() + ".");
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

		bool atPoint = isAtWaypoint();
		if( atPoint )
		{
			this->getAtHotspot()->recordCountDelivery(datas.size());
			CData::deliverAtWaypoint(datas.size());
		}
		else
			CData::deliverOnRoute(datas.size());

		for( auto idata = datas.begin(); idata != datas.end(); ++idata )
		{
			////认为到达 MA 节点即到达 sink
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

	static vector<CHotspot *> m_hotspots;
	static vector<CHARRoute> m_routes;  //即hotspot class
	//vector<CHARMANode> m_MANodes;
	static vector<CHARRoute> m_newRoutes;

	//TODO:
	//static vector<CHARNode*> allNodes;
	//static vector<CHARNode*> aliveNodes;
	//static vector<CHARNode*> deadNodes;

	static vector<CHARMANode *> allMAs;
	static vector<CHARMANode *> busyMAs;
	static vector<CHARMANode *> freeMAs;

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
	//当前活动MA个数不足时调用，将激活闲置的MA或构造新的MA
	static CHARMANode* newMANode(CHARRoute *route, int now)
	{
		CHARMANode *result = nullptr;

		//构造新的MA
		if( freeMAs.empty() )
		{
			result = new CHARMANode(route, now);
			AddToListUniquely(allMAs, result);
		}
		//使用闲置的MA
		else
		{
			result = freeMAs[0];
			result->updateRoute(route, now);
		}
		turnBusy(result);
		return result;
	}
	//取得新的路线集合
	static inline void setNewRoutes(vector<CHARRoute> newRoutes)
	{
		m_newRoutes = newRoutes;
	}
	static inline vector<CHARRoute> getNewRoutes()
	{
		return m_newRoutes;
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
	static double getTimeIncreForInsertion(int now, CHARRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int now, CHARRoute route, int front, CHotspot *hotspot);
	//对一条route进行优化（TSP 最近邻居算法）
	static void OptimizeRoute(CHARRoute &route);
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

//	//在每一次贪婪选择之前调用，将从CHotspot::oldSelectedHotspots中寻找投递计数为0的热点删除放入CHotspot::deletedHotspots
//	//并删除其对应的所有position放入CPosition::deletedPositions
//	static void DecayPositionsWithoutDeliveryCount(int now);


public:

	HAR(){};
	~HAR(){};

	//判断是否还有未分配出去的新路线
	static inline bool hasMoreNewRoutes()
	{
		if( m_newRoutes.empty() )
			return false;
		else
			return true;
	}
	//必须先调用hasMoreNewRoutes判断
	static inline CHARRoute* popRoute()
	{
		CHARRoute result = m_newRoutes[0];
		m_newRoutes.erase(m_newRoutes.begin());
		return new CHARRoute(result);
	}

	static void atMAReturn(CHARMANode* ma, int now)
	{
		if( hasMoreNewRoutes() )
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
	//根据时间和热点，计算等待时间
	static double calculateWaitingTime(int now, CHotspot *hotspot);

	//更新所有 MA 的坐标、等待时间
	//注意：必须在新一轮热点选取之后调用
	static void UpdateMANodeStatus(int now)
	{
			//将新增的路线分配给新的MA
		while( !freeMAs.empty() )
		{
			CHARMANode *ma = freeMAs.front();
			if( !hasMoreNewRoutes() )
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

		vector<CNode*> nodes = CNode::getNodes();
		for( vector<CNode*>::iterator idstNode = nodes.begin(); idstNode != nodes.end(); ++idstNode )
		{
			CNode* dstNode = *idstNode;
			//skip itself
			if( ( dstNode )->getID() == src.getID() )
				continue;

			if( CBasicEntity::withinRange(src, *dstNode, getConfig<int>("trans", "range_trans")) )
			{
				//统计sink节点的相遇计数
				if( typeid( src ) == typeid( CSink ) )
					CSink::encount();

				if( dstNode->isAwake() )
				{
					//统计sink节点的相遇计数
					if( typeid( src ) == typeid( CSink ) )
						CSink::encountActive();

					if( Bet(getConfig<double>("trans", "prob_trans")) )
						neighbors.push_back(dstNode);
				}
			}
		}

		/*************************************************** Sink **********************************************************/

		CSink* sink = CSink::getSink();
		if( CBasicEntity::withinRange(src, *sink, getConfig<int>("trans", "range_trans"))
		   && Bet(getConfig<double>("trans", "prob_trans"))
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
			   && Bet(getConfig<double>("trans", "prob_trans"))
			   && ( *iMA )->isAwake() )
			{
				neighbors.push_back(*iMA);
			}
		}
		return neighbors;
		// TODO: sort by distance with src node ?
	}
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now);
	//从下层协议传入的控制/数据包
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// sink <- MA 
	static vector<CPacket*> receivePackets(CSink* sink, CHARMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- sink 
	static vector<CPacket*> receivePackets(CHARMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time);
	// Node <- MA 
	static vector<CPacket*> receivePackets(CNode* node, CHARMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- Node 
	static vector<CPacket*> receivePackets(CHARMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time);

	static void CommunicateWithNeighbor(int now)
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

		vector<CNode*> nodes = CNode::getNodes();
		vector<CHARMANode*> MAs = busyMAs;

		// xHAR: MAs => nodes
		for( vector<CHARMANode*>::iterator srcMA = MAs.begin(); srcMA != MAs.end(); ++srcMA )
		{
			// skip discover if buffer is full && _selfish is used
			if( ( *srcMA )->getCapacityForward() > 0 )
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

	static bool Init();
	static bool Operate(int now);

};

#endif // __HAR_H__
