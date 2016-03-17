#pragma once

#include "MacProtocol.h"
#include "Node.h"


class CSMac :
	public CMacProtocol
{
public:

	CSMac(){};
	~CSMac(){};

	static void DetectNeighborNode(CNode* node, int currentTime);

	static bool Operate(int currentTime);

//	//打印相关信息到文件
//	static void PrintInfo(int currentTime);
//	static void PrintFinal(int currentTime);

};

