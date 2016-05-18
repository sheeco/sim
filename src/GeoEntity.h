/***********************************************************************************************************************************

次级类 CGeoEntity ： （继承自顶级类 CEntity ）与CBasicEntity类似，但专用于空间相关的实体，所有空间相关的实体，应该继承自这个类

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
	~CGeoEntity(){};

};

#endif // __GEO_ENTITY_H__
