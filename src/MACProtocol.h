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

	//更新所有node的坐标、占空比和工作状态
	static void UpdateNodeStatus(int currentTime);

	static void receivePackage(CGeneralNode& node, CPackage* package, int currentTime);
	static bool broadcastPackage(CPackage* package, int currentTime);
	static bool transmitPackage(CPackage* package, CGeneralNode* dst, int currentTime);


protected:

	CMacProtocol();

	//在限定范围内随机增删一定数量的node
	static void ChangeNodeNumber(int currentTime);

	static void TransmitData(int currentTime);


public:

	static int SIZE_HEADER_MAC;  //Mac Header Size

	static bool TEST_DYNAMIC_NUM_NODE;
	static int SLOT_CHANGE_NUM_NODE;  //动态节点个数测试时，节点个数发生变化的周期

	~CMacProtocol();

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

