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


protected:

	double speed;
	CRoute *route;
	vector<CRoute> routeHistory;
	CBasicEntity *atPoint;
	int waitingWindow;  //��ǰ�涨��waitingʱ�䴰��С
	int waitingState;  //��ǰ����waitingʱ�䴰��λ�ã�ֵΪ0˵����δ��ʼ�ȴ���ֵ�������ֵ˵���ȴ�ʱ�������
	bool returningToSink;
	bool busy;

	static int COUNT_ID;
	static int INIT_NUM_MA;
	static int MAX_NUM_MA;
	static int CAPACITY_BUFFER;
	static double SPEED;

	//	static int encounterActive;  //��Ч����
	static int encounter;


	void init()
	{
		if( COUNT_ID == 0 )
			COUNT_ID = getConfig<int>("ma", "base_id");

		fifo = getConfig<config::EnumQueueScheme>("node", "scheme_queue") == config::_fifo;
		setLocation( CSink::getSink()->getLocation() );  //��ʼ�� MA λ���� sink ��
		speed = getConfig<int>("ma", "speed");
		atPoint = nullptr;	
		waitingWindow = INVALID;
		waitingState = INVALID;
		capacityBuffer = getConfig<int>("ma", "buffer");
		returningToSink = false;
		busy = false;
		route = nullptr;
		atPoint = nullptr;
		time = time;
		generateID();
		setName("MA #" + STRING(this->getID()));
	}
	static void Init()
	{
		INIT_NUM_MA = getConfig<int>("ma", "init_num_ma");
		MAX_NUM_MA = getConfig<int>("ma", "max_num_ma");
		CAPACITY_BUFFER = getConfig<int>("ma", "buffer");
		SPEED = getConfig<int>("ma", "speed");
	}
	//�Զ�����ID�����ֶ�����
	inline void generateID()
	{
		if(this->getID() == INVALID)
		{
			++COUNT_ID;
			this->ID = COUNT_ID;
		}
	}

	inline bool isReturningToSink()
	{
		return this->returningToSink;
	}
	inline void setReturningToSink()
	{
		this->returningToSink = true;
	}
	inline void setBusy(bool busy)
	{
		this->busy = busy;
	}
	inline bool isBusy() const
	{
		return this->busy;
	}
	inline bool hasRoute() const
	{
		return this->route != nullptr;
	}
	inline void setRoute(CRoute* route)
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
	};


public:


	inline double getSpeed() const
	{
		return this->speed;
	}
	static double getMASpeed()
	{
		return SPEED;
	}
	static int getCapacityBuffer()
	{
		return CAPACITY_BUFFER;
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

	inline CRoute* getRoute()
	{
		return route;
	}
	inline void endRoute()
	{
		if( this->route == nullptr )
			return;

		this->routeHistory.push_back(*this->route);
		FreePointer(this->route);
		this->route = nullptr;
		atPoint = nullptr;
		endWaiting();
		this->returningToSink = false;
	}
	inline void updateRoute(CRoute *route)
	{
		this->endRoute();
		this->setRoute(route);
	}
	inline bool isWaiting() const
	{
		return this->waitingWindow >= 0;
	}
	inline void setWaiting(double window)
	{
		if( window < 0 )
			throw string("CMANode::setWaiting(): window = " + STRING(window));

		this->endWaiting();
		this->waitingWindow = (int)window;
		this->waitingState = 0;
	}
	//�ȴ���������ʱ�䴰����
	inline void endWaiting()
	{
		this->waitingWindow = this->waitingState = INVALID;
	}
	//���µȴ�״̬��������½ڵ�ʱ���
	//���صȴ�������ʣ��Ľ������ƶ���ʱ�䣻���� 0 ��ζ�ŵȴ�δ����/�պý���
	inline int wait(int duration)
	{
		if( !this->isWaiting() )
			throw string("CMANode::wait(): " + this->getName() + " is not waiting.");

		int timeLeft = 0;
		//�ȴ���������		
		if( ( waitingState + duration ) >= waitingWindow )
		{
			timeLeft = waitingState + duration - waitingWindow;  //�ȴ�������ʣ��Ľ������ƶ���ʱ��
			this->endWaiting();
			route->updateToPoint();
		}

		//�ȴ���δ����
		else
		{
			waitingState += duration;
		}
		return timeLeft;
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

	vector<CData> bufferData(int now, vector<CData> datas)
	{
		vector<CData> ack = CGeneralNode::bufferData(now, datas);

		if( isAtWaypoint() )
		{
			CData::deliverAtWaypoint(ack.size());
		}
		else
			CData::deliverOnRoute(ack.size());

		return ack;
	}

	//MA node: all on
	bool isAwake() const
	{
		return true;
	}

	//NOTE: �麯�����嵼��VS�������޷���ȷ��ʾthisָ��
	//MA�ƶ������µȴ�ʱ�䡢λ�á�ʱ������ȴ�ʱ��
	//���·�߹��ڻ򻺴���������������sink
	//virtual void updateStatus(int time) = 0;

	CFrame* sendRTSWithCapacity(int now)
	{
		vector<CPacket*> packets;
		packets.push_back(new CCtrl(ID, now, getConfig<int>("data", "size_ctrl"), CCtrl::_rts));
		packets.push_back(new CCtrl(ID, this->getBufferVacancy(), now, getConfig<int>("data", "size_ctrl"), CCtrl::_capacity));

		CFrame* frame = new CFrame(*this, packets);

		return frame;
	}

	void dropDataByAck(vector<CData> ack)
	{
		RemoveFromList(buffer, ack);
	}

	//virtual void updateStatus(int time) = 0;
};

#endif // __MA_NODE_H__
