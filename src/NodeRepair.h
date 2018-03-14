#pragma once

#ifndef __NODE_REPAIR_H__
#define __NODE_REPAIR_H__

#include "Process.h"
#include "Hotspot.h"


class CNodeRepair : 
	virtual public CAlgorithm
{
private:

	int time;
	vector<int> idNodes;
	vector<int> poorNodes;  //保存cover数不足的node
	vector<CHotspot*> unselectedHotspots;  //未被选中的hotspot集合
	vector<CHotspot*> selectedHotspots;  //选中的hotspot集合，即整个贪婪算法的输出

	//hotspot集合中覆盖的属于某一节点的position数目
	int countForNode(vector<CHotspot *> hotspots, int n) const;
	CHotspot* findMaxCoverHotspotForNode(int inode);


	CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> unselectedHotspots, vector<int> idNodes, int now)
		: selectedHotspots(selectedHotspots), unselectedHotspots(unselectedHotspots), idNodes(idNodes), time(now)
	{
		poorNodes = idNodes;
	}
	~CNodeRepair()
	{
	};
	void Repair();


public:

	static double LAMBDA;

	static void Init();

	static void Repair(vector<CHotspot *> &selectedHotspots, vector<CHotspot *> &unselectedHotspots, vector<int> idNodes, int now);

};

#endif // __NODE_REPAIR_H__
