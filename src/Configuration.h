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
	static string PATH_TRACE;


private:

	typedef	struct _FIELD_CONFIGURATION
	{
		_TYPE_FIELD type;
		void * dstAttr;  //�����������ֵ���õ�ַָ��ı���
		int defaultValue;  //���� type == _none ʱ��������ʽָ���˱�����ֵ��һ��Ϊö���ͣ������� _bool /_int / _double /string ��������ʽָ����Ĭ��Ϊ 0 ��մ�
	} _FIELD_CONFIGURATION;

	//string keyword;  //�ؼ��֣��������Ĺؼ����� "-sink"�����������Ĺؼ����� "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //�ùؼ��ֺ󸽴���ֵ��

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

	//���ùؼ����Ƿ����
	static bool ifExists(string keyword);
	//�������� 2 ������
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
