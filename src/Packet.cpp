#include "Packet.h"


CPacket::CPacket()
{
	CPacket::init();
}

int CPacket::getSumSize(vector<CPacket*> packets)
{
	int size = 0;
	for( auto ipacket = packets.begin(); ipacket != packets.end(); ++ipacket )
	{
		size += ( *ipacket )->getSize();
	}
	return size;
}

void CPacket::init()
{
	this->node = -1;
	this->timeBirth = 0;
	this->size = 0;
	this->HOP = 0;
	this->MAX_HOP = INVALID;
}
