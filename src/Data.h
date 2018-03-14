#pragma once

#ifndef __DATA_H__
#define __DATA_H__

#include "Entity.h"


class CGeneralData :
	virtual public CBasicEntity
{
//protected:

//	CCoordinate location;  //未使用
//	int time;  //该data最后一次状态更新的时间戳，用于校验，初始值应等于timeBirth
//	bool flag;

public:

	CGeneralData(){};
	virtual ~CGeneralData() = 0
	{};

};


class CPacket :
	virtual public CGeneralData
{
protected:

	int node;  //所属node
	int timeBirth;  //生成时间
	int size;  //byte
	int HOP;
	int MAX_HOP;

	virtual void init();


public:

	CPacket();
	virtual ~CPacket() = 0
	{
	};

	inline int getNode() const
	{
		return node;
	}
	inline int getTimeBirth() const
	{
		return timeBirth;
	}
	inline int getSize() const
	{
		return size;
	}
	static int getSumSize(vector<CPacket*> packets);

	//该数据被转发到达新的节点后应该调用的函数，将更新跳数或TTL剩余值，并更新时间戳
	//注意：数据发送方应在发送之前检查剩余HOP大于1
	inline void arriveAnotherNode(int now)
	{
		this->HOP++;
	};

	//判断是否允许转发（HOP > 0），不允许则不放入SV中
	inline bool allowForward() const
	{
		return MAX_HOP <= 0 || HOP < MAX_HOP;
	}
};


class CData :
	virtual public CPacket, virtual public CUnique
{
private:

	int timeArrival;  //到达sink的时间

	//用于统计投递率和时延的静态变量
	static int COUNT_ID;  //数值等于data的总数
	static int COUNT_ARRIVAL;  //到达的数据计数
	static double SUM_DELAY;  //时延加和，用于计算平均时延
	static double SUM_HOP;  //跳数加和，用于计算平均跳数

	static int COUNT_DELIVERY_AT_WAYPOINT;  //在热点处得到投递的数据计数
	static int COUNT_DELIVERY_ON_ROUTE;  //在路径上得到投递的数据计数

	CData()
	{
		CData::init();
	};

	//自动生成ID
	//ID = node_id * 10 000 000 + data_counter ，用于在SV中识别Data来源
	inline void generateID() override
	{
		CUnique::generateID(COUNT_ID);
	}


protected:

	void init();


public:

	CData(int node, int timeBirth, int byte)
	{
		CData::init();
		this->node = node;
		this->time = this->timeBirth = timeBirth;
		this->size = byte;
		this->generateID();
	}

	~CData()
	{
	};

	static void deliverAtWaypoint(int n)
	{
		COUNT_DELIVERY_AT_WAYPOINT += n;
	}
	static void deliverOnRoute(int n)
	{
		COUNT_DELIVERY_ON_ROUTE += n;
	}
	//该函数应当在MA的路径更新时调用输出统计结果
	//注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于COUNT_ARRIVAL的，仅作测试用途
	static int getCountDeliveryAtWaypoint()
	{
		return COUNT_DELIVERY_AT_WAYPOINT;
	}
	//该函数应当在MA的路径更新时调用输出统计结果
	//注意：由于这个计数的统计发生在MA，因此这两个值的加和总是大于等于COUNT_ARRIVAL的，仅作测试用途
	static int getCountDeliveryTotal()
	{
		return COUNT_DELIVERY_AT_WAYPOINT + COUNT_DELIVERY_ON_ROUTE;
	}
	static double getPercentDeliveryAtWaypoint()
	{
		if(COUNT_DELIVERY_AT_WAYPOINT == 0)
			return 0.0;
		return double(COUNT_DELIVERY_AT_WAYPOINT) / double(COUNT_DELIVERY_AT_WAYPOINT + COUNT_DELIVERY_ON_ROUTE);
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

	static vector<CData> GetItemsByID(vector<CData> list, vector<int> ids);

	//重载比较操作符，比较生成时间，用于mergeSort
	friend bool operator < (const CData lt, const CData rt);
	friend bool operator > (const CData lt, const CData rt);
	//重载 == 操作符，根据 ID 判断 identical，用于去重
	friend bool operator == (const CData lt, const CData rt);
	friend bool operator == (int id, const CData data);
	friend bool operator == (const CData data, int id);

};


class CCtrl :
	virtual public CPacket
{
public:

	typedef enum EnumCtrlType
	{
		_rts,
		_cts,
		_ack,
		_capacity,  //允许接受的最大数据个数
		_index,   //data index ( delivery preds in Prophet / summary vetor in Epidemic )
		_no_data  //inform no data to send
	} EnumCtrlType;


private:

	EnumCtrlType type;
	int capacity;
	//	vector<int> sv;
	vector<CData> ack;  //直接传递 CData 类，方便操作，实际应传递 sv

	void init();

	CCtrl();


public:

	//RTS / CTS / NO_DATA
	CCtrl(int node, int timeBirth, int byte, EnumCtrlType type);
	//ACK
	CCtrl(int node, vector<CData> datas, int timeBirth, int byte, EnumCtrlType type);
	//capacity
	CCtrl(int node, int capacity, int timeBirth, int byte, EnumCtrlType type);
	////data index ( delivery preds )
	//CCtrl(int node, map<int, double> pred, int timeBirth, int byte, EnumCtrlType type);
	////data index ( summary vector )
	//CCtrl(int node, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type);
	~CCtrl()
	{
	};

	EnumCtrlType getType() const
	{
		return type;
	}

	int getCapacity() const
	{
		return capacity;
	}

	vector<CData> getACK() const
	{
		return ack;
	}

};


class CGeneralNode;

class CFrame :
	virtual public CGeneralData
{
private:

	CGeneralNode* src;
	CGeneralNode* dst;  //默认 null, 广播
	int headerMac;
	//注意：元素可能为空指针，这意味着任何引用之前需要判断
	vector<CPacket*> packets;

	void init();
	CFrame();


public:

	//单播包
	CFrame(CGeneralNode& src, CGeneralNode& dst, vector<CPacket*> packets);
	//广播包
	CFrame(CGeneralNode& src, vector<CPacket*> packets);

	~CFrame();

	inline CGeneralNode* getSrcNode() const
	{
		return src;
	}
	inline CGeneralNode* getDstNode() const
	{
		return dst;
	}
	inline vector<CPacket*> getPackets() const
	{
		return packets;
	}

	int getSize() const;

};


#endif // __DATA_H__
