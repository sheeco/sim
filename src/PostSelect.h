#pragma once

#ifndef __POST_SELECT_H__
#define __POST_SELECT_H__

#include "Hotspot.h"
#include "Process.h"


//HAR中的后续选取算法
class CPostSelect : 
	virtual public CAlgorithm
{
private:

	double maxRatio;
	vector<int> idNodes;
	vector<int> coveredNodes;
	vector<int> coveredPositions;
	vector<CHotspot*> unselectedHotspots;  //候选集合，使用贪婪选取的输出初始化，返回时值为PostSelect中未被选中的hotspot集合
	vector<CHotspot*> selectedHotspots;  //PostSelect选中的hotspot集合

	static double ALPHA;

	/** 辅助函数 **/
	double getRatioForHotspot(CHotspot *hotspot) const;
	void updateForNewlySelectedHotspot(CHotspot *hotspot);
	vector<int> findLostNodes();
	CHotspot* findBestHotspotForNode(int inode);

	//检查选取的hotspot集合是否能覆盖所有node
	bool verifyCompleted();

	CPostSelect(vector<CHotspot *> selectedHotspots, vector<int> idNodes);
	~CPostSelect(){};

	void PostSelect();

public:

	static void Init();
	//执行hotspot选取，返回得到的hotspot集合
	static void PostSelect(vector<CHotspot *> &selectedHotspots, vector<CHotspot *> &unselectedHotspots, vector<int> idNodes);
};

#endif // __POST_SELECT_H__
