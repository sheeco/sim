#pragma once

#include "MacProtocol.h"
#include "Node.h"


class CSMac :
	public CMacProtocol
{
public:

	CSMac(){};
	~CSMac(){};

//	static void DetectNeighborNode(CNode* node, int currentTime);

	static bool Operate(int currentTime);

//	//��ӡ�����Ϣ���ļ�
//	static void PrintInfo(int currentTime);
//	static void PrintFinal(int currentTime);

};

