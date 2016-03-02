#pragma once

#include "Algorithm.h"
#include "Hotspot.h"

extern int NUM_NODE;
extern double LAMBDA;


class CNodeRepair : 
	public CAlgorithm
{
private:

	int time;
	vector<int> poorNodes;  //����cover�������node
	vector<CHotspot*> unselectedHotspots;  //δ��ѡ�е�hotspot����
	vector<CHotspot*> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����

	//hotspot�����и��ǵ�����ĳһ�ڵ��position��Ŀ
	int countForNode(vector<CHotspot *> hotspots, int n) const;
	CHotspot* findMaxCoverHotspotForNode(int inode);


public:

	CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> hotspotCandidates, int time);
	~CNodeRepair(){};
	vector<CHotspot *> RepairPoorNodes();

};

