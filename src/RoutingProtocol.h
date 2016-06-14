/***********************************************************************************************************************************

���� CRoutingProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������·��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __ROUTING_PROTOCOL_H__
#define __ROUTING_PROTOCOL_H__

#include "Protocol.h"


class CRoutingProtocol :
	public CProtocol
{
protected:


public:

	//static int SLOT_DATA_SEND;  //���ݷ���slot

	CRoutingProtocol();
	~CRoutingProtocol();

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	//����final���
	static void PrintFinal(int currentTime);

};

#endif // __ROUTING_PROTOCOL_H__
