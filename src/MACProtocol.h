/***********************************************************************************************************************************

���� CMacProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������MAC��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#include "Protocol.h"
#include "GeneralNode.h"

class CMacProtocol :
	public CProtocol
{
private:

	static void receivePackage(CGeneralNode& src, CPackage* package, int currentTime);
	static void broadcastPackage(CGeneralNode& src, CPackage* package, int currentTime);
	static bool transmitPackage(CGeneralNode& src, CGeneralNode* dst, CPackage* package, int currentTime);


protected:

	CMacProtocol();

	//���޶���Χ�������ɾһ��������node
	static void ChangeNodeNumber(int currentTime);

	//��������node�����ꡢռ�ձȺ͹���״̬���������ݣ������Ƿ����нڵ�
	static bool UpdateNodeStatus(int currentTime);

	static void CommunicateWithNeighbor(int currentTime);


public:

	static int SIZE_HEADER_MAC;  //Mac Header Size

	static bool RANDOM_STATE_INIT;  //�Ƿ��ʼ�����нڵ�Ĺ���״̬Ϊ�����
	static bool TEST_DYNAMIC_NUM_NODE;
	static int SLOT_CHANGE_NUM_NODE;  //��̬�ڵ��������ʱ���ڵ���������仯������

	~CMacProtocol();

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

