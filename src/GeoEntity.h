/***********************************************************************************************************************************

�μ��� CGeoEntity �� ���̳��Զ����� CEntity ����CBasicEntity���ƣ���ר���ڿռ���ص�ʵ�壬���пռ���ص�ʵ�壬Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __GEO_ENTITY_H__
#define __GEO_ENTITY_H__

#include "Entity.h"


class CGeoEntity :
	virtual public CEntity
{
public:

	CGeoEntity(){};
	virtual ~CGeoEntity() = 0
	{};

};

#endif // __GEO_ENTITY_H__
