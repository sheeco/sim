/***********************************************************************************************************************************

���� CRoutingProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������·��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#include "Protocol.h"


class CRoutingProtocol :
	public CProtocol
{
protected:


public:

	static int SLOT_DATA_SEND;  //���ݷ���slot
	static bool TEST_HOTSPOT_SIMILARITY;

	CRoutingProtocol(){};
	~CRoutingProtocol(){};

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	//����debug���
	static void PrintFinal(int currentTime);

};

