#pragma once

#include "Base.h"

//�洢�����ڵ��ƶ�λ�õ���
class CPosition: public CBase
{
private:
	int node;  //�����ڵ�ID
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
	//�Զ�����ID�����ֶ�����
	inline void generateID()
	{
		if(this->ID != -1)
			return;
		this->ID = ID_COUNT;
		ID_COUNT++;
	}
};

