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
	vector<int> poorNodes;  //����cover�������node
	vector<CHotspot*> unselectedHotspots;  //δ��ѡ�е�hotspot����
	vector<CHotspot*> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����

	//hotspot�����и��ǵ�����ĳһ�ڵ��position��Ŀ
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
