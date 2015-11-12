#pragma once

class CData
{
private:
	long int ID;
	int node;  //所属node
	int timeBirth;  //生成时间
	int timeArrival;  //到达sink的时间
	bool flag;

	//用于统计投递率和时延的静态变量
	static long int ID_COUNT;  
	static long int ARRIVAL_COUNT;  //到达的数据计数
	static long double DELAY_SUM;  //时延加和，用于计算平均时延
	static long int OVERFLOW_COUNT;  //因节点Buffer溢出被丢弃的数据计数

	//(注意：由于这两个计数的统计发生在MA，因此这两个值的加和总是大于等于ARRIVAL_COUNT的，仅作测试用途)
	static long int DELIVERY_AT_HOTSPOT_COUNT;  //在热点处得到投递的数据计数
	static long int DELIVERY_ON_ROUTE_COUNT;  //在路径上得到投递的数据计数

	//自动生成ID
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
		this->timeArrival = -1;  //初始值-1
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
	//该函数应当在MA的路径更新时调用输出统计结果
	//注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于ARRIVAL_COUNT的，仅作测试用途
	static long int getDeliveryAtHotspotCount()
	{
		return DELIVERY_AT_HOTSPOT_COUNT;
	}
	//该函数应当在MA的路径更新时调用输出统计结果
	//注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于ARRIVAL_COUNT的，仅作测试用途
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

	//统计数据
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

