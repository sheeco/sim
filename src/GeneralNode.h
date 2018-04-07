/***********************************************************************************************************************************

���� CGeneralNode �� ���̳��� CEntity > CBasicEntity ����������ڵ���Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __GENERAL_NODE_H__
#define __GENERAL_NODE_H__

#include "Data.h"


class CFrame;

class CGeneralNode :
	virtual public CBasicEntity, virtual public CUnique, virtual public CSpatial
{
protected:
	string name;
	bool fifo;

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

	string toString() override
	{
		return this->getName();
	}

	typedef enum EnumTransmitterState
	{
		_awake,  //�����շ�������״̬
		_asleep   //�����շ�������״̬
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
	//ע�⣺���м���������Ӧ���ڵ��ô˺����ж�֮����У����ô˺���֮ǰ����ȷ����updateStatus
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
	//	//���� timerOccupied ������
	//	if( timerOccupied < 0 )
	//		timerOccupied = INVALID;
	//}

	/*************************** Buffer ***************************/

	bool isFull()
	{
		return buffer.size() >= capacityBuffer;
	}

	//�ֶ�������ѹ�� buffer�������������κβ��������������ݵ���Ŀ
	//ע�⣺�����ڵ��ô˺���֮���ֶ����� dropDataIfOverflow() ������
	int pushIntoBuffer(vector<CData> datas, int now);

	//TODO: add queue managing method as arg
	//���ʱ������fifoԭ�������ݲ��������������
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

	//�������������սڵ�� FIFO/FILO ���ԣ�ѡ�����ʵ������������ݴ���
	//���صĶ��в��ᳬ�����䴰�ڴ�С�����capacity Ϊ -1 ��Ĭ�����޼����ڴ�С
	vector<CData> getDataForTrans(int capacity);

	vector<CData> bufferData(int now, vector<CData> datas)
	{
		vector<CData> ack = datas;

		this->pushIntoBuffer(datas, now);
		vector<CData> overflow = this->dropDataIfOverflow();
		RemoveFromList(ack, overflow);

		return ack;
	}

public:

	//���ո����������ü������б����ر��Ƴ�������,FIFO��ζ�Ŵ���࿪ʼ�Ƴ�
	//ע�⣺����֮ǰӦ��ȷ������������
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
