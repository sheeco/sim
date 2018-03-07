#pragma once

#ifndef __POSITION_H__
#define __POSITION_H__

#include "BasicEntity.h"
#include "GeoEntity.h"


//�洢�����ڵ��ƶ�λ�õ���
class CPosition : 
	virtual public CBasicEntity, public CGeoEntity
{
private:

	int node;  //�����ڵ�ID
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
	//�Զ�����ID�����ֶ�����
	inline void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}

};

#endif // __POSITION_H__
