/***********************************************************************************************************************************

基类 CRoutingProtocol ： （继承自 CProcess > CAlgorithm > CProtocol ）所有路由协议算法的实现类，都应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __ROUTING_PROTOCOL_H__
#define __ROUTING_PROTOCOL_H__

#include "Protocol.h"


class CRoutingProtocol :
	virtual public CProtocol
{
protected:


public:

	//static int SLOT_DATA_SEND;  //数据发送slot

	CRoutingProtocol();
	virtual ~CRoutingProtocol() = 0
	{};

	static int WINDOW_TRANS;
	static int TIME_WINDOW_TRANS;

	static int getTimeWindowTrans();

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	//最终final输出
	static void PrintFinal(int currentTime);

};

#endif // __ROUTING_PROTOCOL_H__
