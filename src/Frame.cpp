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
}

CFrame::CFrame(CGeneralNode & src, vector<CPacket*> packets) : src(&src), dst(nullptr)
{
	init();
	this->packets = packets;
}

CFrame::~CFrame()
{
	FreePointerVector(packets);
	packets.clear();
}

void CFrame::init()
{
	this->headerMac = getConfig<int>("data", "size_header_mac");
}

int CFrame::getSize() const
{
	return headerMac + CPacket::getSumSize(packets);
}
