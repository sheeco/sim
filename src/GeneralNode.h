/***********************************************************************************************************************************

基类 CGeneralNode ： （继承自 CEntity > CBasicEntity ）所有网络节点类应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __GENERAL_NODE_H__
#define __GENERAL_NODE_H__

#include "Data.h"
//#include "Frame.h"


class CFrame;

class CGeneralNode :
	virtual public CBasicEntity
{
public:

	typedef enum _STATE
	{
		_awake,  //无线收发器唤醒状态
		_asleep   //无线收发器休眠状态
	} _STATE;

	typedef enum _SEND
	{
		_copy,  //发送数据成功后，保留自身副本
		_dump   //发送数据成功后，删除自身副本
	} _SEND;

	typedef enum _RECEIVE
	{
		_loose,   //MA buffer已满时，仍允许继续接收数据
		_selfish   //MA buffer已满时，不再从其他节点接收数据
	} _RECEIVE;

	typedef enum _QUEUE
	{
		_fifo,   //可发送配额有限时，优先从头部发送
		_lifo   //可发送配额有限时，优先从尾部发送
	} _QUEUE;


protected:

	_STATE state;
	vector<CData> buffer;
	int capacityBuffer;
	double energyConsumption;
	int timerOccupied;

	static double CONSUMPTION_WAKE;
	static double CONSUMPTION_SLEEP;


public:

	static double CONSUMPTION_BYTE_SEND;
	static double CONSUMPTION_BYTE_RECEIVE;
	static int RANGE_TRANS;  //transmission range
	static double PROB_TRANS;
	static int SIZE_CTRL;

	CGeneralNode()
	{
		this->capacityBuffer = 0;
		this->energyConsumption = 0;
		this->timerOccupied = UNVALID;
		state = _awake;
	}

	virtual ~CGeneralNode() = 0
	{};

	inline double getEnergyConsumption() const
	{
		return energyConsumption;
	}
	inline void consumeEnergy(double cons)
	{
		this->energyConsumption += cons;
	}
	inline bool hasData() const
	{
		return ( ! this->buffer.empty() );
	}
	inline int getSizeBuffer() const
	{
		return this->buffer.size();
	}
	inline vector<CData> getAllData() const
	{
		return this->buffer;
	}

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

	virtual void Overhear();
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
	//		timerOccupied = UNVALID;
	//}

//	virtual void receiveFrame(CFrame* frame, int currentTime);

//	virtual vector<CData> bufferData(int time, vector<CData> datas);


//	virtual vector<CData> sendAllData(_SEND mode);

//	virtual bool receiveData(int time, vector<CData> datas);

};

#endif // __GENERAL_NODE_H__
