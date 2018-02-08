/***********************************************************************************************************************************

顶级类 CConfiguration ： 所有参数配置类，应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"


class CConfiguration
{
	friend class CRunHelper;


public:

	typedef enum _TYPE_FIELD
	{
		_none,  //该 keyword 不带任何参数时，向 values 中压入一个 _none 型的 _TYPE_VALUE，并将 defaultValue 的值赋给 dstAttr
		_bool,  //按照布尔型解析，关键字由 KEYWORD_TRUE, KEYWORD_FALSE 指定
		_int,  //按照整型格式解析
		_double,  //按照浮点型格式解析
		_string  //按照字符串解析
	} _TYPE_FIELD;


	// TODO: define all the configs here as static attribute
	static string PATH_TRACE;


private:

	typedef	struct _FIELD_CONFIGURATION
	{
		_TYPE_FIELD type;
		void * dstAttr;  //解析结果将赋值到该地址指向的变量
		int defaultValue;  //仅当 type == _none 时，允许显式指定此变量的值（一般为枚举型）；对于 _bool /_int / _double /string 不允许显式指定，默认为 0 或空串
	} _FIELD_CONFIGURATION;

	//string keyword;  //关键字：带参数的关键字如 "-sink"，不带参数的关键字如 "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //该关键字后附带的值域

	static map<string, pair<string, vector<_FIELD_CONFIGURATION> > > configurations;
	static string KEYWORD_HELP;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};

	//void addValue(_TYPE_VALUE type, void * dstAttr, int defaultValue);
	static void setValue(void * pAttr, bool value);
	static void setValue(void * pAttr, int value);
	static void setValue(void * pAttr, double value);
	static void setValue(void * pAttr, string value);


protected:

	//检测该关键字是否存在
	static bool ifExists(string keyword);
	//最多允许带 2 个参数
	// TODO: add default value ?
	static bool AddConfiguration(string keyword, void * dstAttr, int value, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type, void * dstAttr, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type_1, void * dstAttr_1, _TYPE_FIELD type_2, void * dstAttr_2, string description);

	static vector<string> getConfiguration(int argc, char * argv[]);
	static bool ParseConfiguration(vector<string> args, string description);
	static bool ParseConfiguration(string filename);

	static bool ApplyConfigurations();
};

#endif // __CONFIGURATION_H__
