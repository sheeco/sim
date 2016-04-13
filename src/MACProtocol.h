/***********************************************************************************************************************************

基类 CMacProtocol ： （继承自 CProcess > CAlgorithm > CProtocol ）所有MAC层协议算法的实现类，都应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#include "Protocol.h"
#include "GeneralNode.h"

class CMacProtocol :
	public CProtocol
{
private:

	static int transmitSuccessful;  //成功的数据传输
	static int transmit;
	
	static void receivePackage(CGeneralNode& src, CPackage* package, int currentTime);
	static void broadcastPackage(CGeneralNode& src, CPackage* package, int currentTime);
	static bool transmitPackage(CGeneralNode& src, CGeneralNode* dst, CPackage* package, int currentTime);

	//在限定范围内随机增删一定数量的node
	static void ChangeNodeNumber(int currentTime);
	//更新所有 node 的坐标、占空比和工作状态，生成数据，返回是否仍有节点
	static bool UpdateNodeStatus(int currentTime);


protected:

	CMacProtocol();
	~CMacProtocol();

	//注意：必须在 Prepare() 之后调用
	static void CommunicateWithNeighbor(int currentTime);


public:

	static int SIZE_HEADER_MAC;  //Mac Header Size

	static bool RANDOM_STATE_INIT;  //是否初始化所有节点的工作状态为随机的
	static bool TEST_DYNAMIC_NUM_NODE;
	static int SLOT_CHANGE_NUM_NODE;  //动态节点个数测试时，节点个数发生变化的周期

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

