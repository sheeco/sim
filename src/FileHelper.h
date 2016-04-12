#pragma once

//#include "Hotspot.h"
#include "Helper.h"
#include "Coordinate.h"
#include "CTrace.h"

//包含用于文件解析的函数的类
class CFileHelper :
	public CHelper
{
public:

	static bool IfExists(string filename);
	static bool IsEmpty(string filename);
	static bool Rename(string oldname, string newname);
	static bool SetHidden(string filename);
	static bool UnsetHidden(string filename);

	//static CPosition* findPositionByID(vector<CPosition *> positions, int ID);

	////将hotspot的时间、坐标和cover数保存到文件
	//static void writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail);
	//static void writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail);
	//static void writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot);
	
	//从hotspot_detail.log获取hotspot信息，构造hotspot，需要传入所需的positions
	//static vector<CHotspot *> getHotspotFromFile(char *filename, int time, vector<CPosition *> positions);
	//static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

