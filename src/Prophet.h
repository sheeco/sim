#pragma once

#ifndef __PROPHET_H__
#define __PROPHET_H__

#include "RoutingProtocol.h"
#include "Sink.h"


class CProphet :
	virtual public CRoutingProtocol
{
private:

//	//在特定时槽上发送数据
//	//注意：必须在调用UpdateNodeStatus之后调用此函数
//	static void SendData(int now);

	typedef struct CDeliveryPredRecord
	{
		double pred = 0;
		int timestamp = INVALID;

		CDeliveryPredRecord(){};

		CDeliveryPredRecord(double pred, int time) :
			pred(pred), timestamp(time)
		{
		};
	} CDeliveryPredRecord;

	static map<int, map<int, CDeliveryPredRecord>> deliveryPreds;  //< ID:x, P(this->id, x) >，sink节点ID为0将位于最前，便于查找

	static double INIT_PRED;
	static double DECAY_PRED;
	static double TRANS_PRED;
	static bool TRANS_STRICT_BY_PRED;


public:

	CProphet();
	~CProphet();

	static void initDeliveryPreds(int now);
	
	static void decayDeliveryPreds(int forNode, int withNode, int now);
	static void DecayDeliveryPreds(CNode* node, int now);
	//TODO: test
	static void updateDeliveryPredsBetween(int a, int b, int now);

	static void updateDeliveryPredsWithSink(int node, int now);

	static bool shouldForward(int me, int you, int now);

	//从下层协议传入的控制/数据包
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// Node <- Sink 
	static vector<CPacket*> receivePackets(CNode* node, CSink* sink, vector<CPacket*> packets, int time);
	// Sink <- Node 
	static vector<CPacket*> receivePackets(CSink* sink, CNode* fromNode, vector<CPacket*> packets, int time);
	// Node <- Node 
	static vector<CPacket*> receivePackets(CNode* node, CNode* fromNode, vector<CPacket*> packets, int time);

	static void CommunicateBetweenNeighbors(int now);

	static bool Init(int now);
	static bool Operate(int now);

};

#endif // __PROPHET_H__
