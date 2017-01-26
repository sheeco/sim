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
//	static void SendData(int currentTime);


public:

	static bool TRANS_STRICT_BY_PRED;  //指示是否严格按照投递概率真大于，来决定是否转发

	static double INIT_PRED;
	static double RATIO_PRED_DECAY;
	static double RATIO_PRED_TRANS;


	CProphet();
	~CProphet();

	static void initDeliveryPreds(CNode* node);
	static void decayDeliveryPreds(CNode* node, int currentTime);
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

	static bool Operate(int currentTime);

};

#endif // __PROPHET_H__
