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

	static map<int, map<int, CDeliveryPredRecord>> deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���

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

	//���²�Э�鴫��Ŀ���/���ݰ�
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
