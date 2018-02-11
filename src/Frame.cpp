#include "Frame.h"
#include "MacProtocol.h"
#include "Configuration.h"


CFrame::CFrame() : src(nullptr), dst(nullptr)
{
	init();
}

CFrame::CFrame(CGeneralNode& src, CGeneralNode& dst , vector<CPacket*> packets) : src(&src), dst(&dst)
{
	init();

	this->packets = packets;

//	vector<CPacket*> copy_packets;
//	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); ++ipacket)
//	{
//		CPacket* gpacket = nullptr;
//		if( typeid(*ipacket) == typeid(CCtrl) )
//			gpacket = new CCtrl( *dynamic_cast<CCtrl*>(*ipacket) );
//		else
//			gpacket = new CData( *dynamic_cast<CData*>(*ipacket) );
//		copy_packets.push_back( gpacket );
//		gpacket = nullptr;
//	}
//	this->packets.insert( this->packets.end(), copy_packets.begin(), copy_packets.end() );  // TODO: untested !
}

CFrame::CFrame(CGeneralNode & src, vector<CPacket*> packets) : src(&src), dst(nullptr)
{
	init();

	this->packets = packets;
}

//CFrame::CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b) : src(&node), dst(&dst)
//{
//	init();
//	packets.push_back( new CCtrl(ctrl_a) );
//	packets.push_back( new CCtrl(ctrl_b) );
//}
//
//CFrame::CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, CCtrl ctrl_c) : src(&node), dst(&dst)
//{
//	init();
//	packets.push_back( new CCtrl(ctrl_a) );
//	packets.push_back( new CCtrl(ctrl_b) );
//	packets.push_back( new CCtrl(ctrl_c) );
//}
//
//CFrame::CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl_a, CCtrl ctrl_b, vector<CData> datas) : src(&node), dst(&dst)
//{
//	init();
//	packets.push_back( new CCtrl(ctrl_a) );
//	packets.push_back( new CCtrl(ctrl_b) );
//
//	vector<CData*> copy_data;
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
//	{
//		copy_data.push_back( new CData(*idata) );
//	}
//	packets.insert( packets.end(), copy_data.begin(), copy_data.end() );  // TODO: untested !
//}
//
//CFrame::CFrame(CGeneralNode& node, CGeneralNode& dst , CCtrl ctrl, vector<CData> datas) : src(&node), dst(&dst)
//{
//	init();
//	packets.push_back( new CCtrl(ctrl) );
//
//	vector<CData*> copy_data;
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
//	{
//		copy_data.push_back( new CData(*idata) );
//	}
//	packets.insert( packets.end(), copy_data.begin(), copy_data.end() );  // TODO: untested !
//}
//
//CFrame::CFrame(CGeneralNode& node, CGeneralNode& dst , vector<CData> datas) : src(&node), dst(&dst)
//{
//	init();
//
//	vector<CData*> copy_data;
//	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++ idata)
//	{
//		copy_data.push_back( new CData(*idata) );
//	}
//	packets.insert( packets.end(), copy_data.begin(), copy_data.end() );
//}

CFrame::~CFrame()
{
	FreePointerVector(packets);
	packets.clear();
}

void CFrame::init()
{
	this->headerMac = configs.data.SIZE_HEADER_MAC;
}

int CFrame::getSize() const
{
	return headerMac + CPacket::getSumSize(packets);
}
