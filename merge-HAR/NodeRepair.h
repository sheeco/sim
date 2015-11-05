#pragma once
#include "Preprocessor.h"

using namespace std;

extern int NUM_NODE;

class CNodeRepair
{
private:
	int time;
	vector<int> poorNodes;  //����cover�������node
	vector<CHotspot*> unselectedHotspots;  //δ��ѡ�е�hotspot����
	vector<CHotspot*> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����

	//hotspot�����и��ǵ�����ĳһ�ڵ��position��Ŀ
	int countForNode(vector<CHotspot *> hotspots, int n);
	CHotspot* findMaxCoverHotspotForNode(int inode);

public:
	CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> hotspotCandidates, int time);
	~CNodeRepair(void);
	vector<CHotspot *> RepairPoorNodes();
};

