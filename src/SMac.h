#pragma once

#ifndef __S_MAC_H__
#define __S_MAC_H__

#include "MacProtocol.h"
#include "Node.h"


class CSMac :
	virtual public CMacProtocol
{
public:

	CSMac(){};
	~CSMac(){};

//	static void DetectNeighborNode(CNode* node, int now);

	static bool Operate(int now);

//	//��ӡ�����Ϣ���ļ�
//	static void PrintInfo(int now);
//	static void PrintFinal(int now);

};

#endif // __S_MAC_H__
