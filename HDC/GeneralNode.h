#pragma once

#include "Data.h"

using namespace std;

extern double PROB_DATA_FORWARD;

class CGeneralNode :
	public CBasicEntity
{
protected:
	vector<CData> buffer;
	int bufferCapacity;
	double energy;
	double energyConsumption;


public:
	CGeneralNode(void)
	{
		this->bufferCapacity = 0;
		this->energy = 0;
		this->energyConsumption = 0;
	}

	virtual ~CGeneralNode(void){};

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


	virtual vector<CData> sendAllData(Mode mode) = 0
	{
		double bet = RandomFloat(0, 1);
		if( bet > PROB_DATA_FORWARD )
		{
			energyConsumption += buffer.size() * BYTE_PER_DATA * CONSUMPTION_BYTE_SEND;
			return vector<CData>();
		}

		if(buffer.empty())
			return vector<CData>();
		else
		{
			vector<CData> data = buffer;
			energyConsumption += buffer.size() * BYTE_PER_DATA * CONSUMPTION_BYTE_SEND;
			if(mode == SEND::DUMP)
				buffer.clear();
			return data;
		}
	}

	virtual bool receiveData(int time, vector<CData> datas) = 0;
};

