#pragma once

#ifndef __FILE_HELPER_H__
#define __FILE_HELPER_H__

//#include "Hotspot.h"
#include "Helper.h"
#include "Coordinate.h"

//���������ļ���ز����ĺ�������
class CFileHelper :
	virtual public CHelper
{
public:

	static bool IfExists(string filename);
	static bool IsEmpty(string filename);
	//static bool Rename(string oldname, string newname);
	//static bool SetHidden(string filename);
	//static bool UnsetHidden(string filename);

	static bool IsDirectory(string strPath);
	static bool IsFile(string strPath);
	// Return empty vector if dir does not exist
	static vector<string> ListDirectory(string pathDir);
	static vector<string> FilterByExtension(vector<string> filenames, string extension);
	// Return <dir, filename>
	static pair<string, string> SplitPath(string path);
	// Return <name, extension>
	static pair<string, string> SplitFilename(string filename);
	static void test();

	//static CPosition* findPositionByID(vector<CPosition *> positions, int ID);

	////��hotspot��ʱ�䡢�����cover�����浽�ļ�
	//static void writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail);
	//static void writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail);
	//static void writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot);
	
	//��hotspot_detail.log��ȡhotspot��Ϣ������hotspot����Ҫ���������positions
	//static vector<CHotspot *> getHotspotFromFile(char *filename, int time, vector<CPosition *> positions);
	//static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

#endif // __config.log.FILE_HELPER_H__
