#pragma once

#include "Data.h"

using namespace std;

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

	~CGeneralNode(void){};

	inline double getEnergyConsumption()
	{
		return energyConsumption;
	}
	inline bool hasData()
	{
		return ( ! buffer.empty() );
	}
	inline int getBufferSize()
	{
		return buffer.size();
	}
};

