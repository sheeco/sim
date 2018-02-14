/***********************************************************************************************************************************

顶级类 CConfiguration ： 所有参数配置类，应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"
#include "ParseHelper.h"

class CConfiguration
{
	friend class CRunHelper;

	/******************************** Config Const ********************************/

	//template <class T>
	//struct Described
	//{
	//	T value;
	//	string description;

	//	Described(T value, string description) : value(value), description(description)
	//	{
	//	};
	//	Described(T value) : value(value), description("Unknown")
	//	{
	//	};
	//};
	//typedef struct Described Described;

private:
	static map<string, map<string, pair<void*, EnumType>>> configurations;

	static void addGroup(string group);

	static void addConfiguration(string group, string keyword, const type_info& type, void *value);

	static string KEYWORD_HELP;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};


	static bool has(string group);
	static bool has(string group, string keyword);
	
	static pair<void*, EnumType> getConfiguration(string group, string keyword);


protected:

	static bool ParseConfiguration(vector<string> args, string source);
	static bool ParseConfiguration(string filename);
	static vector<string> ConvertToConfiguration(int argc, char * argv[]);

	static void InitConfiguration();
	static void ValidateConfiguration();
	static void PrintConfiguration();

	static void Help();

public:

	typedef enum EnumMacProtocolScheme
	{
		_smac,
		_hdc
	} EnumMacProtocolScheme;

	typedef enum EnumRoutingProtocolScheme
	{
		_xhar,
		_prophet,
	} EnumRoutingProtocolScheme;

	typedef enum EnumHotspotSelectScheme
	{
		_skip,
		_original,
		_improved,
		_merge
	} EnumHotspotSelectScheme;


	typedef enum EnumForwardScheme
	{
		_copy,  //发送数据成功后，保留自身副本
		_dump   //发送数据成功后，删除自身副本
	} EnumForwardScheme;

	typedef enum EnumRelayScheme
	{
		_loose,   //MA buffer已满时，仍允许继续接收数据
		_selfish   //MA buffer已满时，不再从其他节点接收数据
	} EnumRelayScheme;

	typedef enum EnumQueueScheme
	{
		_fifo,   //可发送配额有限时，优先从头部发送
		_lifo   //可发送配额有限时，优先从尾部发送
	} EnumQueueScheme;


	template <class T>
	static void updateConfiguration(string group, string keyword, T value)
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
			echo << " to " << *pT << "." << endl;
			CPrintHelper::PrintDetail(echo.str());
		}
		else
			throw string("CConfiguration::updateConfiguration(): Cannot convert configuration \"" + keyword + "\" from " + typeid( pVoid ).name() + " to " + typeid( T ).name() + ".");
	}

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
