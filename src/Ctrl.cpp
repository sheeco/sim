#include "Ctrl.h"


CCtrl::CCtrl()
{
	init();
}

void CCtrl::init()
{
	CPacket::init();
	this->HOP = 0;
	this->MAX_HOP = 1;
	this->type = _rts;
	this->capacity = 0;
}

CCtrl::CCtrl(int node, int timeBirth, int byte, EnumCtrlType type)
{
	init();
	this->node = node;
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, vector<CData> datas, int timeBirth, int byte, EnumCtrlType type)
{
	init();
	this->node = node;
	if( type == _ack )
	{
		this->ack = datas;
	}
	else
		throw string("CCtrl::CCtrl(): `type` must be _ack for this constructor.");
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, int capacity, int timeBirth, int byte, EnumCtrlType type)
{
	init();
	this->node = node;
	if( type == _capacity )
	{
		this->capacity = capacity;
	}
	else
		throw string("CCtrl::CCtrl(): `type` must be _capacity for this constructor.");
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}
