#pragma once

#include "Hotspot.h"
#include "Position.h"
#include "Preprocessor.h"

using namespace std;

extern double ALPHA;
extern int NUM_NODE;

extern int g_nPositions;
extern int g_nHotspotCandidates;
//extern int** g_coverMatrix;
//extern int* g_degreeForPositions;
//extern int* g_degreeForHotspots;
extern vector<CHotspot*> g_hotspotCandidates;
extern vector<CHotspot*> g_selectedHotspots;
extern vector<CPosition*> g_positions;
extern vector<CPosition*> g_tmpPositions;

extern int g_old_nPositions;
extern int g_old_nHotspots;

class CPostSelector
{
private:
	unsigned int maxCoverNum;
	vector<int> coveredNodes;
	vector<int> lostNodes;
	vector<int> coveredPositions;
	vector<CHotspot *> hotspotCandidates;
	vector<CHotspot *> selectedHotspots;

	/** �������� **/
	double getRatioForHotspot(CHotspot *hotspot);
	void includeHotspots(CHotspot *hotspot);
	void findLostNodes();
	CHotspot* findMaxCoverHotspotForNode(int inode);


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

