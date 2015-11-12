#pragma once

#include "Base.h"

//存储单个节点移动位置的类
class CPosition: public CBase
{
private:
	int node;  //所属节点ID
	//bool isCovered;
	static long int ID_COUNT;

public:
	CPosition()
	{
		node = -1;
		//isCovered = false;
	}

	//setters & getters
	//inline void setIsCovered(bool isCovered)
	//{
	//	this->isCovered = isCovered;
	//}
	//inline bool IfIsCovered()
	//{
	//	return isCovered;
	//}

	inline int getNode()
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
		//if(this->ID != -1)
		//	return;
		this->ID = ID_COUNT;
		ID_COUNT++;
	}
};

