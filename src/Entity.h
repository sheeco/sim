/***********************************************************************************************************************************

������ CEntity �� ����ʵ�����ʵ����Ļ��࣬Ӧ�����ռ̳��������

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
