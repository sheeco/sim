/***********************************************************************************************************************************

���� CMacProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������MAC��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __MAC_PROTOCOL_H__
#define __MAC_PROTOCOL_H__

#include "Protocol.h"
#include "GeneralNode.h"

class CMacProtocol :
	virtual public CProtocol
{
private:

	static int transmitSuccessful;  //�ɹ������ݴ���
	static int transmit;
	
	static void receiveFrame(CGeneralNode& src, CFrame* frame, int currentTime);
	static void broadcastFrame(CGeneralNode& src, CFrame* frame, int currentTime);
	static bool transmitFrame(CGeneralNode& src, CGeneralNode* dst, CFrame* frame, int currentTime);

	//���޶���Χ�������ɾһ��������node
	static void ChangeNodeNumber(int currentTime);
	//�������� node �����ꡢռ�ձȺ͹���״̬���������ݣ������Ƿ����нڵ�
	static bool UpdateNodeStatus(int currentTime);


protected:

	CMacProtocol();
	virtual ~CMacProtocol() = 0
	{};

	//ע�⣺������ Prepare() ֮�����
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

	//���½ڵ���Ŀ���ڵ�״̬���ռ�λ�õ���Ϣ��ѡȡ�ȵ㡢���½ڵ��Ƿ�λ���ȵ�����
	//����޸���ڵ㣬���� false
	static bool Prepare(int currentTime);
	//�������� MA �����ꡢ�ȴ�ʱ��
	//ע�⣺��������һ���ȵ�ѡȡ֮�����
	static void UpdateMANodeStatus(int currentTime);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

#endif // __MAC_PROTOCOL_H__
