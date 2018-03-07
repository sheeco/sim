#pragma once

#ifndef __POSITION_H__
#define __POSITION_H__

#include "BasicEntity.h"
#include "GeoEntity.h"


//存储单个节点移动位置的类
class CPosition : 
	virtual public CBasicEntity, public CGeoEntity
{
private:

	int node;  //所属节点ID
	//bool isCovered;
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
		this->setLocation(location, time);
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
	inline void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}

};

#endif // __POSITION_H__
