#pragma once

#ifndef __POSITION_H__
#define __POSITION_H__

#include "Entity.h"


//�洢�����ڵ��ƶ�λ�õ���
class CPosition : 
	virtual public CBasicEntity, virtual public CUnique, public CGeoEntity
{
private:

	int node;  //�����ڵ�ID
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
	//�Զ�����ID�����ֶ�����
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
