/***********************************************************************************************************************************

基类 CGeneralNode ： （继承自 CEntity > CBasicEntity ）所有网络节点类应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#include "Data.h"
//#include "Package.h"


class CPackage;

class CGeneralNode :
	public CBasicEntity
{
protected:

	vector<CData> buffer;
	int capacityBuffer;
	double energyConsumption;

	static double CONSUMPTION_LISTEN;
	static double CONSUMPTION_SLEEP;


public:

	typedef enum _SEND {
		_copy,  //发送数据时，保留自身副本
		_dump   //发送数据时，删除自身副本
	} _SEND;

	typedef enum _RECEIVE {
		_loose,   //MA buffer已满时，仍允许继续接收数据
		_selfish   //MA buffer已满时，不再从其他节点接收数据
	} _RECEIVE;

	typedef enum _QUEUE {
		_fifo,   //可发送配额有限时，优先从头部发送
		_lifo   //可发送配额有限时，优先从尾部发送
	} _QUEUE;

	static double CONSUMPTION_BYTE_SEND;
	static double CONSUMPTION_BYTE_RECEIVE;
	static int RANGE_TRANS;  //transmission range
	static double PROB_TRANS;
	static int SIZE_CTRL;

	CGeneralNode()
	{
		this->capacityBuffer = 0;
		this->energyConsumption = 0;
	}

	virtual ~CGeneralNode(){};

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

	virtual bool isListening() const
	{
		return true;
	}

//	virtual void receivePackage(CPackage* package, int currentTime);

//	virtual vector<CData> bufferData(int time, vector<CData> datas);


//	virtual vector<CData> sendAllData(_SEND mode);

//	virtual bool receiveData(int time, vector<CData> datas);

};

