#pragma once

#include "Algorithm.h"
#include "Hotspot.h"


class CNodeRepair : 
	public CAlgorithm
{
private:

	int time;
	vector<int> poorNodes;  //保存cover数不足的node
	vector<CHotspot*> &unselectedHotspots;  //未被选中的hotspot集合
	vector<CHotspot*> selectedHotspots;  //选中的hotspot集合，即整个贪婪算法的输出

	//hotspot集合中覆盖的属于某一节点的position数目
	int countForNode(vector<CHotspot *> hotspots, int n) const;
	CHotspot* findMaxCoverHotspotForNode(int inode);


public:

	CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> &unselectedHotspots);
	~CNodeRepair(){};
	vector<CHotspot *> RepairPoorNodes(int time);

};

