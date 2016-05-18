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
	vector<CHotspot*> &unselectedHotspots;  //未被选中的hotspot集合
	vector<CHotspot*> selectedHotspots;  //选中的hotspot集合，即整个贪婪算法的输出
	vector<CHotspot*> hotspotCandidates;  //选中的hotspot集合，即整个贪婪算法的输出

	/** 辅助函数 **/
	double getRatioForHotspot(CHotspot *hotspot) const;
	void includeHotspots(CHotspot *hotspot);
	void findLostNodes();
	CHotspot* findBestHotspotForNode(int inode);

	//检查选取的hotspot集合是否能覆盖所有node
	bool verifyCompleted();


public:

	static double ALPHA;  //ratio for post selection

	CPostSelect(vector<CHotspot *> selectedHotspots, vector<CHotspot *> &unselectedHotspots);
	~CPostSelect(){};

	//执行hotspot选取，返回得到的hotspot集合
	vector<CHotspot *> PostSelect(int currentTime);
	//int getNCoveredPositions() const;
	
};

#endif // __POST_SELECT_H__
