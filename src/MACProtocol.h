/***********************************************************************************************************************************

���� CMacProtocol �� ���̳��� CProcess > CAlgorithm > CProtocol ������MAC��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __MAC_PROTOCOL_H__
#define __MAC_PROTOCOL_H__

#include "Protocol.h"
#include "Node.h"
#include "GeneralNode.h"

#include "Frame.h"

class CMacProtocol :
	virtual public CProtocol
{
private:

	static int transmitSuccessful;  //�ɹ������ݴ���
	static int transmit;
	
protected:

	CMacProtocol();
	virtual ~CMacProtocol() = 0
	{};


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
		if( getConfig<double>("trans", "constant_trans_delay") >= 0 )
			return int(getConfig<double>("trans", "constant_trans_delay"));
		else
			return ROUND(double(nByte) / double(getConfig<int>("trans", "speed_trans")));
	}

	static int getTransmissionDelay(CFrame* frame)
	{
		return getTransmissionDelay(frame->getSize());
	}

	static int getTransmissionDelay(vector<CPacket*> packets)
	{
		return getTransmissionDelay(getConfig<int>("data", "size_header_mac") + CPacket::getSumSize(packets));
	}

	static int getMaxTransmissionDelay()
	{
		return getTransmissionDelay(getConfig<int>("data", "size_header_mac") + getConfig<int>("trans", "window_trans") * getConfig<int>("data", "size_data") + getConfig<int>("data", "size_ctrl"));
	}

	static vector<CGeneralNode*> findNeighbors(CGeneralNode& src);

	//����ʱ���� false�����򷵻� true
	//default transmitting, search for neighbors only among sensor nodes
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now, vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ))
	{
		return transmitFrame(src, frame, now, findNeighbors, receivePackets);
	}
	static bool receiveFrame(CGeneralNode& src, CFrame* frame, int now, vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ))
	{
		receiveFrame(src, frame, now, receivePackets);
	}
	//TODO: check again
	//custom transmitting, enabled routing protocols to pass in its own neighbor pool
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now, vector<CGeneralNode*> (*findNeighbors)(CGeneralNode&), vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ));
	static bool receiveFrame(CGeneralNode& gnode, CFrame* frame, int now, vector<CGeneralNode*>(*findNeighbors)( CGeneralNode& ), vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ));


	//���½ڵ���Ŀ���ڵ�״̬���ռ�λ�õ���Ϣ��ѡȡ�ȵ㡢���½ڵ��Ƿ�λ���ȵ�����
	//����޸���ڵ㣬���� false
	static bool Init()
	{
		return true;
	};
	static bool Prepare(int now);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int now);
	static void PrintInfo(vector<CNode*> allNodes, int now);
	static void PrintFinal(int now);

};

#endif // __MAC_PROTOCOL_H__
