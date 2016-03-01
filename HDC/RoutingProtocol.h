/***********************************************************************************************************************************

基类 CRoutingProtocol ： （继承自 CProcess > CAlgorithm > CProtocol ）所有路由协议算法的实现类，都应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#include "Protocol.h"

using namespace std;


class CRoutingProtocol :
	public CProtocol
{
protected:

	//在限定范围内随机增删一定数量的node，并删除被抛弃的node的position记录，不再用于热点选取
	static void ChangeNodeNumber(int currentTime);
	//更新所有node的坐标、占空比和工作状态
	static void UpdateNodeStatus(int currentTime);


public:

	CRoutingProtocol(){};
	~CRoutingProtocol(){};

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

