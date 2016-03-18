#include "Package.h"
#include "MacProtocol.h"


CPackage::CPackage() : src(nullptr), dst(nullptr)
{
	init();
}

CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl) : src(&node), dst(&dst)
{
	init();
	content.push_back( new CCtrl(ctrl) );
}

CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b) : src(&node), dst(&dst)
{
	init();
	content.push_back( new CCtrl(ctrl_a) );
	content.push_back( new CCtrl(ctrl_a) );
}

CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl, vector<CData> datas) : src(&node), dst(&dst)
{
	init();
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

void CPackage::init()
{
	this->macHeader = CMacProtocol::MAC_SIZE;
	if( dst != nullptr
		&& dst->getID() == -1 )
		dst = nullptr;
}

int CPackage::getSize() const
{
	int size = macHeader;
	for(auto icontent = content.begin(); icontent != content.end(); ++icontent)
		size += (*icontent)->getSize();
	return size;
}
