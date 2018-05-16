#pragma once

#ifndef __EPIDEMIC_H__
#define __EPIDEMIC_H__

#include "RoutingProtocol.h"
#include "Sink.h"


class CEpidemic :
	virtual public CRoutingProtocol
{
private:

public:

	CEpidemic();
	~CEpidemic();

	//从下层协议传入的控制/数据包
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// Node <- Sink 
	static vector<CPacket*> receivePackets(CNode* node, CSink* sink, vector<CPacket*> packets, int time);
	// Sink <- Node 
	static vector<CPacket*> receivePackets(CSink* sink, CNode* fromNode, vector<CPacket*> packets, int time);
	// Node <- Node 
	static vector<CPacket*> receivePackets(CNode* node, CNode* fromNode, vector<CPacket*> packets, int time);

	static void CommunicateBetweenNeighbors(int now);

	static bool Init(int now);
	static bool Operate(int now);

};

#endif // __EPIDEMIC_H__
