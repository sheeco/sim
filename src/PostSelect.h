#pragma once

#ifndef __POST_SELECT_H__
#define __POST_SELECT_H__

#include "Hotspot.h"
#include "Position.h"
#include "Algorithm.h"


class CPostSelect : 
	public CAlgorithm
{
private:

	double maxRatio;
	vector<int> coveredNodes;
	vector<int> lostNodes;
	vector<int> coveredPositions;
	vector<CHotspot*> &unselectedHotspots;  //δ��ѡ�е�hotspot����
	vector<CHotspot*> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����
	vector<CHotspot*> hotspotCandidates;  //ѡ�е�hotspot���ϣ�������̰���㷨�����

	/** �������� **/
	double getRatioForHotspot(CHotspot *hotspot) const;
	void includeHotspots(CHotspot *hotspot);
	void findLostNodes();
	CHotspot* findBestHotspotForNode(int inode);

	//���ѡȡ��hotspot�����Ƿ��ܸ�������node
	bool verifyCompleted();


public:

	static double ALPHA;  //ratio for post selection

	CPostSelect(vector<CHotspot *> selectedHotspots, vector<CHotspot *> &unselectedHotspots);
	~CPostSelect(){};

	//ִ��hotspotѡȡ�����صõ���hotspot����
	vector<CHotspot *> PostSelect(int currentTime);
	//int getNCoveredPositions() const;
	
};

#endif // __POST_SELECT_H__
