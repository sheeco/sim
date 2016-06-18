#pragma once

#ifndef __FRAME_H__
#define __FRAME_H__

#include "Data.h"
#include "Ctrl.h"


class CGeneralNode;

class CFrame :
	virtual public CGeneralData
{
private:

	CGeneralNode* src;
	CGeneralNode* dst;  //默认 null, 广播
	int headerMac;
	//注意：元素可能为空指针，这意味着任何引用之前需要判断
	vector<CPacket*> packets;

	void init();
	CFrame();

public:

	//单播包
	CFrame(CGeneralNode& src, CGeneralNode& dst , vector<CPacket*> packets);
	//广播包
	CFrame(CGeneralNode& src, vector<CPacket*> packets);

//	//RTS + index / CTS + DP / ACK + NODATA
//	CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b);
//	//CTS + DP + NODATA
//	CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, CCtrl ctrl_c);
//	//CTS + DP + DATA
//	CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, vector<CData> datas);
//	//CTS + DATA (only to Sink)
//	CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl, vector<CData> datas);
//	//DATA only
//	CFrame(CGeneralNode& node, CGeneralNode& dst , vector<CData> datas);
	~CFrame();

	inline CGeneralNode* getSrcNode() const
	{
		return src;
	}
	inline CGeneralNode* getDstNode() const
	{
		return dst;
	}
	inline vector<CPacket*> getPackets() const
	{
		return packets;
	}

	int getSize() const;

};

#endif // __FRAME_H__
