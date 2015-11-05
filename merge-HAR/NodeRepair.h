#pragma once
#include "Preprocessor.h"

using namespace std;

extern int NUM_NODE;

class CNodeRepair
{
private:
	int time;
	vector<int> poorNodes;  //保存cover数不足的node
	vector<CHotspot*> unselectedHotspots;  //未被选中的hotspot集合
	vector<CHotspot*> selectedHotspots;  //选中的hotspot集合，即整个贪婪算法的输出

	//hotspot集合中覆盖的属于某一节点的position数目
	int countForNode(vector<CHotspot *> hotspots, int n);
	CHotspot* findMaxCoverHotspotForNode(int inode);

public:
	CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> hotspotCandidates, int time);
	~CNodeRepair(void);
	vector<CHotspot *> RepairPoorNodes();
};

