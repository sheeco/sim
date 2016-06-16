#pragma once

#ifndef __PROPHET_H__
#define __PROPHET_H__

#include "RoutingProtocol.h"
#include "Sink.h"


class CProphet :
	public CRoutingProtocol
{
private:

//	//���ض�ʱ���Ϸ�������
//	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
//	static void SendData(int currentTime);


public:

	static bool TRANS_STRICT_BY_PRED;  //ָʾ�Ƿ��ϸ���Ͷ�ݸ�������ڣ��������Ƿ�ת��
	static int CAPACITY_FORWARD;

	static double INIT_PRED;
	static double RATIO_PRED_DECAY;
	static double RATIO_PRED_TRANS;


	CProphet();
	~CProphet();

	static bool shouldForward(CNode* node, map<int, double> dstPred);

	// TODO: check hop
	//select proper data to send
	static vector<CData> getDataForTrans(CNode* node);

	static vector<CData> bufferData(CNode* node, vector<CData> datas, int time);

	//���²�Э�鴫��Ŀ���/���ݰ�
	// Node <- Sink 
	static vector<CGeneralData*> receiveContents(CNode* node, CSink* sink, vector<CGeneralData*> contents, int time);
	// Sink <- Node 
	static vector<CGeneralData*> receiveContents(CSink* sink, CNode* fromNode, vector<CGeneralData*> contents, int time);
	// Node <- Node 
	static vector<CGeneralData*> receiveContents(CNode* node, CNode* fromNode, vector<CGeneralData*> contents, int time);

	static bool Operate(int currentTime);

};

#endif // __PROPHET_H__
