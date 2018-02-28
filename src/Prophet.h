#pragma once

#ifndef __PROPHET_H__
#define __PROPHET_H__

#include "RoutingProtocol.h"
#include "Sink.h"


class CProphet :
	virtual public CRoutingProtocol
{
private:

//	//���ض�ʱ���Ϸ�������
//	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
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

	//���²�Э�鴫��Ŀ���/���ݰ�
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
