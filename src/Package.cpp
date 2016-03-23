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
	content.push_back( new CCtrl(ctrl_b) );
}

CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl, vector<CData> datas) : src(&node), dst(&dst)
{
	init();
	content.push_back( new CCtrl(ctrl) );
	if( datas.empty() )  //收到数据队列为空，代表没有需要传输的数据，压入一个空指针
	{
		CGeneralData* temp = nullptr;
		content.push_back( temp );
		return;
	}

	vector<CData*> copy_data;
	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
	{
		copy_data.push_back( new CData(*idata) );
	}
	content.insert( content.end(), copy_data.begin(), copy_data.end() );  // TODO: untested !
}

CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , vector<CData> datas) : src(&node), dst(&dst)
{
	init();
	if( datas.empty() )  //收到数据队列为空，代表没有需要传输的数据，压入一个空指针
	{
		CGeneralData* temp = nullptr;
		content.push_back( temp );
		return;
	}

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
	this->headerMac = CMacProtocol::SIZE_HEADER_MAC;
	if( dst != nullptr
		&& dst->getID() == -1 )
		dst = nullptr;
}

int CPackage::getSize() const
{
	int size = headerMac;
	for(auto icontent = content.begin(); icontent != content.end(); ++icontent)
	{
		if( *icontent != nullptr)
			size += (*icontent)->getSize();
	}
	return size;
}
