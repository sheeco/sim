#pragma once

#include "GeneralData.h"


class CData : 
	public CGeneralData
{
//protected:

//	int ID;  //data编号
//	CCoordinate location;  //未使用
//	int time;  //该data最后一次状态更新的时间戳，用于校验，初始值应等于timeBirth
//	bool flag;
//	int node;  //所属node
//	int timeBirth;  //生成时间
//	int size;  //byte
//	int HOP;


private:

	int timeArrival;  //到达sink的时间
//	//跳数或TTL倒计数，在Epidemic中选择一种使用
//	int TTL;

//	static int ID_MASK;

	//用于统计投递率和时延的静态变量
	static int COUNT_ID;  //数值等于data的总数
	static int COUNT_ARRIVAL;  //到达的数据计数
	static double SUM_DELAY;  //时延加和，用于计算平均时延
	static double SUM_HOP;  //跳数加和，用于计算平均跳数

	static int COUNT_DELIVERY_AT_HOTSPOT;  //在热点处得到投递的数据计数
	static int COUNT_DELIVERY_ON_ROUTE;  //在路径上得到投递的数据计数

	CData()
	{
		CData::init();
	};

	//自动生成ID
	//ID = node_id * 10 000 000 + data_counter ，用于在SV中识别Data来源
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

	//跳数或 TTL 倒计数的初始值，在 Epidemic 中选择一种使用，默认值都是 0，必须在 main 函数中或通过命令行参数赋值
	//注意：两者不能同时取非零值，因为该值也用于该选项的所有判断
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
	//该函数应当在MA的路径更新时调用输出统计结果
	//注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于COUNT_ARRIVAL的，仅作测试用途
	static int getCountDeliveryAtHotspot()
	{
		return COUNT_DELIVERY_AT_HOTSPOT;
	}
	//该函数应当在MA的路径更新时调用输出统计结果
	//注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于COUNT_ARRIVAL的，仅作测试用途
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

	//重载比较操作符，用于mergeSort
	bool operator < (CData rt) const
	{
		return this->timeBirth < rt.getTimeBirth();
	}

	//重载比较操作符，用于去重
	bool operator == (CData rt) const
	{
		return this->ID == rt.getID();
	}

	// TODO: need test
	//重载操作符==用于根据ID判断identical
	bool operator == (int id) const
	{
		return this->ID == id;
	}

	//实际上只更新TTL
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
	//该数据被转发到达新的节点后应该调用的函数，将更新跳数或TTL剩余值，并更新时间戳
	//注意：数据发送方应在发送之前检查剩余HOP大于1
	inline void arriveAnotherNode(int currentTime)
	{
		this->HOP--;

//		this->TTL -= ( currentTime - time );
		this->timeArrival = currentTime;
		this->time = currentTime;
	}

//	//判断是否已经超过生存期(TTL <= 0)，超出应丢弃
//	inline bool isOverdue() const
//	{
//		if( ! useTTL() )
//			return false;
//		else
//			return TTL <= 0;
//	}
	
	//判断是否允许转发（HOP > 0），不允许则不放入SV中
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

	//统计数据
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

};

