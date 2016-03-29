#pragma once

#include "RoutingProtocol.h"
#include "Sink.h"


class CProphet :
	public CRoutingProtocol
{
private:

//	//在特定时槽上发送数据
//	//注意：必须在调用UpdateNodeStatus之后调用此函数
//	static void SendData(int currentTime);


public:

	static bool TRANS_STRICT_BY_PRED;  //指示是否严格按照投递概率真大于，来决定是否转发
	static int CAPACITY_FORWARD;

#ifdef USE_PRED_TOLERANCE

	static double TOLERANCE_PRED;
//	static double DECAY_TOLERANCE_PRED;

#endif


	CProphet();
	~CProphet();

	static bool shouldForward(CNode* node, map<int, double> dstPred);

	// TODO: check hop
	//select proper data to send
	static vector<CData> getDataForTrans(CNode* node);

	static vector<CData> bufferData(CNode* node, vector<CData> datas, int time);

	//从下层协议传入的控制/数据包
	// Node <- Sink 
	static vector<CGeneralData*> receiveContents(CNode* node, CSink* sink, vector<CGeneralData*> contents, int time);
	// Sink <- Node 
	static vector<CGeneralData*> receiveContents(CSink* sink, CNode* fromNode, vector<CGeneralData*> contents, int time);
	// Node <- Node 
	static vector<CGeneralData*> receiveContents(CNode* node, CNode* fromNode, vector<CGeneralData*> contents, int time);

	static bool Operate(int currentTime);

};

