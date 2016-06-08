#pragma once

#ifndef __DATA_H__
#define __DATA_H__

#include "GeneralData.h"


class CData : 
	virtual public CGeneralData
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
//	//������TTL����������Epidemic��ѡ��һ��ʹ��
//	int TTL;

//	static int ID_MASK;

	//����ͳ��Ͷ���ʺ�ʱ�ӵľ�̬����
	static int COUNT_ID;  //��ֵ����data������
	static int COUNT_ARRIVAL;  //��������ݼ���
	static double SUM_DELAY;  //ʱ�ӼӺͣ����ڼ���ƽ��ʱ��
	static double SUM_HOP;  //�����Ӻͣ����ڼ���ƽ������

	static int COUNT_DELIVERY_AT_HOTSPOT;  //���ȵ㴦�õ�Ͷ�ݵ����ݼ���
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
		CGeneralData::init();
		this->timeArrival = -1;
//		this->TTL = 0;
	}


public:

	//������ TTL �������ĳ�ʼֵ���� Epidemic ��ѡ��һ��ʹ�ã�Ĭ��ֵ���� 0�������� main �����л�ͨ�������в�����ֵ
	//ע�⣺���߲���ͬʱȡ����ֵ����Ϊ��ֵҲ���ڸ�ѡ��������ж�
	static int MAX_HOP;
//	static int MAX_TTL;

	CData(int node, int timeBirth, int byte)
	{
		CData::init();
		this->node = node;
		this->time = this->timeBirth = timeBirth;
		this->size = byte;
		this->generateID();
		this->HOP = MAX_HOP;
//		this->TTL = MAX_TTL;
	}

	~CData(){};

	static void deliverAtHotspot(int n)
	{
		COUNT_DELIVERY_AT_HOTSPOT += n;
	}
	static void deliverOnRoute(int n)
	{
		COUNT_DELIVERY_ON_ROUTE += n;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���COUNT_ARRIVAL�ģ�����������;
	static int getCountDeliveryAtHotspot()
	{
		return COUNT_DELIVERY_AT_HOTSPOT;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���COUNT_ARRIVAL�ģ�����������;
	static int getCountDeliveryTotal()
	{
		return COUNT_DELIVERY_AT_HOTSPOT + COUNT_DELIVERY_ON_ROUTE;
	}
	static double getPercentDeliveryAtHotspot()
	{
		if(COUNT_DELIVERY_AT_HOTSPOT == 0)
			return 0.0;
		return double(COUNT_DELIVERY_AT_HOTSPOT) / double( COUNT_DELIVERY_AT_HOTSPOT + COUNT_DELIVERY_ON_ROUTE );
	}

	//setters & getters
	inline int getTimeArrival() const
	{
		return timeArrival;
	}

	//ʵ����ֻ����TTL
	inline void updateStatus(int currentTime)
	{
//		this->TTL -= ( currentTime - time );
		this->time = currentTime;
	}
	inline void arriveSink(int timeArrival)
	{
		this->timeArrival = timeArrival;
		this->time = timeArrival;
		++COUNT_ARRIVAL;
		SUM_DELAY += timeArrival - timeBirth;
		--HOP;
		SUM_HOP += -HOP;
	}

	// TODO: call this func when receiving anything
	//�����ݱ�ת�������µĽڵ��Ӧ�õ��õĺ�����������������TTLʣ��ֵ��������ʱ���
	//ע�⣺���ݷ��ͷ�Ӧ�ڷ���֮ǰ���ʣ��HOP����1
	inline void arriveAnotherNode(int currentTime)
	{
		this->HOP--;

//		this->TTL -= ( currentTime - time );
		this->timeArrival = currentTime;
		this->time = currentTime;
	}

//	//�ж��Ƿ��Ѿ�����������(TTL <= 0)������Ӧ����
//	inline bool isOverdue() const
//	{
//		if( ! useTTL() )
//			return false;
//		else
//			return TTL <= 0;
//	}
	
	//�ж��Ƿ�����ת����HOP > 0�����������򲻷���SV��
	inline bool allowForward() const
	{
		if( ! useHOP() )
			return true;
		else
			return HOP > 0;
	}

	static bool useHOP()
	{
		return MAX_HOP > 0;
	}
//	static bool useTTL()
//	{
//		return MAX_TTL > 0;	
//	}

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
	//���� == ���������Ƚ� ID������ȥ��
	friend bool operator == (const CData lt, const CData rt);
	//���ز����� == ���ڸ��� ID �ж� identical
	friend bool operator == (int id, const CData data);
	friend bool operator == (const CData data, int id);

};

#endif // __DATA_H__
