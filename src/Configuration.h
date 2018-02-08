/***********************************************************************************************************************************

������ CConfiguration �� ���в��������࣬Ӧ�ü̳��������

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
		_none,  //�� keyword �����κβ���ʱ���� values ��ѹ��һ�� _none �͵� _TYPE_VALUE������ defaultValue ��ֵ���� dstAttr
		_bool,  //���ղ����ͽ������ؼ����� KEYWORD_TRUE, KEYWORD_FALSE ָ��
		_int,  //�������͸�ʽ����
		_double,  //���ո����͸�ʽ����
		_string  //�����ַ�������
	} _TYPE_FIELD;


	// TODO: define all the configs here as static attribute


private:

	typedef	struct _FIELD_CONFIGURATION
	{
		_TYPE_FIELD type;
		void * dstAttr;  //�����������ֵ���õ�ַָ��ı���
		void * value;  //value of this configuration
		bool state;  //������ _none ʱ����¼��ѡ��Ŀ�/��
	} _FIELD_CONFIGURATION;

	//string keyword;  //�ؼ��֣��������Ĺؼ����� "-sink"�����������Ĺؼ����� "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //�ùؼ��ֺ󸽴���ֵ��

	static map<string, pair<string, vector<_FIELD_CONFIGURATION> > > configurations;
	static string KEYWORD_HELP;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};


	//���ùؼ����Ƿ����
	static bool ifExists(string keyword);
	static pair<string, vector<_FIELD_CONFIGURATION>> getConfiguration(string keyword);
	//�������� 2 ������
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
