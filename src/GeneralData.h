#pragma once

#ifndef __GENERAL_DATA_H__
#define __GENERAL_DATA_H__

#include "BasicEntity.h"


class CGeneralData :
	virtual public CBasicEntity
{
//protected:

//	int ID;  //data���
//	CCoordinate location;  //δʹ��
//	int time;  //��data���һ��״̬���µ�ʱ���������У�飬��ʼֵӦ����timeBirth
//	bool flag;

public:

	CGeneralData(){};
	virtual ~CGeneralData() = 0
	{};

};
	
//CGeneralData::~CGeneralData(){};

#endif // __GENERAL_DATA_H__
