/***********************************************************************************************************************************

���� CGeneralNode �� ���̳��� CEntity > CBasicEntity ����������ڵ���Ӧ�ü̳��������

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

	typedef enum EnumTransmitterState
	{
		_awake,  //�����շ�������״̬
		_asleep   //�����շ�������״̬
	} EnumTransmitterState;


protected:

	EnumTransmitterState state;
	vector<CData> buffer;
	int capacityBuffer;
	double energyConsumption;
	int timerOccupied;


public:

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
	virtual void consumeEnergy(double cons, int currentTime)
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
	//ע�⣺���м���������Ӧ���ڵ��ô˺����ж�֮����У����ô˺���֮ǰ����ȷ����updateStatus
	virtual bool isAwake() const
	{
		return state == _awake;
	}

	virtual void Overhear(int currentTime);
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
	//		timerOccupied = UNVALID;
	//}

//	virtual void receiveFrame(CFrame* frame, int currentTime);

//	virtual vector<CData> bufferData(int time, vector<CData> datas);


//	virtual vector<CData> sendAllData(_SEND mode);

//	virtual bool receiveData(int time, vector<CData> datas);

};

#endif // __GENERAL_NODE_H__
