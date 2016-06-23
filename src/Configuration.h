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
	friend class CRunHelper;


public:

	typedef enum _TYPE_FIELD
	{
		_none,  //�� keyword �����κβ���ʱ���� values ��ѹ��һ�� _none �͵� _TYPE_VALUE������ defaultValue ��ֵ���� dstAttr
		_int,  //�������͸�ʽ����
		_double,  //���ո����͸�ʽ����
		_bool  //���ղ����ͽ������ؼ����� KEYWORD_TRUE, KEYWORD_FALSE ָ��
	} _TYPE_FIELD;


private:

	typedef	struct _FIELD_CONFIGURATION
	{
		_TYPE_FIELD type;
		void * dstAttr;  //�����������ֵ���õ�ַָ��ı���
		int defaultValue;  //���� type == _none ʱ��������ʽָ���˱�����ֵ��һ��Ϊö���ͣ������� _int / _double / _bool ��������ʽָ����Ĭ��Ϊ 0
	} _FIELD_CONFIGURATION;

	//string keyword;  //�ؼ��֣��������Ĺؼ����� "-sink"�����������Ĺؼ����� "--prophet"
	//vector<_FIELD_CONFIGURATION> fields;  //�ùؼ��ֺ󸽴���ֵ��

	static map<string, pair<string, vector<_FIELD_CONFIGURATION> > > configurations;


	CConfiguration() {};
	//CConfiguration(string keyword);
	~CConfiguration() {};

	//void addValue(_TYPE_VALUE type, void * dstAttr, int defaultValue);
	void setFieldValue();


protected:

	//���ùؼ����Ƿ����
	static bool ifExists(string keyword);
	//�������� 2 ������
	static bool AddConfiguration(string keyword, void * dstAttr, int value, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type, void * dstAttr, string description);
	static bool AddConfiguration(string keyword, _TYPE_FIELD type_1, void * dstAttr_1, _TYPE_FIELD type_2, void * dstAttr_2, string description);

	static bool ParseConfiguration(int argc, char * argv[], string description);
	static bool ParseConfiguration(vector<string> args, string description);
	static bool ParseConfiguration(string filename);
};

#endif // __CONFIGURATION_H__
