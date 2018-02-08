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


private:

	typedef	struct _FIELD_CONFIGURATION
	{
		_TYPE_FIELD type;
		void * dstAttr;  //解析结果将赋值到该地址指向的变量
		void * value;  //value of this configuration
		bool state;  //仅用于 _none 时，记录该选项的开/关
	} _FIELD_CONFIGURATION;

	//string keyword;  //关键字：带参数的关键字如 "-sink"，不带参数的关键字如 "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //该关键字后附带的值域

	static map<string, pair<string, vector<_FIELD_CONFIGURATION> > > configurations;
	static string KEYWORD_HELP;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};


	//检测该关键字是否存在
	static bool ifExists(string keyword);
	static pair<string, vector<_FIELD_CONFIGURATION>> getConfiguration(string keyword);
	//最多允许带 2 个参数
	static void AddConfiguration(string keyword, string description, vector<_FIELD_CONFIGURATION> fields);
	static bool AddConfiguration(string keyword, void *OnValue, bool defaultState, void *dstAttr, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type, void *defaultValue, void * dstAttr, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type_1, void *defaultValue_1, void * dstAttr_1, 
								 _TYPE_FIELD type_2, void *defaultValue_2, void * dstAttr_2, string description);
	

protected:

	static void UpdateConfiguration(string keyword, vector<string> argvs);

	static vector<string> ConvertToConfiguration(int argc, char * argv[]);
	static bool ParseConfiguration(vector<string> args);
	static bool ParseConfiguration(string filename);

	static void InitConfiguration();
	static void ValidateConfiguration();
	static bool ApplyConfigurations();
	static void PrintConfiguration();

	static void Help();

};

#endif // __CONFIGURATION_H__
