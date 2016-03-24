#include "Package.h"
#include "MacProtocol.h"


CPackage::CPackage() : src(nullptr), dst(nullptr)
{
	init();
}

CPackage::CPackage(CGeneralNode& src, CGeneralNode& dst , vector<CGeneralData*> contents) : src(&src), dst(&dst)
{
	init();

	this->contents = contents;

//	vector<CGeneralData*> copy_contents;
//	for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); ++icontent)
//	{
//		CGeneralData* gcontent = nullptr;
//		if( typeid(*icontent) == typeid(CCtrl) )
//			gcontent = new CCtrl( *dynamic_cast<CCtrl*>(*icontent) );
//		else
//			gcontent = new CData( *dynamic_cast<CData*>(*icontent) );
//		copy_contents.push_back( gcontent );
//		gcontent = nullptr;
//	}
//	this->contents.insert( this->contents.end(), copy_contents.begin(), copy_contents.end() );  // TODO: untested !
}

//CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b) : src(&node), dst(&dst)
//{
//	init();
//	contents.push_back( new CCtrl(ctrl_a) );
//	contents.push_back( new CCtrl(ctrl_b) );
//}
//
//CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, CCtrl ctrl_c) : src(&node), dst(&dst)
//{
//	init();
//	contents.push_back( new CCtrl(ctrl_a) );
//	contents.push_back( new CCtrl(ctrl_b) );
//	contents.push_back( new CCtrl(ctrl_c) );
//}
//
//CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, vector<CData> datas) : src(&node), dst(&dst)
//{
//	init();
//	contents.push_back( new CCtrl(ctrl_a) );
//	contents.push_back( new CCtrl(ctrl_b) );
//
//	vector<CData*> copy_data;
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
//	{
//		copy_data.push_back( new CData(*idata) );
//	}
//	contents.insert( contents.end(), copy_data.begin(), copy_data.end() );  // TODO: untested !
//}
//
//CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl, vector<CData> datas) : src(&node), dst(&dst)
//{
//	init();
//	contents.push_back( new CCtrl(ctrl) );
//
//	vector<CData*> copy_data;
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
//	{
//		copy_data.push_back( new CData(*idata) );
//	}
//	contents.insert( contents.end(), copy_data.begin(), copy_data.end() );  // TODO: untested !
//}
//
//CPackage::CPackage(CGeneralNode& node, CGeneralNode& dst , vector<CData> datas) : src(&node), dst(&dst)
//{
//	init();
//
//	vector<CData*> copy_data;
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
//	{
//		copy_data.push_back( new CData(*idata) );
//	}
//	contents.insert( contents.end(), copy_data.begin(), copy_data.end() );
//}

CPackage::~CPackage()
{
	FreePointerVector(contents);
	contents.clear();
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
	for(auto icontent = contents.begin(); icontent != contents.end(); ++icontent)
	{
		size += (*icontent)->getSize();
	}
	return size;
}
