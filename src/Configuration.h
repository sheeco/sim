/***********************************************************************************************************************************

������ CConfiguration �� ���в��������࣬Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"
#include "ParseHelper.h"

class CConfiguration :
	virtual public CHelper
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

	//TODO: opt for map definitions 
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
		_smac = 1,
		_hdc = 2
	} EnumMacProtocolScheme;

	typedef enum EnumRoutingProtocolScheme
	{
		_epidemic = 1,
		_prophet = 2,
		_xhar = 3,
		_pferry = 4
	} EnumRoutingProtocolScheme;

	typedef enum EnumHotspotSelectScheme
	{
		_skip = 0,
		_original = 1,
		_improved = 2,
		_merge = 3
	} EnumHotspotSelectScheme;

	//TODO: not in use now
	//typedef enum EnumForwardScheme
	//{
	//	_copy = 1,  //�������ݳɹ��󣬱���������
	//	_dump = 2   //�������ݳɹ���ɾ��������
	//} EnumForwardScheme;

	//typedef enum EnumRelayScheme
	//{
	//	_loose = 1,   //buffer����ʱ�������������������
	//	_selfish = 2   //buffer����ʱ�����ٴ������ڵ��������
	//} EnumRelayScheme;

	typedef enum EnumQueueScheme
	{
		_fifo = 1,   //�ɷ����������ʱ�����ȴ�ͷ������
		_lifo = 2   //�ɷ����������ʱ�����ȴ�β������
	} EnumQueueScheme;


	template <class T>
	static void updateConfiguration(string group, string keyword, T value)
	{
		updateConfiguration(group, keyword, value, false);
	}
		
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
