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
//	inline void setNode(int node)
//	{
//		this->node = node;
//	}
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

	//该数据被转发到达新的节点后应该调用的函数，将更新跳数或TTL剩余值，并更新时间戳
	//注意：数据发送方应在发送之前检查剩余HOP大于1
	virtual inline void arriveAnotherNode(int currentTime)
	{};

	virtual inline bool allowForward() const
	{
		return true;
	};

};

