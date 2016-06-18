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
	CGeneralNode* dst;  //Ĭ�� null, �㲥
	int headerMac;
	//ע�⣺Ԫ�ؿ���Ϊ��ָ�룬����ζ���κ�����֮ǰ��Ҫ�ж�
	vector<CPacket*> packets;

	void init();
	CFrame();

public:

	//������
	CFrame(CGeneralNode& src, CGeneralNode& dst , vector<CPacket*> packets);
	//�㲥��
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
