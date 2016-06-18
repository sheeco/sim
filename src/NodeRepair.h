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
	vector<int> poorNodes;  //����cover�������node
	vector<CHotspot*> &unselectedHotspots;  //δ��ѡ�е�hotspot����
	vector<CHotspot*> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����

	//hotspot�����и��ǵ�����ĳһ�ڵ��position��Ŀ
	int countForNode(vector<CHotspot *> hotspots, int n) const;
	CHotspot* findMaxCoverHotspotForNode(int inode);


public:

	CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> &unselectedHotspots);
	~CNodeRepair(){};
	vector<CHotspot *> RepairPoorNodes(int time);

};

#endif // __NODE_REPAIR_H__
