/***********************************************************************************************************************************

������ CConfiguration �� ���в��������࣬Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"


// TODO: def in this class or create multiple derived class ?
class CConfiguration
{
private:

	typedef enum _TYPE_VALUE
	{
		_int,
		_double,
		_bool
	} _TYPE_VALUE;
	
	typedef	struct _VALUE_CONFIGURATION
	{
		_TYPE_VALUE type = _int;
		void * dst = nullptr;
	} _VALUE_CONFIGURATION;

	string keyword;
	vector<_VALUE_CONFIGURATION> values;


public:

	CConfiguration();
	~CConfiguration();

};

#endif // __CONFIGURATION_H__
