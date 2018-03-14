/***********************************************************************************************************************************

���� CRoutingProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������·��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __ROUTING_PROTOCOL_H__
#define __ROUTING_PROTOCOL_H__

#include "Process.h"
#include "Node.h"


class CRoutingProtocol :
	virtual public CProtocol
{
protected:


public:

	//static int SLOT_DATA_SEND;  //���ݷ���slot

	CRoutingProtocol();
	virtual ~CRoutingProtocol() = 0
	{};

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int now);
	static void PrintInfo(vector<CNode*> allNodes, int now);
	//����final���
	static void PrintFinal(int now);

};

#endif // __ROUTING_PROTOCOL_H__
