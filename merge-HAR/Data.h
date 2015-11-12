#pragma once

class CData
{
private:
	long int ID;
	int node;  //����node
	int timeBirth;  //����ʱ��
	int timeArrival;  //����sink��ʱ��
	bool flag;

	//����ͳ��Ͷ���ʺ�ʱ�ӵľ�̬����
	static long int ID_COUNT;  
	static long int ARRIVAL_COUNT;  //��������ݼ���
	static long double DELAY_SUM;  //ʱ�ӼӺͣ����ڼ���ƽ��ʱ��
	static long int OVERFLOW_COUNT;  //��ڵ�Buffer��������������ݼ���

	//(ע�⣺����������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���ARRIVAL_COUNT�ģ�����������;)
	static long int DELIVERY_AT_HOTSPOT_COUNT;  //���ȵ㴦�õ�Ͷ�ݵ����ݼ���
	static long int DELIVERY_ON_ROUTE_COUNT;  //��·���ϵõ�Ͷ�ݵ����ݼ���

	//�Զ�����ID
	inline void generateID()
	{
		this->ID = ID_COUNT;
		ID_COUNT++;
	}

public:
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
		this->timeArrival = -1;  //��ʼֵ-1
		this->generateID();
		this->flag = false;
	}

	~CData(void);

	inline void arriveSink(int timeArrival)
	{
		this->timeArrival = timeArrival;
		ARRIVAL_COUNT++;
		DELAY_SUM += timeArrival - timeBirth;
	}
	static void deliverAtHotspot(int n)
	{
		DELIVERY_AT_HOTSPOT_COUNT += n;
	}
	static void deliverOnRoute(int n)
	{
		DELIVERY_ON_ROUTE_COUNT += n;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���ARRIVAL_COUNT�ģ�����������;
	static long int getDeliveryAtHotspotCount()
	{
		return DELIVERY_AT_HOTSPOT_COUNT;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���ARRIVAL_COUNT�ģ�����������;
	static long int getDeliveryTotalCount()
	{
		return DELIVERY_AT_HOTSPOT_COUNT + DELIVERY_ON_ROUTE_COUNT;
	}
	static double getDeliveryAtHotspotPercent()
	{
		return (double)DELIVERY_AT_HOTSPOT_COUNT / (double)( DELIVERY_AT_HOTSPOT_COUNT + DELIVERY_ON_ROUTE_COUNT );
	}

	static inline void overflow()
	{
		OVERFLOW_COUNT++;
	}
	static long int getOverflowCount()
	{
		return OVERFLOW_COUNT;
	}
	//setters & getters
	inline void setNode(int node)
	{
		this->node = node;
	}
	inline long int getID()
	{
		return ID;
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
	inline bool getFlag()
	{
		return flag;
	}
	inline void setFlag(bool flag)
	{
		this->flag = flag;
	}

	//ͳ������
	static long int getDataCount()
	{
		return ID_COUNT;
	}
	static long int getDataArrivalCount()
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
	static long double getAverageDelay()
	{
		if(ARRIVAL_COUNT == 0)
			return 0;
		return DELAY_SUM / ARRIVAL_COUNT;
	}
	static double getAverageEnergyConsumption();

};

