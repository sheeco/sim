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

	static int ID_COUNT;

	void init()
	{
		atHotspot = nullptr;	
		waitingTime = -1;
		bufferCapacity = BUFFER_CAPACITY;
	}

	CMANode()
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
		this->setLocation(CSink::SINK_X, CSink::SINK_Y);
		atHotspot = nullptr;
		this->time = time;
		generateID();
	}


public:

	static int SPEED;
	static int BUFFER_CAPACITY;  // TODO: static getter & ref mod
	static _RECEIVE RECEIVE_MODE;

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

	static int getBufferCapacity()
	{
		return BUFFER_CAPACITY;
	}

	static int getSpeed()
	{
		return SPEED;
	}

	//�ж�Buffer�Ƿ�����
	inline bool isFull() const
	{
		if(buffer.size() >= BUFFER_CAPACITY)
			return true;
		else
			return false;
	}	

	//��������ʱ������������յ����������
	inline int getDataTolerance() const
	{
		int tolerance = bufferCapacity - buffer.size();
		if( tolerance < 0 )
			tolerance = 0;

		if( RECEIVE_MODE == _selfish )
			return tolerance;
		else if( RECEIVE_MODE == _loose )
			return bufferCapacity;
		else
		{
			cout << endl << "Error @ CMANode::getDataTolerance() : RECEIVE_MODE = " << RECEIVE_MODE << endl;
			// TODO: add Exit( proper code ) after all errors
			_PAUSE_;
			Exit(-1);
		}
	}

	bool isListening() const override
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

