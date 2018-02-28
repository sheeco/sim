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


protected:

	CRoute *route;
	vector<CRoute*> routeHistory;
	CBasicEntity *atPoint;
	int waitingWindow;  //当前规定的waiting时间窗大小
	int waitingState;  //当前处于waiting时间窗的位置（值为0说明还未开始等待，值等于最大值说明等待时间结束）
	bool returnAtOnce;

//	static int encounterActive;  //有效相遇
	static int encounter;
	static int COUNT_ID;


	void init()
	{
		if( COUNT_ID == 0 )
			COUNT_ID = getConfig<int>("ma", "base_id");

		setLocation( CSink::getSink()->getLocation() );  //初始化 MA 位置在 sink 处
		atPoint = nullptr;	
		waitingWindow = 0;
		waitingState = 0;
		capacityBuffer = getConfig<int>("ma", "buffer");
		returnAtOnce = false;
		route = nullptr;
		atPoint = nullptr;
		time = time;
		generateID();
		setName("MA #" + STRING(this->getID()));
	}

	//自动生成ID，需手动调用
	inline void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}

	inline bool ifReturnAtOnce()
	{
		return this->returnAtOnce;
	}
	inline void setReturnAtOnce(bool returnAtOnce)
	{
		this->returnAtOnce = returnAtOnce;
		this->waitingWindow = this->waitingState = 0;
	}

protected:
	
	void setRoute(CRoute* route)
	{
		this->route = route;
	}
	CMANode()
	{
		init();
	}
	CMANode(CRoute* route, int time)
	{
		init();
		FreePointer(this->route);
		this->setRoute(route);
		this->setTime(time);
		generateID();
		this->setName("MA #" + STRING(this->getID()));
	}
	virtual ~CMANode() = 0
	{
		FreePointer(this->route);
		FreePointerVector(this->routeHistory);
	};


public:


	static int getCapacityBuffer()
	{
		return getConfig<int>("ma", "buffer");
	}

	static int getSpeed()
	{
		return getConfig<int>("ma", "speed");
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

	inline CRoute* getRoute()
	{
		return route;
	}
	inline void endRoute()
	{
		this->routeHistory.push_back(this->route);
		this->route = nullptr;
		atPoint = nullptr;
		waitingWindow = 0;
		waitingState = 0;
		this->setReturnAtOnce(false);
	}
	inline void updateRoute(CRoute *route)
	{
		this->endRoute();
		this->setRoute(route);
	}
	//inline void setAtSink(bool atSink)
	//{
	//	this->atSink = atSink;
	//}
	inline void setAtWaypoint(CBasicEntity *wayPoint)
	{
		this->atPoint = wayPoint;
	}

	//TODO: include when im still in range of the hotspot after leaving the center, maybe introducing m_lastAtHotspot
	inline bool isAtWaypoint() const
	{
		if(atPoint == nullptr)
			return false;
		else
			return true;
	}
	inline CBasicEntity* getAtWaypoint() const
	{
		return atPoint;
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
		if(buffer.size() >= getConfig<int>("ma", "buffer"))
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

		if( getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_selfish )
			return capacity;
		else if( getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_loose )
			return capacityBuffer;
		else
			return 0;
	}

	//MA node: all on
	bool isAwake() const
	{
		return true;
	}

	//NOTE: 虚函数定义导致VS调试器无法正确显示this指针
	//MA移动，更新等待时间、位置、时间戳、等待时间
	//如果路线过期或缓存已满，立即返回sink
	//virtual void updateStatus(int time) = 0;

	//TODO: ?
	CFrame* sendRTSWithCapacity(int now)
	{
		vector<CPacket*> packets;
		packets.push_back(new CCtrl(ID, now, getConfig<int>("data", "size_ctrl"), CCtrl::_rts));
		if( getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_selfish
		   && ( !buffer.empty() ) )
			packets.push_back(new CCtrl(ID, capacityBuffer - buffer.size(), now, getConfig<int>("data", "size_ctrl"), CCtrl::_capacity));

		CFrame* frame = new CFrame(*this, packets);

		return frame;
	}

	void checkDataByAck(vector<CData> ack)
	{
		RemoveFromList(buffer, ack);
	}

};

#endif // __MA_NODE_H__
