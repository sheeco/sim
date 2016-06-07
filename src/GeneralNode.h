/***********************************************************************************************************************************

���� CGeneralNode �� ���̳��� CEntity > CBasicEntity ����������ڵ���Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __GENERAL_NODE_H__
#define __GENERAL_NODE_H__

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
		_copy,  //�������ݳɹ��󣬱���������
		_dump   //�������ݳɹ���ɾ��������
	} _SEND;

	typedef enum _RECEIVE {
		_loose,   //MA buffer����ʱ�������������������
		_selfish   //MA buffer����ʱ�����ٴ������ڵ��������
	} _RECEIVE;

	typedef enum _QUEUE {
		_fifo,   //�ɷ����������ʱ�����ȴ�ͷ������
		_lifo   //�ɷ����������ʱ�����ȴ�β������
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

#endif // __GENERAL_NODE_H__
