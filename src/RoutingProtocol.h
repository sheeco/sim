/***********************************************************************************************************************************

���� CRoutingProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������·��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __ROUTING_PROTOCOL_H__
#define __ROUTING_PROTOCOL_H__

#include "Protocol.h"
#include "GeneralNode.h"


class CRoutingProtocol :
	virtual public CProtocol
{
protected:


public:

	//static int SLOT_DATA_SEND;  //���ݷ���slot

	CRoutingProtocol();
	virtual ~CRoutingProtocol() = 0
	{};

	static int WINDOW_TRANS;
	static int TIME_WINDOW_TRANS;

	static int getTimeWindowTrans();
	//���������� FIFO/FILO�����غ��ʵ������������ݴ���
	//���صĶ��в��ᳬ�����䴰�ڴ�С�����c apacity Ϊ 0 ��Ĭ�����޼����ڴ�С
	static vector<CData> getDataForTrans(CGeneralNode* node, int capacity, bool FIFO);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	//����final���
	static void PrintFinal(int currentTime);

};

#endif // __ROUTING_PROTOCOL_H__
