/***********************************************************************************************************************************

基类 CGeneralNode ： （继承自 CEntity > CBasicEntity ）所有网络节点类应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#include "Data.h"

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


	virtual vector<CData> sendAllData(Mode mode) = 0;

	virtual bool receiveData(int time, vector<CData> datas) = 0;

};

