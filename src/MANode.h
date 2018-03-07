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

	CRoute *route;
	vector<CRoute> routeHistory;
	CBasicEntity *atPoint;
	int waitingWindow;  //��ǰ�涨��waitingʱ�䴰��С
	int waitingState;  //��ǰ����waitingʱ�䴰��λ�ã�ֵΪ0˵����δ��ʼ�ȴ���ֵ�������ֵ˵���ȴ�ʱ�������
	bool returnAtOnce;

//	static int encounterActive;  //��Ч����
	static int encounter;
	static int COUNT_ID;


	void init()
	{
		if( COUNT_ID == 0 )
			COUNT_ID = getConfig<int>("ma", "base_id");

		setLocation( CSink::getSink()->getLocation() );  //��ʼ�� MA λ���� sink ��
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

	//�Զ�����ID�����ֶ�����
	inline void generateID()
	{
		if(this->getID() == INVALID)
		{
			++COUNT_ID;
			this->ID = COUNT_ID;
		}
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
		if( this->route != nullptr )
		{
			this->routeHistory.push_back(*this->route);
			FreePointer(this->route);
			this->route = nullptr;
		}
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
	inline void setAtHotspot(CBasicEntity *wayPoint)
	{
		this->atPoint = wayPoint;
	}

	//TODO: include when im still in range of the hotspot after leaving the center, maybe introducing m_lastAtHotspot
	inline bool isAtHotspot() const
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

	//�ж�Buffer�Ƿ�����
	inline bool isFull() const
	{
		if(buffer.size() >= getConfig<int>("ma", "buffer"))
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

	//NOTE: �麯�����嵼��VS�������޷���ȷ��ʾthisָ��
	//MA�ƶ������µȴ�ʱ�䡢λ�á�ʱ������ȴ�ʱ��
	//���·�߹��ڻ򻺴���������������sink
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
