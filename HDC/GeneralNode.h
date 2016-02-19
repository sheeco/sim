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
	int energy;
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


	virtual vector<CData> sendAllData(Mode mode) = 0;

	virtual bool receiveData(int time, vector<CData> datas) = 0;
};

