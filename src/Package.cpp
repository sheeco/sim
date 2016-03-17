#include "Package.h"
#include "MacProtocol.h"


CPackage::CPackage()
{
	init(nullptr);
}

CPackage::CPackage(CNode* node, CCtrl ctrl)
{
	init(node);
	content.push_back( new CCtrl(ctrl) );
}

CPackage::CPackage(CNode* node, CCtrl ctrl_a, CCtrl ctrl_b)
{
	init(node);
	content.push_back( new CCtrl(ctrl_a) );
	content.push_back( new CCtrl(ctrl_a) );
}

CPackage::CPackage(CNode* node, CCtrl ctrl, vector<CData> datas)
{
	init(node);
	content.push_back( new CCtrl(ctrl) );
	vector<CData*> copy_data;
	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
	{
		copy_data.push_back( new CData(*idata) );
	}
	content.insert( content.end(), copy_data.begin(), copy_data.end() );  // TODO: untested !
}

CPackage::~CPackage()
{
	FreePointerVector(content);
	content.clear();
}

void CPackage::init(CNode* node)
{
	this->node = node;
	this->macHeader = CMacProtocol::MAC_SIZE;
}

int CPackage::getSize() const
{
	int size = macHeader;
	for(auto icontent = content.begin(); icontent != content.end(); ++icontent)
		size += (*icontent)->getSize();
	return size;
}
