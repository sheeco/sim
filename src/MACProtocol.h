/***********************************************************************************************************************************

基类 CMacProtocol ： （继承自 CProcess > CAlgorithm > CProtocol ）所有MAC层协议算法的实现类，都应该继承自这个类

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

	static int transmitSuccessful;  //成功的数据传输
	static int transmit;
	
	//过听时返回 false；否则返回 true
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int currentTime);
	static bool receiveFrame(CGeneralNode& src, CFrame* frame, int currentTime);

	//更新所有 node 的坐标、占空比和工作状态，生成数据，返回是否仍有节点
	static bool UpdateNodeStatus(int currentTime);


protected:

	CMacProtocol();
	virtual ~CMacProtocol() = 0
	{};

	//注意：必须在 Prepare() 之后调用
	static void CommunicateWithNeighbor(int currentTime);


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


	//更新节点数目、节点状态；收集位置点信息、选取热点、更新节点是否位于热点区域；
	//如果无更多节点，返回 false
	static bool Prepare(int currentTime);
	//更新所有 MA 的坐标、等待时间
	//注意：必须在新一轮热点选取之后调用
	static void UpdateMANodeStatus(int currentTime);

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

#endif // __MAC_PROTOCOL_H__
