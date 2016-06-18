#include "Packet.h"


CPacket::CPacket()
{
	CPacket::init();
}

void CPacket::init()
{
	this->node = -1;
	this->timeBirth = 0;
	this->size = 0;
	this->HOP = 0;
}
