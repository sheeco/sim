#pragma once

#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#include "Data.h"
#include "Ctrl.h"
//#include "GeneralNode.h"


class CGeneralNode;

class CPackage :
	public CEntity
{
private:

	CGeneralNode* src;
	CGeneralNode* dst;  //Ĭ�� null, �㲥
	int headerMac;
	//ע�⣺Ԫ�ؿ���Ϊ��ָ�룬����ζ���κ�����֮ǰ��Ҫ�ж�
	vector<CGeneralData*> contents;

	void init();
	CPackage();

public:

//	//ACK Package
	CPackage(CGeneralNode& src, CGeneralNode& dst , vector<CGeneralData*> contents);

//	//RTS + index / CTS + DP / ACK + NODATA
//	CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b);
//	//CTS + DP + NODATA
//	CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, CCtrl ctrl_c);
//	//CTS + DP + DATA
//	CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, vector<CData> datas);
//	//CTS + DATA (only to Sink)
//	CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl, vector<CData> datas);
//	//DATA only
//	CPackage(CGeneralNode& node, CGeneralNode& dst , vector<CData> datas);
	~CPackage();

	inline CGeneralNode* getSrcNode() const
	{
		return src;
	}
	inline CGeneralNode* getDstNode() const
	{
		return dst;
	}
	inline vector<CGeneralData*> getContents() const
	{
		return contents;
	}

	int getSize() const;

};

#endif // __PACKAGE_H__
