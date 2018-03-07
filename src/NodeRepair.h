#pragma once

#ifndef __NODE_REPAIR_H__
#define __NODE_REPAIR_H__

#include "Algorithm.h"
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
	static int LIFETIME_POSITION;

	static void Init()
	{
		if(EQUAL( LAMBDA, INVALID))
			LAMBDA = getConfig<double>("ihs", "lambda");
		if(LIFETIME_POSITION == INVALID)
			LIFETIME_POSITION = getConfig<int>("ihs", "lifetime_position");
	}

	static void Repair(vector<CHotspot *> &selectedHotspots, vector<CHotspot *> &unselectedHotspots, vector<int> idNodes, int now);

};

#endif // __NODE_REPAIR_H__
