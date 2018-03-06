/***********************************************************************************************************************************

基类 CMacProtocol ： （继承自 CProcess > CAlgorithm > CProtocol ）所有MAC层协议算法的实现类，都应该继承自这个类

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

	static int transmitSuccessful;  //成功的数据传输
	static int transmit;
	
protected:

	CMacProtocol();
	virtual ~CMacProtocol() = 0
	{};


public:

	//数据传输计数：用于统计数据传输成功的百分比
	//数据传输为双向计算，单方节点成功收到数据就记作一次
	static void transmitTry()
	{
		++transmit;
	}
	//数据传输为双向计算，单方节点 / Sink成功收到数据就记作一次
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

	//过听时返回 false；否则返回 true
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now);
	//TODO: change to below
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now, vector<CGeneralNode*> (*findNeighbors)(CGeneralNode&), vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ));
	static bool receiveFrame(CGeneralNode& src, CFrame* frame, int now);
	//TODO: change to below
	static bool receiveFrame(CGeneralNode& gnode, CFrame* frame, int now, vector<CGeneralNode*>(*findNeighbors)( CGeneralNode& ), vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ));

	//注意：必须在 Prepare() 之后调用
	static void CommunicateWithNeighbor(int now);


	//更新节点数目、节点状态；收集位置点信息、选取热点、更新节点是否位于热点区域；
	//如果无更多节点，返回 false
	static bool Init()
	{
		return true;
	};
	static bool Prepare(int now);

	//打印相关信息到文件
	static void PrintInfo(int now);
	static void PrintInfo(vector<CNode*> allNodes, int now);
	static void PrintFinal(int now);

};

#endif // __MAC_PROTOCOL_H__
