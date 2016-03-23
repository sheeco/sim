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

	//��������node�����ꡢռ�ձȺ͹���״̬
	static void UpdateNodeStatus(int currentTime);

	static void receivePackage(CGeneralNode& node, CPackage* package, int currentTime);
	static bool broadcastPackage(CPackage* package, int currentTime);
	static bool transmitPackage(CPackage* package, CGeneralNode* dst, int currentTime);


protected:

	CMacProtocol();

	//���޶���Χ�������ɾһ��������node
	static void ChangeNodeNumber(int currentTime);

	static void TransmitData(int currentTime);


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

