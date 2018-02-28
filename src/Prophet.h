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


public:

	CProphet();
	~CProphet();

	static void initDeliveryPreds(CNode* node);
	static void decayDeliveryPreds(CNode* node, int now);
	static void updateDeliveryPredsWith(CNode* node, int fromNode, map<int, double> preds);
	static void updateDeliveryPredsWithSink(CNode* node, CSink* sink);

	static bool shouldForward(CNode* node, map<int, double> dstPred);

	//static vector<CData> getDataForTrans(CNode* node);

	static vector<CData> bufferData(CNode* node, vector<CData> datas, int time);

	//从下层协议传入的控制/数据包
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// Node <- Sink 
	static vector<CPacket*> receivePackets(CNode* node, CSink* sink, vector<CPacket*> packets, int time);
	// Sink <- Node 
	static vector<CPacket*> receivePackets(CSink* sink, CNode* fromNode, vector<CPacket*> packets, int time);
	// Node <- Node 
	static vector<CPacket*> receivePackets(CNode* node, CNode* fromNode, vector<CPacket*> packets, int time);

	static bool Init();
	static bool Operate(int now);

};

#endif // __PROPHET_H__
