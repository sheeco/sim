#pragma once

#ifndef __NODE_H__
#define __NODE_H__

#include "DutyCycle.h"


class CNode :
	public CDutyCycle
{
private:

	static vector<int> idNodes;  //用于储存所有传感器节点的ID，便于处理
								 //待测试
	//待测试
	static void newNodes(int n);
	static void removeNodes(int n);


protected:

	static vector<CNode &> nodes;  //用于储存所有传感器节点
	static vector<CNode &> deadNodes;  //能量耗尽的节点
	static vector<CNode &> deletedNodes;  //用于暂存Node个数动态变化时被暂时移出的节点

	static void pushBackNode(CNode& node);
	static vector<CNode&>::iterator eraseNode(vector<CNode&>::iterator inode);


public:

	CNode();
	CNode(double dataRate) : CBasicNode(dataRate)
	{};
	virtual ~CNode();


	static bool checkInitialization();
	static bool assertInitialization();

	static vector<CNode &>& getNodes();

	static int getNNodes();

	//包括已经失效的节点和删除的节点，按照ID排序
	static vector<CNode &> getAllNodes(bool sort);

	static vector<int>& getIdNodes();

	//该节点不存在时无返回值所以将出错，所以必须在每次调用之前调用函数ifNodeExists()进行检查
	static CNode& getNodeByID(int id);

	static bool ifNodeExists(int id);

	static bool hasNodes(int currentTime);

	//将死亡节点整理移出
	static void ClearDeadNodes(int currentTime);

	//在限定范围内随机增删一定数量的node
	static int ChangeNodeNumber();

	static double getSumEnergyConsumption();


};

#endif // __NODE_H__
