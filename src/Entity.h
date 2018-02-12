/***********************************************************************************************************************************

顶级类 CEntity ： 所有实体类或实体类的基类，应该最终继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __ENTITY_H__
#define __ENTITY_H__


class CEntity
{
public:

	CEntity(){};
	virtual ~CEntity() = 0
	{};

	virtual string toString()
	{
		return toString(true);
	};
	virtual string toString(bool brief)
	{
		return "";
	};

};

#endif // __ENTITY_H__
