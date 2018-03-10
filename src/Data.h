#pragma once

#ifndef __DATA_H__
#define __DATA_H__

#include "Packet.h"
#include "Configuration.h"


class CData : 
	virtual public CPacket
{
//protected:

//	int ID;  //data���
//	CCoordinate location;  //δʹ��
//	int time;  //��data���һ��״̬���µ�ʱ���������У�飬��ʼֵӦ����timeBirth
//	bool flag;
//	int node;  //����node
//	int timeBirth;  //����ʱ��
//	int size;  //byte
//	int HOP;


private:

	int timeArrival;  //����sink��ʱ��

//	static int ID_MASK;

	//����ͳ��Ͷ���ʺ�ʱ�ӵľ�̬����
	static int COUNT_ID;  //��ֵ����data������
	static int COUNT_ARRIVAL;  //��������ݼ���
	static double SUM_DELAY;  //ʱ�ӼӺͣ����ڼ���ƽ��ʱ��
	static double SUM_HOP;  //�����Ӻͣ����ڼ���ƽ������

	static int COUNT_DELIVERY_AT_WAYPOINT;  //���ȵ㴦�õ�Ͷ�ݵ����ݼ���
	static int COUNT_DELIVERY_ON_ROUTE;  //��·���ϵõ�Ͷ�ݵ����ݼ���

	CData()
	{
		CData::init();
	};

	//�Զ�����ID
	//ID = node_id * 10 000 000 + data_counter ��������SV��ʶ��Data��Դ
	inline void generateID()
	{
		++COUNT_ID;
//		this->ID = node * ID_MASK + COUNT_ID;
		this->ID = COUNT_ID;
	}


protected:

	void init()
	{
		CPacket::init();
		this->timeArrival = INVALID;
		this->HOP = 0;
		this->MAX_HOP = getConfig<int>("data", "max_hop");
	}


public:

	CData(int node, int timeBirth, int byte)
	{
		CData::init();
		this->node = node;
		this->time = this->timeBirth = timeBirth;
		this->size = byte;
		this->generateID();
	}

	~CData(){};

	static void deliverAtWaypoint(int n)
	{
		COUNT_DELIVERY_AT_WAYPOINT += n;
	}
	static void deliverOnRoute(int n)
	{
		COUNT_DELIVERY_ON_ROUTE += n;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���COUNT_ARRIVAL�ģ�����������;
	static int getCountDeliveryAtWaypoint()
	{
		return COUNT_DELIVERY_AT_WAYPOINT;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���COUNT_ARRIVAL�ģ�����������;
	static int getCountDeliveryTotal()
	{
		return COUNT_DELIVERY_AT_WAYPOINT + COUNT_DELIVERY_ON_ROUTE;
	}
	static double getPercentDeliveryAtWaypoint()
	{
		if(COUNT_DELIVERY_AT_WAYPOINT == 0)
			return 0.0;
		return double(COUNT_DELIVERY_AT_WAYPOINT) / double( COUNT_DELIVERY_AT_WAYPOINT + COUNT_DELIVERY_ON_ROUTE );
	}

	//setters & getters
	inline int getTimeArrival() const
	{
		return timeArrival;
	}

	inline void arriveSink(int timeArrival)
	{
		this->timeArrival = timeArrival;
		this->time = timeArrival;
		++COUNT_ARRIVAL;
		SUM_DELAY += timeArrival - timeBirth;
		SUM_HOP += HOP;
	}

	//ͳ������
	static int getCountData()
	{
		return COUNT_ID;
	}
	static int getCountDelivery()
	{
		return COUNT_ARRIVAL;
	}
	static double getDeliveryRatio()
	{
		if(COUNT_ID == 0)
			return 0;
		else
			return double(COUNT_ARRIVAL) / double(COUNT_ID);
	}
	static double getAverageDelay()
	{
		if(COUNT_ARRIVAL == 0)
			return 0;
		return SUM_DELAY / COUNT_ARRIVAL;
	}
	static double getAverageHOP()
	{
		if(COUNT_ARRIVAL == 0)
			return 0;
		return SUM_HOP / COUNT_ARRIVAL;
	}
	static double getAverageEnergyConsumption();

	static vector<CData> GetItemsByID(vector<CData> list, vector<int> ids);

	//���رȽϲ��������Ƚ�����ʱ�䣬����mergeSort
	friend bool operator < (const CData lt, const CData rt);
	friend bool operator > (const CData lt, const CData rt);
	//���� == ���������Ƚ� ID������ȥ��
	friend bool operator == (const CData lt, const CData rt);
	//���ز����� == ���ڸ��� ID �ж� identical
	friend bool operator == (int id, const CData data);
	friend bool operator == (const CData data, int id);

};

#endif // __DATA_H__
