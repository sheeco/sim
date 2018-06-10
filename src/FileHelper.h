#pragma once

#ifndef __FILE_HELPER_H__
#define __FILE_HELPER_H__

#include "Process.h"
#include "Entity.h"

//文件操作的辅助类
class CFileHelper :
	virtual public CHelper
{
public:

	static bool IfExists(string filename);
	static bool IsEmpty(string filename);
	//static bool Rename(string oldname, string newname);
	static bool SetHidden(string filename);
	static bool UnsetHidden(string filename);

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

	////将hotspot的时间、坐标和cover数保存到文件
	//static void writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail);
	//static void writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail);
	//static void writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot);
	
	//从hotspot_detail.log获取hotspot信息，构造hotspot，需要传入所需的positions
	//static vector<CHotspot *> getHotspotFromFile(char *filename, int time, vector<CPosition *> positions);
	//static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

#endif // __config.log.FILE_HELPER_H__
