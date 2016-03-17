#include "Ctrl.h"


CCtrl::CCtrl()
{
	init();
}

CCtrl::~CCtrl()
{
}

void CCtrl::init()
{
	CGeneralData::init();
	this->HOP = 1;
	this->dst = -1;
}

CCtrl::CCtrl(int node, int timeBirth, int byte, _TYPE_CTRL type)
{
	init();
	this->node = node;
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
	if( type != _rts )
	{
		cout << "Error @ CCtrl::CCtrl() : dst required !" << endl;
		_PAUSE_;
	}
};

CCtrl::CCtrl(int node, int dst, int timeBirth, int byte, _TYPE_CTRL type)
{
	init();
	this->node = node;
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
	if( type != _rts )
		this->dst = dst;
}

CCtrl::CCtrl(int node, int dst, int timeBirth, map<int, double> pred, int byte, _TYPE_CTRL type)
{
	init();
	this->node = node;
	this->time = this->timeBirth = timeBirth;
	if( type == _pred )
		this->pred = pred;
	this->size = byte;
	this->type = type;
	if( type != _rts )
		this->dst = dst;
}
