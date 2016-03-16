#pragma once

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
	vector<CHotspot *> hotspotCandidates;
	vector<CHotspot *> selectedHotspots;

	/** �������� **/
	double getRatioForHotspot(CHotspot *hotspot) const;
	void includeHotspots(CHotspot *hotspot);
	void findLostNodes();
	CHotspot* findBestHotspotForNode(int inode);

	//���ѡȡ��hotspot�����Ƿ��ܸ�������node
	bool verifyCompleted();


public:

	static double ALPHA;  //ratio for post selection

	CPostSelect(vector<CHotspot *> hotspotCandidates);
	~CPostSelect(){};

	//ѡȡhotspot��ɺ󣬽������ǵ�ÿһ��position���䵽Ψһһ��hotspot
	vector<CHotspot *> assignPositionsToHotspots(vector<CHotspot *> hotspots) const;	
	//ִ��hotspotѡȡ�����صõ���hotspot����
	vector<CHotspot *> PostSelect(int currentTime);
	int getNCoveredPositions() const;
	
};
