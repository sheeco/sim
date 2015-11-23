#pragma once
#include "Preprocessor.h"

using namespace std;

extern bool DO_MERGE_HAR;
extern double GAMA;
extern int startTimeForHotspotSelection;


extern string logInfo;

//HAR中原始的贪婪hotspot选取方法的包装
class CGreedySelection
{
private:
	vector<CHotspot *> copy_hotspotCandidates;  //来自CHotspot::hotspotCandidates，贪婪选取过程中会修改hotspot的信息
	vector<CPosition *> uncoveredPositions;  //保存尚未被cover的position
	vector<CHotspot *> unselectedHotspots;  //未被选中的hotspot集合
	vector<CHotspot *> hotspotsAboveAverage;  //ratio高于1/2的hotspot集合
	vector<CHotspot *> selectedHotspots;  //选中的hotspot集合，即整个贪婪算法的输出
	
	//检查是否已完成选取得到合法的解
	inline bool isValid()
	{
		if( (! selectedHotspots.empty()) && uncoveredPositions.empty() )
			return true;
		else
			return false;
	}
	//更新hotspotsAboveAverage，在每一次迭代后调用
	void UpdateStatus();

public:

	CGreedySelection();
	//CGreedySelection(vector<CHotspot *> &hotspotsCandidates);
	////merge_HAR: 
	//CGreedySelection(vector<CHotspot *> &hotspotsCandidates, vector<CHotspot *> &oldSelectedHotspots);

	~CGreedySelection(void);

	//执行贪婪选取过程，返回选取结果
	//注意：返回的信息将在CGreedySelection类析构之后失效，应及时保存
	void GreedySelect(int time);

	//merge-HAR: 
	//执行热点归并过程，归并得到的新热点直接替换旧热点放在copy_hotspotCandidates中，用于执行后续的原贪婪选取算法
	//注意：对于每一个旧热点，为其遍历寻找最佳归并；已经被归并过的热点不再参与其他归并；
	void mergeHotspots(int time);

	vector<CHotspot *> getSelectedHotspots()
	{
		return selectedHotspots;
	}

	int getCost();
	string toString();
};

