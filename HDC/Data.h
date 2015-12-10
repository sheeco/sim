#pragma once

#include "BasicEntity.h"

using namespace std;

class CData : 
	public CBasicEntity
{
//protected:
//	int ID;  //data编号
//	double x;  //未使用
//	double y;  //未使用
//	int time;  //该data最后一次状态更新的时间戳，用于校验，初始值应等于timeBirth
//	bool flag;

private:
	int node;  //所属node
	int timeBirth;  //生成时间
	int timeArrival;  //到达sink的时间
	//跳数或TTL倒计数，在Epidemic中选择一种使用
	int HOP;
	int TTL;

	//跳数或TTL倒计数的初始值，在Epidemic中选择一种使用，默认值都是0，必须在main函数中或通过命令行参数赋值
	//注意：两者不能同时取非零值，因为该值也用于该选项的所有判断
	static bool MAX_HOP;
	static bool MAX_TTL;
	static int ID_MASK;

	//用于统计投递率和时延的静态变量
	static int ID_COUNT;  //数值等于data的总数
	static int ARRIVAL_COUNT;  //到达的数据计数
	static double DELAY_SUM;  //时延加和，用于计算平均时延
	//static int OVERFLOW_COUNT;  //因节点Buffer溢出被丢弃的数据计数


	//(注意：由于这两个计数的统计发生在MA，因此这两个值的加和总是大于等于ARRIVAL_COUNT的，仅作测试用途)
	//static int DELIVERY_AT_HOTSPOT_COUNT;  //在热点处得到投递的数据计数
	//static int DELIVERY_ON_ROUTE_COUNT;  //在路径上得到投递的数据计数

	//自动生成ID
	//ID = node_id * 10 000 000 + data_counter ，用于在SV中识别Data来源
	inline void generateID()
	{
		this->ID = node * ID_MASK + ID_COUNT;
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
		this->time = timeBirth;
		this->timeArrival = -1;  //初始值-1
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
	////该函数应当在MA的路径更新时调用输出统计结果
	////注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于ARRIVAL_COUNT的，仅作测试用途
	//static int getDeliveryAtHotspotCount()
	//{
	//	return DELIVERY_AT_HOTSPOT_COUNT;
	//}
	////该函数应当在MA的路径更新时调用输出统计结果
	////注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于ARRIVAL_COUNT的，仅作测试用途
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

	//重载比较操作符，用于mergeSort
	bool operator <= (CData rt)
	{
		return this->timeBirth <= rt.getTimeBirth();
	}

	//实际上只更新TTL
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

	//该数据被转发到达新的节点后应该调用的函数，将更新跳数或TTL剩余值，并更新时间戳
	//注意：数据发送方应在发送之前检查剩余HOP大于1
	inline void arriveAnotherNode(int currentTime)
	{
		if( useHOP() )
			this->HOP--;
		else
			this->TTL -= ( currentTime - time );
		this->time = currentTime;
	}

	//判断是否已经超过生存期(TTL <= 0)，超出应丢弃
	inline bool isOverdue()
	{
		if( useHOP() )
			return false;
		else
			return TTL <= 0;
	}
	
	//判断是否允许转发（HOP > 1），不允许则不放入SV中
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

	//统计数据
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

