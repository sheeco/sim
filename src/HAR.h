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

	//	static double energyConsumption;
	static vector<CHARMANode *> allMANodes;
	static vector<CHARMANode *> busyMANodes;
	static vector<CHARMANode *> freeMANodes;


	CHARMANode(CHARRoute route, int time)
	{
		this->setRoute(new CHARRoute(route));
	}
	//暂时闲置
	void turnFree()
	{
		for( auto iMANode = busyMANodes.begin(); iMANode != busyMANodes.end(); ++iMANode )
		{
			if( ( *iMANode ) == this )
			{
				busyMANodes.erase(iMANode);
				break;
			}
		}
		freeMANodes.push_back(this);
	}

	static vector<CHARMANode *>& getBusyMANodes()
	{
		return busyMANodes;
	}

	static vector<CHARMANode *>& getAllMANodes()
	{
		return allMANodes;
	}

	static inline double getSumEnergyConsumption()
	{
		double sumEnergyConsumption = 0;
		for( auto iMANode = busyMANodes.begin(); iMANode != busyMANodes.end(); ++iMANode )
			sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();
		for( auto iMANode = freeMANodes.begin(); iMANode != freeMANodes.end(); ++iMANode )
			sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();

		return sumEnergyConsumption;
	}
	inline CHARRoute* getHARRoute() const
	{
		return dynamic_cast< CHARRoute* >( this->route );
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

	//当前活动MA个数不足时调用，将激活闲置的MA或构造新的MA
	static CHARMANode* newMANode(CHARRoute route, int time)
	{
		CHARMANode *result = nullptr;

		//构造新的MA
		if( freeMANodes.empty() )
		{
			result = new CHARMANode(route, time);
		}
		//使用闲置的MA
		else
		{
			result = freeMANodes[0];
			freeMANodes.erase(freeMANodes.begin());
			result->updateRoute(route);
		}
		busyMANodes.push_back(result);
		allMANodes.push_back(result);
		return result;
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
	static double getTimeIncreForInsertion(int currentTime, CHARRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int currentTime, CHARRoute route, int front, CHotspot *hotspot);
	//对一条route进行优化（TSP 最近邻居算法）
	static void OptimizeRoute(CHARRoute &route);
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
	static inline CHARRoute popRoute()
	{
		CHARRoute result = m_newRoutes[0];
		m_newRoutes.erase(m_newRoutes.begin());
		return result;
	}

	//根据时间和热点，计算等待时间
	static double calculateWaitingTime(int currentTime, CHotspot *hotspot);

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

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

	static bool Init();
	static bool Operate(int currentTime);

};

#endif // __HAR_H__
