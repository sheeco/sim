#pragma once

#include "Helper.h"
#include "Global.h"

class CXmlHelper : 
	public CHelper
{
public:

	CXmlHelper();
	~CXmlHelper();

	/*!
	*  /brief 创建xml文件。
	*
	*  /param XmlFile xml文件全路径。
	*  /return 是否成功。true为成功，false表示失败。
	*/
	bool CreateXml(string XmlFile);
	bool ParseXml(string XmlFile);

};

