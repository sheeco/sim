#pragma once

#include "BasicEntity.h"


class CGeneralData :
	public CBasicEntity
{
//protected:

//	int ID;  //data编号
//	CCoordinate location;  //未使用
//	int time;  //该data最后一次状态更新的时间戳，用于校验，初始值应等于timeBirth
//	bool flag;


protected:

	int node;  //所属node
	int timeBirth;  //生成时间
	int size;  //byte
	int HOP;

	virtual void init();


public:

	CGeneralData();
	virtual ~CGeneralData();

	//setters & getters
	inline void setNode(int node)
	{
		this->node = node;
	}
	inline int getNode() const
	{
		return node;
	}
	inline int getTimeBirth() const
	{
		return timeBirth;
	}
	inline int getSize() const
	{
		return size;
	}

//	inline bool allowForward() const
//	{
//		return HOP > 0;
//	}

};

