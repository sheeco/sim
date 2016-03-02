#pragma once

#include "SortHelper.h"
#include "Algorithm.h"

extern double GAMMA;
extern int startTimeForHotspotSelection;

extern string INFO_LOG;


//HAR中原始的贪婪hotspot选取方法的包装
class CHotspotSelect :
	public CAlgorithm
{
private:

	static vector<CHotspot *> copy_hotspotCandidates;  //来自CHotspot::hotspotCandidates，贪婪选取过程中会修改hotspot的信息
	static vector<CPosition *> uncoveredPositions;  //保存尚未被cover的position
	static vector<CHotspot *> unselectedHotspots;  //未被选中的hotspot集合
	static vector<CHotspot *> hotspotsAboveAverage;  //ratio高于1/2的hotspot集合
	static vector<CHotspot *> selectedHotspots;  //选中的hotspot集合，即整个贪婪算法的输出
	
	//检查是否已完成选取得到合法的解
	static inline bool isValid()
	{
		if( (! selectedHotspots.empty()) && uncoveredPositions.empty() )
			return true;
		else
			return false;
	}
	//在构建候选热点之后调用
	static void updateHotspotCandidates();
	//更新hotspotsAboveAverage，在每一次迭代后调用
	static void updateStatus();


public:

	CHotspotSelect(){};

	~CHotspotSelect(){};

	//从文件中读取所有节点的当前位置，加入position列表（append）
	//由main函数，在每个地理位置信息收集时隙上调用
	static void CollectNewPositions(int time);

	//根据到目前为止的所有position记录，构建候选hotspot列表
	//由main函数，在每个hotspot更新时隙上调用
	static void BuildCandidateHotspots(int time);

	//执行贪婪选取过程，返回选取结果
	//注意：返回的信息将在CHotspotSelect类析构之后失效，应及时保存
	static void GreedySelect(int time);

	//merge-HAR: 
	//执行热点归并过程，归并得到的新热点直接替换旧热点放在copy_hotspotCandidates中，用于执行后续的原贪婪选取算法
	//注意：对于每一个旧热点，为其遍历寻找最佳归并；已经被归并过的热点不再参与其他归并；
	static void MergeHotspots(int time);

};

