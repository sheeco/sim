#pragma once

#include "BasicEntity.h"

using namespace std;

class CData : 
	public CBasicEntity
{
//protected:
//	int ID;  //data���
//	double x;  //δʹ��
//	double y;  //δʹ��
//	int time;  //��data���һ��״̬���µ�ʱ���������У�飬��ʼֵӦ����timeBirth
//	bool flag;

private:
	int node;  //����node
	int timeBirth;  //����ʱ��
	int timeArrival;  //����sink��ʱ��
	//������TTL����������Epidemic��ѡ��һ��ʹ��
	int HOP;
	int TTL;

	static int ID_MASK;

	//����ͳ��Ͷ���ʺ�ʱ�ӵľ�̬����
	static int ID_COUNT;  //��ֵ����data������
	static int ARRIVAL_COUNT;  //��������ݼ���
	static double DELAY_SUM;  //ʱ�ӼӺͣ����ڼ���ƽ��ʱ��
	//static int OVERFLOW_COUNT;  //��ڵ�Buffer��������������ݼ���

	//static int DELIVERY_AT_HOTSPOT_COUNT;  //���ȵ㴦�õ�Ͷ�ݵ����ݼ���
	//static int DELIVERY_ON_ROUTE_COUNT;  //��·���ϵõ�Ͷ�ݵ����ݼ���

	//�Զ�����ID
	//ID = node_id * 10 000 000 + data_counter ��������SV��ʶ��Data��Դ
	inline void generateID()
	{
		this->ID = node * ID_MASK + ID_COUNT;
		ID_COUNT++;
	}


public:

	//������TTL�������ĳ�ʼֵ����Epidemic��ѡ��һ��ʹ�ã�Ĭ��ֵ����0��������main�����л�ͨ�������в�����ֵ
	//ע�⣺���߲���ͬʱȡ����ֵ����Ϊ��ֵҲ���ڸ�ѡ��������ж�
	static bool MAX_HOP;
	static bool MAX_TTL;

	CData(void)
	{
		ID = -1;
		node = -1;
		timeBirth = 0;
		flag = false;
	}

	CData(int node, int timeBirth)
	{
		this->node = node;
		this->timeBirth = timeBirth;
		this->time = timeBirth;
		this->timeArrival = -1;  //��ʼֵ-1
		this->generateID();
		this->flag = false;
	}

	~CData(void);

	//static void deliverAtHotspot(int n)
	//{
	//	DELIVERY_AT_HOTSPOT_COUNT += n;
	//}
	//static void deliverOnRoute(int n)
	//{
	//	DELIVERY_ON_ROUTE_COUNT += n;
	//}
	////�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	////ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���ARRIVAL_COUNT�ģ�����������;
	//static int getDeliveryAtHotspotCount()
	//{
	//	return DELIVERY_AT_HOTSPOT_COUNT;
	//}
	////�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	////ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���ARRIVAL_COUNT�ģ�����������;
	//static int getDeliveryTotalCount()
	//{
	//	return DELIVERY_AT_HOTSPOT_COUNT + DELIVERY_ON_ROUTE_COUNT;
	//}
	//static double getDeliveryAtHotspotPercent()
	//{
	//	return (double)DELIVERY_AT_HOTSPOT_COUNT / (double)( DELIVERY_AT_HOTSPOT_COUNT + DELIVERY_ON_ROUTE_COUNT );
	//}

	//static inline void overflow()
	//{
	//	OVERFLOW_COUNT++;
	//}
	//static int getOverflowCount()
	//{
	//	return OVERFLOW_COUNT;
	//}
	//setters & getters
	inline void setNode(int node)
	{
		this->node = node;
	}
	inline int getTimeBirth()
	{
		return timeBirth;
	}
	inline int getTimeArrival()
	{
		return timeArrival;
	}
	inline int getNode()
	{
		return node;
	}

	//���رȽϲ�����������mergeSort
	bool operator < (CData rt)
	{
		return this->timeBirth < rt.getTimeBirth();
	}

	//���رȽϲ�����������ȥ��
	bool operator == (CData rt)
	{
		return this->ID == rt.getID();
	}

	bool operator == (int id)
	{
		return this->ID == id;
	}

	//ʵ����ֻ����TTL
	inline void updateStatus(int currentTime)
	{
		if( useTTL() )
			this->TTL -= ( currentTime - time );
		this->time = currentTime;
	}
	inline void arriveSink(int timeArrival)
	{
		this->timeArrival = timeArrival;
		this->time = timeArrival;
		ARRIVAL_COUNT++;
		DELAY_SUM += timeArrival - timeBirth;
	}

	//�����ݱ�ת�������µĽڵ��Ӧ�õ��õĺ�����������������TTLʣ��ֵ��������ʱ���
	//ע�⣺���ݷ��ͷ�Ӧ�ڷ���֮ǰ���ʣ��HOP����1
	inline void arriveAnotherNode(int currentTime)
	{
		if( useHOP() )
			this->HOP--;
		else
			this->TTL -= ( currentTime - time );
		this->time = currentTime;
	}

	//�ж��Ƿ��Ѿ�����������(TTL <= 0)������Ӧ����
	inline bool isOverdue()
	{
		if( useHOP() )
			return false;
		else
			return TTL <= 0;
	}
	
	//�ж��Ƿ�����ת����HOP > 1�����������򲻷���SV��
	inline bool allowForward()
	{
		if( useTTL() )
			return true;
		else
			return HOP > 1;
	}

	static bool useHOP()
	{
		if( MAX_HOP > 0 && MAX_TTL > 0 )
		{
			cout << "Error @ CData::useHOP() : INIT_HOP > 0 && INIT_TTL > 0 " << endl;
			_PAUSE;
		}
		else
			return MAX_HOP > 0;
	}
	static bool useTTL()
	{
		if( MAX_HOP > 0 && MAX_TTL > 0 )
		{
			cout << "Error @ CData::useTTL() : INIT_HOP > 0 && INIT_TTL > 0 " << endl;
			_PAUSE;
		}
		else
			return MAX_TTL > 0;	
	}
	static int getNodeByMask(int id)
	{
		return id / ID_MASK;
	}

	//ͳ������
	static int getDataCount()
	{
		return ID_COUNT;
	}
	static int getDataArrivalCount()
	{
		return ARRIVAL_COUNT;
	}
	static double getDeliveryRatio()
	{
		if(ID_COUNT == 0)
			return 0;
		else
			return (double)ARRIVAL_COUNT / (double)ID_COUNT;
	}
	static double getAverageDelay()
	{
		if(ARRIVAL_COUNT == 0)
			return 0;
		return DELAY_SUM / ARRIVAL_COUNT;
	}
	static double getAverageEnergyConsumption();

};

