#pragma once

#include "Hotspot.h"
#include "Node.h"

extern int NUM_NODE;
extern int startTimeForHotspotSelection;

//���������ļ������ĺ�������
class CFileParser
{
public:
	//����nodeID��time�������ļ��н���������Ϣ����pos
	static void getPositionFromFile(int nodeID, int time, double &x, double &y);
	static CPosition* findPositionByID(vector<CPosition *> positions, int ID);

	//��hotspot��ʱ�䡢�����cover�����浽�ļ�
	static void writeHotspotToFile(char *filename, CHotspot *hotspot, bool withDetail);
	static void writeHotspotToFile(char *filename, vector<CHotspot *> list_hotspot, bool withDetail);
	static void writeHotspotStatisics(int time, char *filename, vector<CHotspot *> list_hotspot);
	//��hotspot_detail.txt��ȡhotspot��Ϣ������hotspot����Ҫ���������positions
	static vector<CHotspot *> getHotspotFromFile(char *filename, int time, vector<CPosition *> positions);
};

