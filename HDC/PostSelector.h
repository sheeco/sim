#pragma once

#include "Hotspot.h"
#include "Position.h"
#include "Preprocessor.h"
#include "Algorithm.h"

using namespace std;

extern double ALPHA;
extern int MAX_NUM_HOTSPOT;
extern int NUM_NODE;

extern vector<CPosition*> g_tmpPositions;

extern int g_old_nPositions;
extern int g_old_nHotspots;
extern string logInfo;

class CPostSelector : 
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
	double getRatioForHotspot(CHotspot *hotspot);
	void includeHotspots(CHotspot *hotspot);
	void findLostNodes();
	CHotspot* findBestHotspotForNode(int inode);

	//���ѡȡ��hotspot�����Ƿ��ܸ�������node
	bool verifyCompleted();

public:
	CPostSelector(vector<CHotspot *> hotspotCandidates);
	~CPostSelector(void);

	//ѡȡhotspot��ɺ󣬽������ǵ�ÿһ��position���䵽Ψһһ��hotspot
	vector<CHotspot *> assignPositionsToHotspots(vector<CHotspot *> hotspots);	
	//ִ��hotspotѡȡ�����صõ���hotspot����
	vector<CHotspot *> PostSelect(int currentTime);
	int getNCoveredPositions();
	
};

