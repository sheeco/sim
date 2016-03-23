#pragma once

#include "Data.h"
#include "Route.h"
#include "Sink.h"
#include "GeneralNode.h"


class CMANode :
	public CGeneralNode
{
//protected:

//	int ID;  //MA�ڵ�ı��
//	CCoordinate location;  //MA�ڵ����ڵ�y����
//	int time;  //����MA�ڵ������ʱ���
//	bool flag;


private:

	vector<CData> buffer;
	CRoute route;
	vector<CRoute> oldRoutes;
	CHotspot *atHotspot;  //MA�����hotspot
	int waitingTime;  //�ڵ�ǰλ�õ�ʣ��waitingʱ��

//	static double energyConsumption;
	static vector<CMANode *> MANodes;
	static vector<CMANode *> freeMANodes;

	static int COUNT_ID;

	void init()
	{
		atHotspot = nullptr;	
		waitingTime = -1;
		capacityBuffer = CAPACITY_BUFFER;
	}

	CMANode()
	{
		init();
	}

	//�Զ�����ID�����ֶ�����
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


public:

	static int SPEED;
	static int CAPACITY_BUFFER;  // TODO: static getter & ref mod
	static _RECEIVE MODE_RECEIVE;

	static vector<CMANode *> getMANodes()
	{
		return MANodes;
	}

	~CMANode(){};

	//��ǰ�MA��������ʱ���ã����������õ�MA�����µ�MA
	static CMANode* newMANode(CRoute route, int time)
	{
		CMANode *result = nullptr;

		//�����µ�MA
		if( freeMANodes.empty() )
		{
			result = new CMANode(route, time);
		}
		//ʹ�����õ�MA
		else
		{
			result = freeMANodes[0];
			freeMANodes.erase( freeMANodes.begin() );
			result->updateRoute(route);
		}
		MANodes.push_back(result);
		return result;
	}

	//��ʱ����
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
		this->waitingTime = waitingTime;
	}
	inline int getWaitingTime() const
	{
		return waitingTime;
	}

	static int getCapacityBuffer()
	{
		return CAPACITY_BUFFER;
	}

	static int getSpeed()
	{
		return SPEED;
	}

	//�ж�Buffer�Ƿ�����
	inline bool isFull() const
	{
		if(buffer.size() >= CAPACITY_BUFFER)
			return true;
		else
			return false;
	}	

	// TODO: send tolerance / MAX_DATA_TRANS during as index
	//��������ʱ������������յ����������
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

	//MA�ƶ�������timeʱ�̵�λ��
	void updateLocation(int time);

};

