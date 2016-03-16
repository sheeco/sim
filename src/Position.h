#pragma once

#include "BasicEntity.h"
#include "GeoEntity.h"


//存储单个节点移动位置的类
class CPosition : 
	public CBasicEntity, public CGeoEntity
{
private:

	int node;  //所属节点ID
	//bool isCovered;
	double weight;
	static int ID_COUNT;


public:

//	static double CO_POSITION_DECAY;

	//以下公有静态变量是从原来的g_系列全局变量移动到此处的，所有原来的引用都已作出替换
	static vector<CPosition *> positions;
	static int nPositions;
//	//保存从候选position集合中删除的position，测试用
//	static vector<CPosition *> deletedPositions;

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

	inline int getNode() const
	{
		return node;
	}
	inline void setNode(int node)
	{
		this->node = node;
	}
	inline double getWeight() const
	{
		return weight;
	}
//	inline void decayWeight()
//	{
//		this->weight /= CO_POSITION_DECAY;
//	}
	//自动生成ID，需手动调用
	inline void generateID()
	{
		ID_COUNT++;
		this->ID = ID_COUNT;
	}


	//根据position指针得到它在全局变量CPosition::positions中的下标，仅在热点候选集合构建过程中使用
	//应在CPosition::positions完成排序之后调用，并且此后不能再改变其中的元素顺序
	static int getIndexOfPosition(CPosition* pos);

};

