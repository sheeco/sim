#pragma once

#ifndef __GENERAL_DATA_H__
#define __GENERAL_DATA_H__

#include "BasicEntity.h"


class CGeneralData :
	virtual public CBasicEntity
{
//protected:

//	int ID;  //data编号
//	CCoordinate location;  //未使用
//	int time;  //该data最后一次状态更新的时间戳，用于校验，初始值应等于timeBirth
//	bool flag;

public:

	CGeneralData(){};
	virtual ~CGeneralData() = 0
	{};

};
	
//CGeneralData::~CGeneralData(){};

#endif // __GENERAL_DATA_H__
