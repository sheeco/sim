/***********************************************************************************************************************************

���� CGeneralNode �� ���̳��� CEntity > CBasicEntity ����������ڵ���Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#include "Data.h"


class CGeneralNode :
	public CBasicEntity
{
protected:

	vector<CData> buffer;
	int bufferCapacity;
	int energy;
	double energyConsumption;

	static double CONSUMPTION_BYTE_SEND;
	static double CONSUMPTION_BYTE_RECIEVE;
	static double CONSUMPTION_LISTEN;
	static double CONSUMPTION_SLEEP;


public:

	typedef enum _SEND {
		_copy,  //��������ʱ������������
		_dump   //��������ʱ��ɾ��������
	} _SEND;

	typedef enum _RECEIVE {
		_loose,   //MA buffer����ʱ�������������������
		_selfish   //MA buffer����ʱ�����ٴ������ڵ��������
	} _RECEIVE;

	typedef enum _QUEUE {
		_fifo,   //�ɷ����������ʱ�����ȴ�ͷ������
		_lifo   //�ɷ����������ʱ�����ȴ�β������
	} _QUEUE;

	static int TRANS_RANGE;  //transmission range
	static double PROB_DATA_FORWARD;

	CGeneralNode()
	{
		this->bufferCapacity = 0;
		this->energy = 0;
		this->energyConsumption = 0;
	}

	virtual ~CGeneralNode(){};

	inline double getEnergyConsumption() const
	{
		return energyConsumption;
	}
	inline bool hasData() const
	{
		return ( ! buffer.empty() );
	}
	inline int getBufferSize() const
	{
		return buffer.size();
	}


	virtual vector<CData> sendAllData(_SEND mode) = 0;

	virtual bool receiveData(int time, vector<CData> datas) = 0;

};

