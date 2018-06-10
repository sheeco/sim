/***********************************************************************************************************************************

顶级类 CConfiguration ： 所有参数配置类，应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"
#include "ParseHelper.h"


//用于实现配置参数的定义、初始化、访问、更新的类
//所有协议/方案的选择、算法参数、网络环境、仿真过程、控制台输出和日志文件等方面的参数，
//都被作为配置参数，可以通过配置文件`bin/default.config`或命令行参数来修改
class CConfiguration :
	virtual public CHelper
{
	friend class CRunHelper;

private:

	static map<string, map<string, pair<void*, EnumType>>> configurations;

	//Add new configuration group
	static void addGroup(string group);

	//Add new configuration to given group, given configuration keyword, type & default value
	static void addConfiguration(string group, string keyword, const type_info& type, void *value);

	static string KEYWORD_HELP;


	CConfiguration() {};
	~CConfiguration() {};

	//Check if configuration group with given name exists
	static bool has(string group);
	//Check if configuration with given name exists in certain group
	static bool has(string group, string keyword);
	
	//Get configuration value by group name & keyword
	static pair<void*, EnumType> getConfiguration(string group, string keyword);


protected:

	//Parse configuration updates from command arguments
	static bool ParseConfiguration(vector<string> args, string source);
	//Parse configuration updates from disk file
	static bool ParseConfiguration(string filename);
	//Convert command line arguments into string vector for convenience
	static vector<string> ConvertToConfiguration(int argc, char * argv[]);

	//Define all the available configurations (including log files) & assign default values
	static void InitConfiguration();
	//Value validation for some configurations
	static void ValidateConfiguration();
	//Print all the configuration values to log file `file_config`
	static void PrintConfiguration();

	//Print help information in file "help.md"
	static void Help();

public:

	//enum type for all the available MAC protocol
	typedef enum EnumMacProtocolScheme
	{
		_smac = 1,
		_hdc = 2
	} EnumMacProtocolScheme;

	//Enum type for all the available routing protocol
	typedef enum EnumRoutingProtocolScheme
	{
		_epidemic = 1,
		_prophet = 2,
		_xhar = 3,
		_pferry = 4
	} EnumRoutingProtocolScheme;

	//Enum type for hotspot selection scheme
	typedef enum EnumHotspotSelectScheme
	{
		_skip = 0,
		_original = 1,  //HAR
		_improved = 2,  //iHAR
		_merge = 3  //with hotspot merge (unpublished)
	} EnumHotspotSelectScheme;
	
	//Enum type for queue management scheme
	typedef enum EnumQueueScheme
	{
		_fifo = 1,   //可发送配额有限时，优先从头部发送
		_lifo = 2   //可发送配额有限时，优先从尾部发送
	} EnumQueueScheme;

	
	/** Public functions for configuraiton access & update **/

	//Update certain configuration (specified by <group> & <keyword>) with given <value>, 
	//without console feedback if <silence>
	template <class T>
	static void updateConfiguration(string group, string keyword, T value, bool silence)
	{
		if( !has(group, keyword) )
			throw string("CConfiguration::updateConfiguration(): Cannot find configuration with keyword \"" + keyword + "\" in group \"" + group + "\".");

		stringstream echo;
		echo << "Configuration \"" << group << "." << keyword << "\" is updated ";
		pair<void*, EnumType> pairConfig = getConfiguration(group, keyword);
		void *pVoid = pairConfig.first;
		EnumType type = pairConfig.second;
		if( T *pT = CParseHelper::TryCast<T>(pVoid) )
		{
			echo << "from " << *pT;
			*pT = value;
			echo << " to " << *pT << ".";
			if( !silence )
				CPrintHelper::PrintDetail(echo.str(), 0);
		}
		else
			throw string("CConfiguration::updateConfiguration(): Cannot convert configuration \"" + keyword + "\" from " + typeid( pVoid ).name() + " to " + typeid( T ).name() + ".");
	}

	template <class T>
	static void updateConfiguration(string group, string keyword, T value)
	{
		updateConfiguration(group, keyword, value, false);
	}
		
	//Get configuration value by <group> & <keyword>
	template <class T>
	static T getConfiguration(string group, string keyword)
	{
		pair<void*, EnumType> pairConfig = getConfiguration(group, keyword);
		void *pVoid = pairConfig.first;
		EnumType type = pairConfig.second;
		if( T *pT = CParseHelper::TryCast<T>(pVoid) )
		{
			// e.g. initialize enum type with int
			return T(*pT);
		}
		else
			throw string("CConfiguration::getConfiguration(): Cannot convert configuration \"" + keyword + "\" from " + typeid( pVoid ).name() + " to " + typeid( T ).name() + ".");
	}
	
	static void test();


};

#endif // __CONFIGURATION_H__

using config = CConfiguration;
template <class T> T (*getConfig)(string,string) = CConfiguration::getConfiguration;
template <class T> void (*updateConfig)( string, string, T) = CConfiguration::updateConfiguration;
