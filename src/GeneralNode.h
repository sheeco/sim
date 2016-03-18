/***********************************************************************************************************************************

���� CGeneralNode �� ���̳��� CEntity > CBasicEntity ����������ڵ���Ӧ�ü̳��������

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
	int bufferCapacity;
	int energy;
	double energyConsumption;

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

	static double CONSUMPTION_BYTE_SEND;
	static double CONSUMPTION_BYTE_RECIEVE;
	static int TRANS_RANGE;  //transmission range
	static double PROB_DATA_FORWARD;
	static int CTRL_SIZE;

	CGeneralNode()
	{
		this->bufferCapacity = 0;
		this->energy = 0;
		this->energyConsumption = 0;
	}

	virtual ~CGeneralNode(){};

	double getEnergyConsumption() const
	{
		return energyConsumption;
	}
	void consumeEnergy(double cons)
	{
		this->energyConsumption += cons;
	}
	bool hasData() const
	{
		return ( ! buffer.empty() );
	}
	int getBufferSize() const
	{
		return buffer.size();
	}
	vector<CData> getAllData() const
	{
		return buffer;
	}

	virtual bool isListening() const
	{
		return true;
	}

	CPackage* sendRTS(int currentTime);

//	virtual void receivePackage(CPackage* package, int currentTime);

//	virtual vector<CData> bufferData(int time, vector<CData> datas);


//	virtual vector<CData> sendAllData(_SEND mode);

//	virtual bool receiveData(int time, vector<CData> datas);

};

