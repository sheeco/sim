#include "Ctrl.h"


CCtrl::CCtrl()
{
	CCtrl::init();
}

void CCtrl::init()
{
	CPacket::init();
	this->HOP = 1;
	this->type = _rts;
	this->capacity = 0;
}

CCtrl::CCtrl(int node, int timeBirth, int byte, EnumCtrlType type)
{
	CCtrl::init();
	this->node = node;
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, vector<CData> datas, int timeBirth, int byte, EnumCtrlType type)
{
	CCtrl::init();
	this->node = node;
	if( type == _ack )
	{
		this->ack = datas;
	}
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, int capacity, int timeBirth, int byte, EnumCtrlType type)
{
	CCtrl::init();
	this->node = node;
	if( type == _capacity )
	{
		this->capacity = capacity;
	}
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, map<int, double> pred, int timeBirth, int byte, EnumCtrlType type)
{
	CCtrl::init();
	this->node = node;
	if( type == _index )
	{
		this->pred = pred;
	}
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

//CCtrl::CCtrl(int node, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type)
//{
//	CCtrl::init();
//	this->node = node;
//	if( type == _index )
//	{
//		this->sv = sv;
//	}
//	this->time = this->timeBirth = timeBirth;
//	this->size = byte;
//	this->type = type;
//}
