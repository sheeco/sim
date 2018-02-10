#pragma once

#ifndef __FILE_HELPER_H__
#define __FILE_HELPER_H__

//#include "Hotspot.h"
#include "Helper.h"
#include "Coordinate.h"

//包含用于文件相关操作的函数的类
class CFileHelper :
	virtual public CHelper
{
public:

	static bool IfExists(string filename);
	static bool IsEmpty(string filename);
	static bool Rename(string oldname, string newname);
	static bool SetHidden(string filename);
	static bool UnsetHidden(string filename);

	// Return empty vector if dir does not exist
	static vector<string> ListDirectory(string pathDir);
	static vector<string> FilterByExtension(vector<string> filenames, string extension);

	static void test();

	//static CPosition* findPositionByID(vector<CPosition *> positions, int ID);

	////将hotspot的时间、坐标和cover数保存到文件
	//static void writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail);
	//static void writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail);
	//static void writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot);
	
	//从hotspot_detail.log获取hotspot信息，构造hotspot，需要传入所需的positions
	//static vector<CHotspot *> getHotspotFromFile(char *filename, int time, vector<CPosition *> positions);
	//static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

#endif // __config.log.FILE_HELPER_H__
