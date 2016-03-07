/***********************************************************************************************************************************

���� CRoutingProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������·��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#include "Protocol.h"


class CRoutingProtocol :
	public CProtocol
{
protected:

	static int SLOT_DATA_SEND;  //���ݷ���slot

	//���޶���Χ�������ɾһ��������node
	static void ChangeNodeNumber(int currentTime);
	//��������node�����ꡢռ�ձȺ͹���״̬
	static void UpdateNodeStatus(int currentTime);


public:

	static bool TEST_DYNAMIC_NUM_NODE;
	static bool TEST_HOTSPOT_SIMILARITY;
	static int SLOT_CHANGE_NUM_NODE;  //��̬�ڵ��������ʱ���ڵ���������仯������

CRoutingProtocol(){};
	~CRoutingProtocol(){};

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

