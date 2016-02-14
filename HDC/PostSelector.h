#pragma once

#include "Hotspot.h"
#include "Position.h"
#include "Algorithm.h"

using namespace std;

extern double ALPHA;
extern int MAX_NUM_HOTSPOT;
extern int NUM_NODE;

extern vector<CPosition*> g_tmpPositions;

extern int g_old_nPositions;
extern int g_old_nHotspots;
extern string INFO_LOG;

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

	/** 辅助函数 **/
	double getRatioForHotspot(CHotspot *hotspot) const;
	void includeHotspots(CHotspot *hotspot);
	void findLostNodes();
	CHotspot* findBestHotspotForNode(int inode);

	//检查选取的hotspot集合是否能覆盖所有node
	bool verifyCompleted();

public:
	CPostSelector(vector<CHotspot *> hotspotCandidates);
	~CPostSelector(void);

	//选取hotspot完成后，将被覆盖的每一个position分配到唯一一个hotspot
	vector<CHotspot *> assignPositionsToHotspots(vector<CHotspot *> hotspots) const;	
	//执行hotspot选取，返回得到的hotspot集合
	vector<CHotspot *> PostSelect(int currentTime);
	int getNCoveredPositions() const;
	
};

