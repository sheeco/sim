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
//using System.Xml;        //添加这个引用

using namespace tinyxml2;


CXmlHelper::CXmlHelper()
{
}

CXmlHelper::~CXmlHelper()
{
}

bool CXmlHelper::CreateXml(string XmlFile)
{
	//XML文件的路径
    string filePath = "demoTinyXml2.xml";
    //xml文档
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
	if( pDoc == nullptr ) 
	{
        return false;
    }
    //xml声明
    XMLDeclaration *pDec = pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    if( pDec == nullptr ) 
	{
        return false;
    }

    pDoc->LinkEndChild(pDec);
    //根节点plist
    XMLElement *plistElement = pDoc->NewElement("plist");
    plistElement->SetAttribute("version", "1.0");//给节点设置属性
	//将元素连接到文档根目录下
    pDoc->LinkEndChild(plistElement);
        //节点dict
        XMLElement *dictElement = pDoc->NewElement("dict");
		//将dict连接到plist下
        plistElement->LinkEndChild(dictElement);
            //节点key
            XMLElement *keyElement = pDoc->NewElement("key");
            keyElement->LinkEndChild(pDoc->NewText("keyText"));//给节点设置值
			//将key连接到dict下
            dictElement->LinkEndChild(keyElement);
            //节点string
            XMLElement *stringElement = pDoc->NewElement("string");
            stringElement->LinkEndChild(pDoc->NewText("stringText"));//给节点设置值
            //将string连接到dict下
			dictElement->LinkEndChild(stringElement);
            //节点array
            XMLElement *arrayElemet = pDoc->NewElement("array");
			//将array连接到dict下
            dictElement->LinkEndChild(arrayElemet);
				//三个string节点
                for (int i = 0; i<3; i++) 
				{
                    XMLElement *strEle = pDoc->NewElement("string");
                    strEle->LinkEndChild(pDoc->NewText("icon"));
					//将string连接到plist下
                    arrayElemet->LinkEndChild(strEle);
                }

    //保存文件（路径）
    pDoc->SaveFile(filePath.c_str());
    //pDoc->Print();//打印
	// 当析构时，tinyxml 会对所有已经连接进来的节点进行释放，所以不需要手动 delete
	// 但如果 XmlDocument 对象来源于 new，则需要对 XmlDocument 对象执行 delete
    delete pDoc;
	return true;
}

bool CXmlHelper::ParseXml(string XmlFile)
{
	//xml文件路径
    std::string filePath = "demoTinyXml2.xml";
	//xmlDoc
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
    pDoc->LoadFile(filePath.c_str());
    //得到根节点
    XMLElement *rootEle = pDoc->RootElement();
    //打印节点的值
    //CCLog("%s",rootEle->GetText());

    //节点的第一个属性
    const XMLAttribute *attribute = rootEle->FirstAttribute();
    //打印属性的名字和值
    //CCLog("%s %s",attribute->Name(),attribute->Value());    
    
	//查找节点的属性值
    float value = 0.1f;
    rootEle->QueryFloatAttribute("version", &value);
    //CCLog("%f",value);
    
	//设置节点属性值
    rootEle->SetAttribute("version", 1.4);
    
	//根节点的第一个子节点 dict
    XMLElement *dictEle = rootEle->FirstChildElement();
    //dict下的子节点 key
    XMLElement *keyEle = dictEle->FirstChildElement();
    //打印key节点的值
    //CCLog("%s,%s",keyEle->Name(),keyEle->GetText());
    //key节点的next节点 string
    XMLElement *stringEle = keyEle->NextSiblingElement();
    //CCLog("%s,%s",stringEle->Name(),stringEle->GetText());
    //string节点的子节点
    XMLElement *nulXmlEle = stringEle->FirstChildElement();
	if ( nulXmlEle == nullptr ) 
	{
        //CCLog("string下面没有子点了");
    }

    //保存xml
    pDoc->SaveFile(filePath.c_str());	
	return true;
}
