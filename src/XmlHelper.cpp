#include "XmlHelper.h"
#include "../lib/xml/tinyxml2.h"
//#import <msxml.dll>
//using namespace MSXML;

//using System;
//using System.Collections.Generic;
//using System.Text;
//using System.Data;
//using System.Threading;
//using LKTOOL;
//using LKETOOL;
//using WSS = CXX201WS.WSS;
//using System.Xml;        //����������

using namespace tinyxml2;


CXmlHelper::CXmlHelper()
{
}

CXmlHelper::~CXmlHelper()
{
}

bool CXmlHelper::CreateXml(string XmlFile)
{
	//XML�ļ���·��
    string filePath = "demoTinyXml2.xml";
    //xml�ĵ�
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
	if( pDoc == nullptr ) 
	{
        return false;
    }
    //xml����
    XMLDeclaration *pDec = pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    if( pDec == nullptr ) 
	{
        return false;
    }

    pDoc->LinkEndChild(pDec);
    //���ڵ�plist
    XMLElement *plistElement = pDoc->NewElement("plist");
    plistElement->SetAttribute("version", "1.0");//���ڵ���������
	//��Ԫ�����ӵ��ĵ���Ŀ¼��
    pDoc->LinkEndChild(plistElement);
        //�ڵ�dict
        XMLElement *dictElement = pDoc->NewElement("dict");
		//��dict���ӵ�plist��
        plistElement->LinkEndChild(dictElement);
            //�ڵ�key
            XMLElement *keyElement = pDoc->NewElement("key");
            keyElement->LinkEndChild(pDoc->NewText("keyText"));//���ڵ�����ֵ
			//��key���ӵ�dict��
            dictElement->LinkEndChild(keyElement);
            //�ڵ�string
            XMLElement *stringElement = pDoc->NewElement("string");
            stringElement->LinkEndChild(pDoc->NewText("stringText"));//���ڵ�����ֵ
            //��string���ӵ�dict��
			dictElement->LinkEndChild(stringElement);
            //�ڵ�array
            XMLElement *arrayElemet = pDoc->NewElement("array");
			//��array���ӵ�dict��
            dictElement->LinkEndChild(arrayElemet);
				//����string�ڵ�
                for (int i = 0; i<3; i++) 
				{
                    XMLElement *strEle = pDoc->NewElement("string");
                    strEle->LinkEndChild(pDoc->NewText("icon"));
					//��string���ӵ�plist��
                    arrayElemet->LinkEndChild(strEle);
                }

    //�����ļ���·����
    pDoc->SaveFile(filePath.c_str());
    //pDoc->Print();//��ӡ
	// ������ʱ��tinyxml ��������Ѿ����ӽ����Ľڵ�����ͷţ����Բ���Ҫ�ֶ� delete
	// ����� XmlDocument ������Դ�� new������Ҫ�� XmlDocument ����ִ�� delete
    delete pDoc;
	return true;
}

bool CXmlHelper::ParseXml(string XmlFile)
{
	//xml�ļ�·��
    std::string filePath = "demoTinyXml2.xml";
	//xmlDoc
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
    pDoc->LoadFile(filePath.c_str());
    //�õ����ڵ�
    XMLElement *rootEle = pDoc->RootElement();
    //��ӡ�ڵ��ֵ
    //CCLog("%s",rootEle->GetText());

    //�ڵ�ĵ�һ������
    const XMLAttribute *attribute = rootEle->FirstAttribute();
    //��ӡ���Ե����ֺ�ֵ
    //CCLog("%s %s",attribute->Name(),attribute->Value());    
    
	//���ҽڵ������ֵ
    float value = 0.1f;
    rootEle->QueryFloatAttribute("version", &value);
    //CCLog("%f",value);
    
	//���ýڵ�����ֵ
    rootEle->SetAttribute("version", 1.4);
    
	//���ڵ�ĵ�һ���ӽڵ� dict
    XMLElement *dictEle = rootEle->FirstChildElement();
    //dict�µ��ӽڵ� key
    XMLElement *keyEle = dictEle->FirstChildElement();
    //��ӡkey�ڵ��ֵ
    //CCLog("%s,%s",keyEle->Name(),keyEle->GetText());
    //key�ڵ��next�ڵ� string
    XMLElement *stringEle = keyEle->NextSiblingElement();
    //CCLog("%s,%s",stringEle->Name(),stringEle->GetText());
    //string�ڵ���ӽڵ�
    XMLElement *nulXmlEle = stringEle->FirstChildElement();
	if ( nulXmlEle == nullptr ) 
	{
        //CCLog("string����û���ӵ���");
    }

    //����xml
    pDoc->SaveFile(filePath.c_str());	
	return true;
}
