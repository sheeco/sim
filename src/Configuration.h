/***********************************************************************************************************************************

������ CConfiguration �� ���в��������࣬Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Global.h"
#include "ParseHelper.h"


//����ʵ�����ò����Ķ��塢��ʼ�������ʡ����µ���
//����Э��/������ѡ���㷨���������绷����������̡�����̨�������־�ļ��ȷ���Ĳ�����
//������Ϊ���ò���������ͨ�������ļ�`bin/default.config`�������в������޸�
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
		_fifo = 1,   //�ɷ����������ʱ�����ȴ�ͷ������
		_lifo = 2   //�ɷ����������ʱ�����ȴ�β������
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
