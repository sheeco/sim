#pragma once

#include "BasicEntity.h"
#include "GeoEntity.h"

extern double CO_POSITION_DECAY;

using namespace std;

//存储单个节点移动位置的类
class CPosition : 
	public CGeoEntity
{
private:
	int node;  //所属节点ID
	//bool isCovered;
	double weight;
	static int ID_COUNT;

public:

	//以下公有静态变量是从原来的g_系列全局变量移动到此处的，所有原来的引用都已作出替换
	static vector<CPosition *> positions;
	static int nPositions;
	//保存从候选position集合中删除的position，测试用
	static vector<CPosition *> deletedPositions;

	CPosition()
	{
		node = -1;
		//isCovered = false;
		weight = 1;
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
	inline double getWeight()
	{
		return weight;
	}
	inline void decayWeight()
	{
		this->weight /= CO_POSITION_DECAY;
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

