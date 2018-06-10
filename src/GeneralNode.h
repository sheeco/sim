/***********************************************************************************************************************************

基类 CGeneralNode ： （继承自 CEntity > CBasicEntity ）所有网络节点类应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __GENERAL_NODE_H__
#define __GENERAL_NODE_H__

#include "Data.h"


class CFrame;

//所有类型节点(sink, sensor node, MA)的通用基类
class CGeneralNode :
	virtual public CBasicEntity, virtual public CUnique, virtual public CSpatial
{
protected:
	string name;
	bool fifo;

	vector<CData> bufferHistory;


public:

	void setName(string name)
	{
		this->name = name;
	}

	string getName() const
	{
		return this->name;
	}

	bool getFifo() const
	{
		return fifo;
	}

	vector<CData> getBufferHistory() const
	{
		return this->bufferHistory;
	}

	string toString() override
	{
		return this->getName();
	}

	typedef enum EnumTransmitterState
	{
		_awake,  //无线收发器唤醒状态
		_asleep   //无线收发器休眠状态
	} EnumTransmitterState;


protected:

	EnumTransmitterState state;
	vector<CData> buffer;
	int capacityBuffer;
	int capacityEnergy;
	double energyConsumption;
	int timerOccupied;


public:

	CGeneralNode()
	{
		this->capacityBuffer = 0;
		this->capacityEnergy = 0;
		this->energyConsumption = 0;
		this->timerOccupied = INVALID;
		state = _awake;
		fifo = true;
	}

	virtual ~CGeneralNode() = 0
	{};

	inline bool finiteEnergy() const
	{
		return this->capacityEnergy > 0;
	}
	inline bool hasEnergyLeft() const
	{
		return !finiteEnergy() || ( this->getEnergyLeft() > 0 );
	}
	double getEnergyLeft() const
	{
		return this->capacityEnergy - energyConsumption;
	}
	inline void setCapacityEnergy(int energy)
	{
		this->capacityEnergy = energy;
	}
	inline int getCapacityEnergy() const
	{
		return capacityEnergy;
	}
	inline double getEnergyConsumption() const
	{
		return energyConsumption;
	}
	virtual void consumeEnergy(double cons, int now)
	{
		this->energyConsumption += cons;
	}
	inline bool hasData() const
	{
		return ( ! this->buffer.empty() );
	}
	inline int getCapacityBuffer() const
	{
		return this->capacityBuffer;
	}
	inline int getBufferSize() const
	{
		return this->buffer.size();
	}
	inline int getBufferVacancy() const
	{
		return this->capacityBuffer - this->getBufferSize();
	}
	inline vector<CData> getAllData() const
	{
		return this->buffer;
	}

	/*************************** Communication ***************************/

	CFrame* sendRTS(int now);

	/*************************** DC ***************************/

	virtual inline void Wake()
	{
		state = _awake;
	}
	virtual inline void Sleep()
	{
		state = _asleep;
	}
	//注意：所有监听动作都应该在调用此函数判断之后进行，调用此函数之前必须确保已updateStatus
	virtual bool isAwake() const
	{
		return state == _awake;
	}

	virtual void Overhear(int now);
	//virtual void Occupy(int time)
	//{
	//	if( time <= 0 )
	//		return;
	//	this->timerOccupied = time;
	//}
	//bool isOccupied()
	//{
	//	return timerOccupied > 0;
	//}

	//void updateTimerOccupied(int time)
	//{
	//	if( timerOccupied > 0 )
	//		timerOccupied -= time - this->time;
	//	//保留 timerOccupied 的下沿
	//	if( timerOccupied < 0 )
	//		timerOccupied = INVALID;
	//}

	/*************************** Buffer ***************************/

	bool isFull()
	{
		return buffer.size() >= capacityBuffer;
	}

	//手动将数据压入 buffer，不伴随其他任何操作，返回新数据的数目
	//注意：必须在调用此函数之后手动调用 dropDataIfOverflow() 检查溢出
	int pushIntoBuffer(vector<CData> datas, int now);

	//TODO: add queue managing method as arg
	//溢出时将按照fifo原则丢弃数据并返回溢出的数据
	vector<CData> dropDataIfOverflow()
	{
		if( buffer.empty() )
			return vector<CData>();

		vector<CData> myData;
		vector<CData> overflow;

		myData = buffer;
		//myData = CSortHelper::mergeSort(myData, CSortHelper::ascendByTimeBirth);
		overflow = clipDataByCapacity(myData, capacityBuffer, this->fifo);

		buffer = myData;
		return overflow;
	}

	//给定容量，按照节点的 FIFO/FILO 策略，选出合适的数据用于数据传输
	//返回的队列不会超过传输窗口大小，如果capacity 为 -1 即默认上限即窗口大小
	vector<CData> getDataForTrans(int capacity);

	//选择对方没有的数据用于发送
	vector<CData> getDataForTrans(vector<CData> except, int capacity);

	vector<CData> bufferData(int now, vector<CData> datas)
	{
		vector<CData> ack = datas;

		this->pushIntoBuffer(datas, now);
		vector<CData> overflow = this->dropDataIfOverflow();
		RemoveFromList(ack, overflow);
		AddToListUniquely(this->bufferHistory, ack);

		return ack;
	}

public:

	//按照给定的容量裁剪数据列表，返回被移除的数据,FIFO意味着从左侧开始移除
	//注意：调用之前应该确保数据已排序
	static vector<CData> clipDataByCapacity(vector<CData> &datas, int capacity, bool fifo)
	{
		vector<CData> overflow;
		if( capacity <= 0
			|| datas.size() <= capacity )
			return overflow;

		if( fifo )
		{
			overflow = vector<CData>(datas.begin(), datas.begin() + capacity);
			datas = vector<CData>(datas.begin() + capacity, datas.end());
		}
		else
		{
			datas = vector<CData>(datas.begin(), datas.begin() + capacity);
			overflow = vector<CData>(datas.begin() + capacity, datas.end());
		}

		return overflow;
	}

};

#endif // __GENERAL_NODE_H__
