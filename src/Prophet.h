#pragma once

#include "RoutingProtocol.h"


class CProphet :
	public CRoutingProtocol
{
private:

//	//在特定时槽上发送数据
//	//注意：必须在调用UpdateNodeStatus之后调用此函数
//	static void SendData(int currentTime);


public:

	static bool TRANS_STRICT_BY_PRED;  //指示是否严格按照投递概率真大于，来决定是否转发
	static int MAX_DATA_TRANS;

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

	static bool Operate(int currentTime);

};

