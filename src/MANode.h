#pragma once

#include "Data.h"
#include "Route.h"
#include "Sink.h"
#include "GeneralNode.h"


class CMANode :
	public CGeneralNode
{
//protected:

//	int ID;  //MA节点的编号
//	CCoordinate location;  //MA节点现在的y坐标
//	int time;  //更新MA节点坐标的时间戳
//	bool flag;


private:

	vector<CData> buffer;
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
		atHotspot = nullptr;	
		waitingWindow = 0;
		waitingState = 0;
		capacityBuffer = CAPACITY_BUFFER;
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
		this->setLocation(CSink::SINK_X, CSink::SINK_Y);
		atHotspot = nullptr;
		this->time = time;
		generateID();
	}
	~CMANode(){};


public:

	static int SPEED;
	static int CAPACITY_BUFFER;  // TODO: static getter & ref mod
	static _RECEIVE MODE_RECEIVE;

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
		return CAPACITY_BUFFER;
	}

	static int getSpeed()
	{
		return SPEED;
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
	inline void setWaitingTime(int waitingTime)
	{
		this->waitingWindow = waitingTime;
	}
	inline int getWaitingTime() const
	{
		return waitingWindow;
	}

	//判断Buffer是否已满
	inline bool isFull() const
	{
		if(buffer.size() >= CAPACITY_BUFFER)
			return true;
		else
			return false;
	}	

	// TODO: send tolerance / CAPACITY_FORWARD during as index
	//接收数据时，返回允许接收的最大数据数
	inline int getToleranceData() const
	{
		int tolerance = capacityBuffer - buffer.size();
		if( tolerance < 0 )
			tolerance = 0;

		if( MODE_RECEIVE == _selfish )
			return tolerance;
		else if( MODE_RECEIVE == _loose )
			return capacityBuffer;
	}

	//MA node: all on
	bool isListening() const
	{
		return true;
	}

//	void receivePackage(CPackage* package, int currentTime) override
//	{
//	}

//	vector<CData>  sendAllData(_SEND mode) override
//	{
//		return CGeneralNode::sendAllData(mode);
//	}

//	bool receiveData(int time, vector<CData> datas) override;

	CPackage* sendRTS(int currentTime);

	//MA移动，更新等待时间、位置、时间戳
	void updateStatus(int time);

};

