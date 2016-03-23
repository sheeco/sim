#pragma once

#include "Data.h"
#include "Ctrl.h"
//#include "GeneralNode.h"


class CGeneralNode;

class CPackage :
	public CEntity
{
private:

	CGeneralNode* src;
	CGeneralNode* dst;  //默认 null, 广播
	int headerMac;
	//注意：元素可能为空只恨，这意味着任何引用之前需要判断
	vector<CGeneralData*> content;

	void init();
	CPackage();

public:

	//RTS / CTS / ACK Package
	CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl);
	//CTS piggyback with data index ( delivery preds & summary vector )
	CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b);
	//datas piggyback with data index ( delivery preds & summary vector ) / ACK
	CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl, vector<CData> datas);
	//datas only
	CPackage(CGeneralNode& node, CGeneralNode& dst , vector<CData> datas);
	~CPackage();

	inline CGeneralNode* getSrcNode() const
	{
		return src;
	}
	inline CGeneralNode* getDstNode() const
	{
		return dst;
	}
	inline vector<CGeneralData*> getContent() const
	{
		return content;
	}

	int getSize() const;

};

