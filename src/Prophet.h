#pragma once

#include "RoutingProtocol.h"


class CProphet :
	public CRoutingProtocol
{
private:

//	//���ض�ʱ���Ϸ�������
//	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
//	static void SendData(int currentTime);


public:

	static bool TRANS_STRICT_BY_PRED;  //ָʾ�Ƿ��ϸ���Ͷ�ݸ�������ڣ��������Ƿ�ת��
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

