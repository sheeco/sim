#include "GeneralData.h"


CGeneralData::CGeneralData()
{
	CGeneralData::init();
}

CGeneralData::~CGeneralData()
{
}

void CGeneralData::init()
{
	this->node = -1;
	this->timeBirth = 0;
	this->size = 0;
	this->HOP = 0;
}
