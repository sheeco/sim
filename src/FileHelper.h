#pragma once

//#include "Hotspot.h"
#include "Helper.h"
#include "Coordinate.h"

//���������ļ������ĺ�������
class CFileHelper :
	public CHelper
{
public:

	//����nodeID��time�������ļ��н���������Ϣ����pos
	static bool getLocationFromFile(int nodeID, int time, CCoordinate &location);
	//static CPosition* findPositionByID(vector<CPosition *> positions, int ID);

	////��hotspot��ʱ�䡢�����cover�����浽�ļ�
	//static void writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail);
	//static void writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail);
	//static void writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot);
	
	//��hotspot_detail.log��ȡhotspot��Ϣ������hotspot����Ҫ���������positions
	//static vector<CHotspot *> getHotspotFromFile(char *filename, int time, vector<CPosition *> positions);
	//static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

