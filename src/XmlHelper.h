#pragma once

#ifndef __XML_HELPER_H__
#define __XML_HELPER_H__

#include "Helper.h"
#include "Global.h"

class CXmlHelper : 
	public CHelper
{
public:

	CXmlHelper();
	~CXmlHelper();

	/*!
	*  /brief ����xml�ļ���
	*
	*  /param XmlFile xml�ļ�ȫ·����
	*  /return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
	*/
	bool CreateXml(string XmlFile);
	bool ParseXml(string XmlFile);

};

#endif // __XML_HELPER_H__
