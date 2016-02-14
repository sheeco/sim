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
//	int ID;  //MA�ڵ�ı��
//	double x;  //MA�ڵ����ڵ�x����
//	double y;  //MA�ڵ����ڵ�y����
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

	//�Զ�����ID�����ֶ�����
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

	//�ж�Buffer�Ƿ�����
	inline bool isFull() const
	{
		if(buffer.size() >= BUFFER_CAPACITY_MA)
			return true;
		else
			return false;
	}	

	//��������ʱ������������յ����������
	inline int getBufferCapacity() const
	{
		int capacity = bufferCapacity - buffer.size();
		if( capacity < 0 )
			capacity = 0;

		if( BUFFER_MODE == BUFFER::SELFISH )
			return capacity;
		else if( BUFFER_MODE == BUFFER::LOOSE )
			return bufferCapacity;
	}

	vector<CData>  sendAllData(Mode mode) override
	{
		return CGeneralNode::sendAllData(mode);
	}

	bool receiveData(int time, vector<CData> datas) override;

	//MA�ƶ�������timeʱ�̵�λ��
	void updateLocation(int time);

};

