#pragma once

#ifndef __POSITION_H__
#define __POSITION_H__

#include "Entity.h"


//存储节点地理位置记录的类
class CPosition : 
	virtual public CBasicEntity, virtual public CUnique, public CGeoEntity
{
private:

	int node;  //所属节点ID
	static int COUNT_ID;


public:

	void init()
	{
		node = INVALID;
	}
	CPosition()
	{
		init();
	}

	CPosition(int node, CCoordinate location, int time)
	{
		init();
		this->setNode(node);
		this->setLocation(location);
		this->setTime(time);
	}

	inline int getNode() const
	{
		return node;
	}
	inline void setNode(int node)
	{
		this->node = node;
	}
	//自动生成ID，需手动调用
	inline void generateID() override
	{
		CUnique::generateID(COUNT_ID);
	}


	string toString() override
	{
		return this->location.toString();
	};

};

#endif // __POSITION_H__
