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

	static int MAX_DATA_TRANS;

#ifdef USE_PRED_TOLERANCE

	static double TOLERANCE_PRED;
//	static double DECAY_TOLERANCE_PRED;

#endif


	CProphet();
	~CProphet();

	// TODO: check hop
	//select data to sent based on preds
	static vector<CData> selectDataByIndex(CNode* node, CCtrl* ctrl);

	static vector<CData> bufferData(CNode* node, vector<CData> datas, int time);

	static bool Operate(int currentTime);

};

