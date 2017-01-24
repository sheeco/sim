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

//	int ID;  //MA�ڵ�ı��
//	CCoordinate location;  //MA�ڵ����ڵ�y����
//	int time;  //����MA�ڵ������ʱ���
//	bool flag;


private:

	CRoute route;
	vector<CRoute> oldRoutes;
	CHotspot *atHotspot;  //MA�����hotspot
	int waitingWindow;  //��ǰ�涨��waitingʱ�䴰��С
	int waitingState;  //��ǰ����waitingʱ�䴰��λ�ã�ֵΪ0˵����δ��ʼ�ȴ���ֵ�������ֵ˵���ȴ�ʱ�������

//	static double energyConsumption;
	static vector<CMANode *> MANodes;
	static vector<CMANode *> freeMANodes;

//	static int encounterActive;  //��Ч����
	static int encounter;
	static int COUNT_ID;


	void init()
	{
		if( COUNT_ID == 0 )
			COUNT_ID = START_COUNT_ID;

		setLocation( CSink::getSink()->getLocation() );  //��ʼ�� MA λ���� sink ��
		atHotspot = nullptr;	
		waitingWindow = 0;
		waitingState = 0;
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
	~CMANode(){};


public:

	static int START_COUNT_ID;
	static int SPEED;
	static int CAPACITY_BUFFER;  // TODO: static getter & ref mod
	static _RECEIVE MODE_RECEIVE;

	static vector<CMANode *>& getMANodes()
	{
		return MANodes;
	}


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

	//����������ͳ�� MA �ͽڵ������
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

	//�ж�Buffer�Ƿ�����
	inline bool isFull() const
	{
		if(buffer.size() >= CAPACITY_BUFFER)
			return true;
		else
			return false;
	}	

	//��������ʱ������������յ����������
	inline int getCapacityForward() const
	{
		int capacity = capacityBuffer - buffer.size();
		if( capacity < 0 )
			capacity = 0;

		if( MODE_RECEIVE == _selfish )
			return capacity;
		else if( MODE_RECEIVE == _loose )
			return capacityBuffer;
		else
			return 0;
	}

	//MA node: all on
	bool isAwake() const
	{
		return true;
	}

	//MA�ƶ������µȴ�ʱ�䡢λ�á�ʱ������ȴ�ʱ��
	//���·�߹��ڻ򻺴���������������sink
	void updateStatus(int time);

	CFrame* sendRTSWithCapacity(int currentTime);

	vector<CData> bufferData(int time, vector<CData> datas);

	void checkDataByAck(vector<CData> ack);

};

#endif // __MA_NODE_H__
