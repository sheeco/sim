/***********************************************************************************************************************************

���� CMacProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������MAC��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __MAC_PROTOCOL_H__
#define __MAC_PROTOCOL_H__

#include "Protocol.h"
#include "GeneralNode.h"
#include "Frame.h"

class CMacProtocol :
	virtual public CProtocol
{
private:

	static int transmitSuccessful;  //�ɹ������ݴ���
	static int transmit;
	
	//����ʱ���� false�����򷵻� true
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int currentTime);
	static bool receiveFrame(CGeneralNode& src, CFrame* frame, int currentTime);

	//�������� node �����ꡢռ�ձȺ͹���״̬���������ݣ������Ƿ����нڵ�
	static bool UpdateNodeStatus(int currentTime);


protected:

	CMacProtocol();
	virtual ~CMacProtocol() = 0
	{};

	//ע�⣺������ Prepare() ֮�����
	static void CommunicateWithNeighbor(int currentTime);


public:

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

	static int getTransmissionDelay(int nByte)
	{
		if( configs.trans.CONSTANT_TRANS_DELAY >= 0 )
			return int(configs.trans.CONSTANT_TRANS_DELAY);
		else
			return ROUND(double(nByte) / double(configs.trans.SPEED_TRANS));
	}

	static int getTransmissionDelay(CFrame* frame)
	{
		return getTransmissionDelay(frame->getSize());
	}

	static int getTransmissionDelay(vector<CPacket*> packets)
	{
		return getTransmissionDelay(configs.data.SIZE_HEADER_MAC + CPacket::getSumSize(packets));
	}

	static int getMaxTransmissionDelay()
	{
		return getTransmissionDelay(configs.data.SIZE_HEADER_MAC + configs.trans.WINDOW_TRANS * configs.data.SIZE_DATA + configs.data.SIZE_CTRL);
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
