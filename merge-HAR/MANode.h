#pragma once

#include "Data.h"
#include "Route.h"
#include "Sink.h"

class CMANode :
	public CBase
{
//protected:
//	int ID;  //MA节点的编号
//	double x;  //MA节点现在的x坐标
//	double y;  //MA节点现在的y坐标
//	int time;  //更新MA节点坐标的时间戳
//	bool flag;

private:
	vector<CData> buffer;
	CRoute route;
	//bool atSink;  //MA是否到达了Sink
	CHotspot *atHotspot;  //MA到达的hotspot
	int waitingTime;  //在当前位置的剩余waiting时间

	static double energyConsumption;
	static long int ID_COUNT;

	static int encounter;
	static int encounterAtHotspot;
	static int encounterOnRoute;

public:
	CMANode(void)
	{
	}

	CMANode(CRoute route, int time)
	{
		this->route = route;
		this->x = SINK_X;
		this->y = SINK_Y;
		this->time = time;
		//atSink = true;
		atHotspot = NULL;
		waitingTime = -1;
		generateID();
	}

	~CMANode(void){}

	//自动生成ID，需手动调用
	inline void generateID()
	{
		if(this->ID != -1)
			return;
		this->ID = ID_COUNT;
		ID_COUNT++;
	}

	static inline double getEnergyConsumption()
	{
		return energyConsumption;
	}
	inline int getBufferSize()
	{
		return buffer.size();
	}
	inline CRoute* getRoute()
	{
		return &route;
	}
	inline void setRoute(CRoute route)
	{
		this->route = route;
	}
	inline bool routeIsOverdue()
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
	
	//判断MA是否位于sink处
	//inline bool isAtSink()
	//{
	//	return atSink;
	//}

	inline bool isAtHotspot()
	{
		if(atHotspot == NULL)
			return false;
		else
			return true;
	}
	inline CHotspot* getAtHotspot()
	{
		return atHotspot;
	}
	inline void setWaitingTime(int waitingTime)
	{
		this->waitingTime = waitingTime;
	}
	inline int getWaitingTime()
	{
		return waitingTime;
	}

	//判断Buffer是否已满
	inline bool isFull()
	{
		if(buffer.size() >= BUFFER_CAPACITY_MA)
			return true;
		else
			return false;
	}	
	//buffer空余空间大小
	inline int getBufferCapacity()
	{
		if(buffer.size() >= BUFFER_CAPACITY_MA)
			return 0;
		else
			return ( BUFFER_CAPACITY_MA - buffer.size() );
	}
	//相遇计数
	inline static double getEncounterPercentAtHotspot()
	{
		return (double)encounterAtHotspot / (double)encounter;
	}
	inline static int getEncounter()
	{
		return encounter;
	}
	inline static int getEncounterAtHotspot()
	{
		return encounterAtHotspot;
	}

	//用于记录MA节点与sensor的相遇计数
	inline static void encountAtHotspot()
	{
		encounterAtHotspot++;
		encounter++;
	}
	inline static void encountOnRoute()
	{
		encounterOnRoute++;
		encounter++;
	}

	//接收data
	void receiveData(int time, vector<CData> data);

	//发送所有data
	vector<CData> sendAllData();

	//MA移动，更新time时刻的位置
	void updateLocation(int time);

};

