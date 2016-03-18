#include "Ctrl.h"


CCtrl::CCtrl()
{
	CCtrl::init();
}

CCtrl::~CCtrl()
{
}

void CCtrl::init()
{
	CGeneralData::init();
	this->HOP = 1;
}

CCtrl::CCtrl(int node, int timeBirth, int byte, _TYPE_CTRL type)
{
	CCtrl::init();
	this->node = node;
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, vector<CData> datas, int timeBirth, int byte, _TYPE_CTRL type)
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

CCtrl::CCtrl(int node, map<int, double> pred, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type)
{
	CCtrl::init();
	this->node = node;
	if( type == _index )
	{
		this->pred = pred;
		this->sv = sv;
	}
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}
