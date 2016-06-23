/***********************************************************************************************************************************

顶级类 CConfiguration ： 所有参数配置类，应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"


// TODO: def in this class or create multiple derived class ?
class CConfiguration
{
	friend class CRunHelper;


public:

	typedef enum _TYPE_FIELD
	{
		_none,  //该 keyword 不带任何参数时，向 values 中压入一个 _none 型的 _TYPE_VALUE，并将 defaultValue 的值赋给 dstAttr
		_int,  //按照整型格式解析
		_double,  //按照浮点型格式解析
		_bool  //按照布尔型解析，关键字由 KEYWORD_TRUE, KEYWORD_FALSE 指定
	} _TYPE_FIELD;


private:

	typedef	struct _FIELD_CONFIGURATION
	{
		_TYPE_FIELD type;
		void * dstAttr;  //解析结果将赋值到该地址指向的变量
		int defaultValue;  //仅当 type == _none 时，允许显式指定此变量的值（一般为枚举型）；对于 _int / _double / _bool 不允许显式指定，默认为 0
	} _FIELD_CONFIGURATION;

	//string keyword;  //关键字：带参数的关键字如 "-sink"，不带参数的关键字如 "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //该关键字后附带的值域

	static map<string, pair<string, vector<_FIELD_CONFIGURATION> > > configurations;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};

	//void addValue(_TYPE_VALUE type, void * dstAttr, int defaultValue);
	void setFieldValue();


protected:

	//检测该关键字是否存在
	static bool ifExists(string keyword);
	//最多允许带 2 个参数
	static bool AddConfiguration(string keyword, void * dstAttr, int value, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type, void * dstAttr, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type_1, void * dstAttr_1, _TYPE_FIELD type_2, void * dstAttr_2, string description);

	static bool ParseConfiguration(int argc, char * argv[], string description);
	static bool ParseConfiguration(vector<string> args, string description);
	static bool ParseConfiguration(string filename);
};

#endif // __CONFIGURATION_H__
