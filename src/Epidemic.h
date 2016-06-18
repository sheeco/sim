//#pragma once
//
//#include "RoutingProtocol.h"
//
//#ifndef __EPIDEMIC_H__
//#define __EPIDEMIC_H__
//
//
//// TODO: rewrite
//class CEpidemic :
//	virtual public CRoutingProtocol
//{
//private:
//
////	//在特定时槽上发送数据
////	//注意：必须在调用UpdateNodeStatus之后调用此函数
////	static void SendData(int currentTime);
//
//
//public:
//
//	static int MAX_DATA_RELAY;  //同意存储的来自其他节点的data的最大总数，超过该数目将丢弃（是否在Request之前检查？）默认值等于buffer容量
//
//	CEpidemic(){};
//
//	~CEpidemic(){};
//
//	static bool Operate(int currentTime);
//
//};
//
//#endif // __EPIDEMIC_H__
