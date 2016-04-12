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

	static int transmitSuccessful;  //�ɹ������ݴ���
	static int transmit;
	
	static void receivePackage(CGeneralNode& src, CPackage* package, int currentTime);
	static void broadcastPackage(CGeneralNode& src, CPackage* package, int currentTime);
	static bool transmitPackage(CGeneralNode& src, CGeneralNode* dst, CPackage* package, int currentTime);


protected:

	CMacProtocol();
	~CMacProtocol();

	//���޶���Χ�������ɾһ��������node
	static void ChangeNodeNumber(int currentTime);

	static void CommunicateWithNeighbor(int currentTime);


public:

	static int SIZE_HEADER_MAC;  //Mac Header Size

	static bool RANDOM_STATE_INIT;  //�Ƿ��ʼ�����нڵ�Ĺ���״̬Ϊ�����
	static bool TEST_DYNAMIC_NUM_NODE;
	static int SLOT_CHANGE_NUM_NODE;  //��̬�ڵ��������ʱ���ڵ���������仯������

	//���ݴ������������ͳ�����ݴ���ɹ��İٷֱ�
	//���ݴ���Ϊ˫����㣬�����ڵ�ɹ��յ����ݾͼ���һ��
	static void transmitTry()
	{
		++transmit;
	}
	//���ݴ���Ϊ˫����㣬�����ڵ� / Sink�ɹ��յ����ݾͼ���һ��
	static void transmitSucceed()
	{
		++transmitSuccessful;
	}

	static int getTransmit()
	{
		return transmit;
	}
	static int getTransmitSuccessful()
	{
		return transmitSuccessful;
	}
	static double getPercentTransmitSuccessful()
	{
		if( transmitSuccessful == 0 )
			return 0.0;
		return double(transmitSuccessful) / double(transmit);
	}

	//��������node�����ꡢռ�ձȺ͹���״̬���������ݣ������Ƿ����нڵ�
	static bool UpdateNodeStatus(int currentTime);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

