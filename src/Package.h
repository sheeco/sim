#pragma once

#include "Data.h"
#include "Ctrl.h"


class CPackage :
	public CEntity
{
private:

	CNode* node;
	int macHeader;
	vector<CGeneralData*> content;

	void init(CNode* node);
	CPackage();

public:

	CPackage(CNode* node, CCtrl ctrl);
	CPackage(CNode* node, CCtrl ctrl_a, CCtrl ctrl_b);
	CPackage(CNode* node, CCtrl ctrl, vector<CData> datas);
	~CPackage();

	inline CNode* getNode() const
	{
		return node;
	}
	inline vector<CGeneralData*> getContent() const
	{
		return content;
	}

	int getSize() const;

};

