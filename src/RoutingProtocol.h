/***********************************************************************************************************************************

基类 CRoutingProtocol ： （继承自 CProcess > CAlgorithm > CProtocol ）所有路由协议算法的实现类，都应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __ROUTING_PROTOCOL_H__
#define __ROUTING_PROTOCOL_H__

#include "Protocol.h"
#include "Node.h"


class CRoutingProtocol :
	virtual public CProtocol
{
protected:


public:

	//static int SLOT_DATA_SEND;  //数据发送slot

	CRoutingProtocol();
	virtual ~CRoutingProtocol() = 0
	{};

	//给定容量和 FIFO/FILO，返回合适的数据用于数据传输
	//返回的队列不会超过传输窗口大小，如果c apacity 为 0 即默认上限即窗口大小
	static vector<CData> getDataForTrans(CGeneralNode* node, int capacity, bool FIFO);

	//打印相关信息到文件
	static void PrintInfo(int now);
	static void PrintInfo(vector<CNode*> allNodes, int now);
	//最终final输出
	static void PrintFinal(int now);

};

#endif // __ROUTING_PROTOCOL_H__
