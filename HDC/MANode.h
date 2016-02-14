#pragma once

#include "Data.h"
#include "Route.h"
#include "Sink.h"
#include "GeneralNode.h"

using namespace std;

class CMANode :
	public CGeneralNode
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
	vector<CRoute> oldRoutes;
	CHotspot *atHotspot;  //MA到达的hotspot
	int waitingTime;  //在当前位置的剩余waiting时间

//	static double energyConsumption;
	static vector<CMANode *> MANodes;
	static vector<CMANode *> freeMANodes;

	static int ID_COUNT;

	void init()
	{
		atHotspot = nullptr;	
		waitingTime = -1;
		bufferCapacity = BUFFER_CAPACITY_MA;
	}

	CMANode(void)
	{
		init();
	}

	//自动生成ID，需手动调用
	inline void generateID()
	{
		ID_COUNT++;
		this->ID = ID_COUNT;
	}

	CMANode(CRoute route, int time)
	{
		init();
		this->route = route;
		this->x = SINK_X;
		this->y = SINK_Y;
		atHotspot = nullptr;
		this->time = time;
		generateID();
	}


public:

	static Mode BUFFER_MODE;

	static vector<CMANode *> getMANodes()
	{
		return MANodes;
	}

	~CMANode(void){};

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

	static inline double getSumEnergyConsumption()
	{
		double sumEnergyConsumption = 0;
		for(auto iMANode = MANodes.begin(); iMANode != MANodes.end(); ++iMANode)
			sumEnergyConsumption += (*iMANode)->getEnergyConsumption();
		for(auto iMANode = freeMANodes.begin(); iMANode != freeMANodes.end(); ++iMANode)
			sumEnergyConsumption += (*iMANode)->getEnergyConsumption();

		return sumEnergyConsumption;
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
		this->waitingTime = waitingTime;
	}
	inline int getWaitingTime() const
	{
		return waitingTime;
	}

	//判断Buffer是否已满
	inline bool isFull() const
	{
		if(buffer.size() >= BUFFER_CAPACITY_MA)
			return true;
		else
			return false;
	}	

	//接收数据时，返回允许接收的最大数据数
	inline int getBufferCapacity() const
	{
		int capacity = bufferCapacity - buffer.size();
		if( capacity < 0 )
			capacity = 0;

		if( BUFFER_MODE == BUFFER::SELFISH )
			return capacity;
		else if( BUFFER_MODE == BUFFER::LOOSE )
			return bufferCapacity;
		else
		{
			cout << endl << "Error @ CMANode::getBufferCapacity() : BUFFER_MODE = " << BUFFER_MODE << endl;
			_PAUSE;
		}
	}

	vector<CData>  sendAllData(Mode mode) override
	{
		return CGeneralNode::sendAllData(mode);
	}

	bool receiveData(int time, vector<CData> datas) override;

	//MA移动，更新time时刻的位置
	void updateLocation(int time);

};

