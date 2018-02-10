#pragma once

#ifndef __MA_NODE_H__
#define __MA_NODE_H__

#include "Data.h"
#include "Route.h"
#include "Sink.h"
#include "GeneralNode.h"


class CMANode :
	virtual public CGeneralNode
{
//protected:

//	int ID;  //MA节点的编号
//	CCoordinate location;  //MA节点现在的y坐标
//	int time;  //更新MA节点坐标的时间戳
//	bool flag;


private:

	CRoute route;
	vector<CRoute> oldRoutes;
	CHotspot *atHotspot;  //MA到达的hotspot
	int waitingWindow;  //当前规定的waiting时间窗大小
	int waitingState;  //当前处于waiting时间窗的位置（值为0说明还未开始等待，值等于最大值说明等待时间结束）

//	static double energyConsumption;
	static vector<CMANode *> MANodes;
	static vector<CMANode *> freeMANodes;

//	static int encounterActive;  //有效相遇
	static int encounter;
	static int COUNT_ID;


	void init()
	{
		if( COUNT_ID == 0 )
			COUNT_ID = configs.ma.START_COUNT_ID;

		setLocation( CSink::getSink()->getLocation() );  //初始化 MA 位置在 sink 处
		atHotspot = nullptr;	
		waitingWindow = 0;
		waitingState = 0;
		capacityBuffer = configs.ma.CAPACITY_BUFFER;
	}

	CMANode()
	{
		init();
	}

	//自动生成ID，需手动调用
	inline void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}

	CMANode(CRoute route, int time)
	{
		init();
		this->route = route;
		this->setLocation( configs.sink.X, configs.sink.Y);
		atHotspot = nullptr;
		this->time = time;
		generateID();
	}
	~CMANode(){};


public:

	static vector<CMANode *>& getMANodes()
	{
		return MANodes;
	}


	//当前活动MA个数不足时调用，将激活闲置的MA或构造新的MA
	static CMANode* newMANode(CRoute route, int time)
	{
		CMANode *result = nullptr;

		//构造新的MA
		if( freeMANodes.empty() )
		{
			result = new CMANode(route, time);
		}
		//使用闲置的MA
		else
		{
			result = freeMANodes[0];
			freeMANodes.erase( freeMANodes.begin() );
			result->updateRoute(route);
		}
		MANodes.push_back(result);
		return result;
	}

	static int getCapacityBuffer()
	{
		return configs.ma.CAPACITY_BUFFER;
	}

	static int getSpeed()
	{
		return configs.ma.SPEED;
	}

	static inline double getSumEnergyConsumption()
	{
		double sumEnergyConsumption = 0;
		for(auto iMANode = MANodes.begin(); iMANode != MANodes.end(); ++iMANode)
			sumEnergyConsumption += (*iMANode)->getEnergyConsumption();
		for(auto iMANode = freeMANodes.begin(); iMANode != freeMANodes.end(); ++iMANode)
			sumEnergyConsumption += (*iMANode)->getEnergyConsumption();

		return sumEnergyConsumption;
	}

	//相遇计数：统计 MA 和节点的相遇
	static void encount() 
	{
		++encounter;
	}
//	static void encountActive() 
//	{
//		++encounterActive;
//	}

	static int getEncounter() 
	{
		return encounter;
	}
//	static int getEncounterActive()
//	{
//		return encounterActive;
//	}
//	static double getPercentEncounterActive() 
//	{
//		if(encounterActive == 0)
//			return 0.0;
//		return double(encounterActive) / double(encounter);
//	}

	string toString() const
	{
		return "MA " + NDigitIntString(this->ID, 3);
	}

	//暂时闲置
	void turnFree()
	{
		for(auto iMANode = MANodes.begin(); iMANode != MANodes.end(); ++iMANode)
		{
			if( (*iMANode) == this )
			{
				MANodes.erase(iMANode);
				break;
			}
		}
		freeMANodes.push_back(this);
	}

	inline CRoute* getRoute()
	{
		return &route;
	}
	inline void updateRoute(CRoute route)
	{
		if( this->route.getToPoint() != nullptr )
			oldRoutes.push_back(this->route);

		this->route = route;
	}
	inline bool routeIsOverdue() const
	{
		return route.isOverdue();
	}
	inline void setRouteOverdue(bool overdue)
	{
		this->route.setOverdue(overdue);
	}
	//inline void setAtSink(bool atSink)
	//{
	//	this->atSink = atSink;
	//}
	inline void setAtHotspot(CHotspot *atHotspot)
	{
		this->atHotspot = atHotspot;
	}

	//TODO: include when im still in range of the hotspot after leaving the center, maybe introducing m_lastAtHotspot
	inline bool isAtHotspot() const
	{
		if(atHotspot == nullptr)
			return false;
		else
			return true;
	}
	inline CHotspot* getAtHotspot() const
	{
		return atHotspot;
	}
//	inline void setWaitingTime(int waitingTime)
//	{
//		this->waitingWindow = waitingTime;
//	}
//	inline int getWaitingTime() const
//	{
//		return waitingWindow;
//	}

	//判断Buffer是否已满
	inline bool isFull() const
	{
		if(buffer.size() >= configs.ma.CAPACITY_BUFFER)
			return true;
		else
			return false;
	}	

	//接收数据时，返回允许接收的最大数据数
	inline int getCapacityForward() const
	{
		int capacity = capacityBuffer - buffer.size();
		if( capacity < 0 )
			capacity = 0;

		if( configs.ma.SCHEME_RELAY == config::_selfish )
			return capacity;
		else if( configs.ma.SCHEME_RELAY == config::_loose )
			return capacityBuffer;
		else
			return 0;
	}

	//MA node: all on
	bool isAwake() const
	{
		return true;
	}

	//MA移动，更新等待时间、位置、时间戳、等待时间
	//如果路线过期或缓存已满，立即返回sink
	void updateStatus(int time);

	CFrame* sendRTSWithCapacity(int currentTime);

	vector<CData> bufferData(int time, vector<CData> datas);

	void checkDataByAck(vector<CData> ack);

};

#endif // __MA_NODE_H__
