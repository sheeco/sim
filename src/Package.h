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
	CGeneralNode* dst;  //Ĭ�� null, �㲥
	int macHeader;
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
	~CPackage();

	inline CGeneralNode* getSourceNode() const
	{
		return src;
	}
	inline CGeneralNode* getDestNode() const
	{
		return dst;
	}
	inline vector<CGeneralData*> getContent() const
	{
		return content;
	}

	int getSize() const;

};

