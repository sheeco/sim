#pragma once

#include "SortHelper.h"
#include "Algorithm.h"


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
	
	//用于计算最终取出的热点总数的历史平均值
	static int SUM_HOTSPOT_COST;
	static int COUNT_HOTSPOT_COST;
	//用于计算最终选取出的热点集合中，merge和old热点的比例的历史平均值
	static double SUM_PERCENT_MERGE;
	static int COUNT_PERCENT_MERGE;
	static double SUM_PERCENT_OLD;
	static int COUNT_PERCENT_OLD;
	//用于计算热点前后相似度的历史平均值
	static double SUM_SIMILARITY_RATIO;
	static int COUNT_SIMILARITY_RATIO;


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

	//根据到目前为止的所有position记录，构建候选hotspot列表
	//由main函数，在每个hotspot更新时隙上调用
	static void BuildCandidateHotspots(int currentTime);

	//执行贪婪选取过程，返回选取结果
	//注意：返回的信息将在CHotspotSelect类析构之后失效，应及时保存
	static void GreedySelect(int currentTime);

	//merge-HAR: 
	//执行热点归并过程，归并得到的新热点直接替换旧热点放在copy_hotspotCandidates中，用于执行后续的原贪婪选取算法
	//注意：对于每一个旧热点，为其遍历寻找最佳归并；已经被归并过的热点不再参与其他归并；
	static void MergeHotspots(int currentTime);

	//比较此次热点选取的结果与上一次选取结果之间的相似度
	static void CompareWithOldHotspots(int currentTime);


public:


	static int SLOT_POSITION_UPDATE;  //地理信息收集的slot
	static int SLOT_HOTSPOT_UPDATE;	 //更新热点和分类的slot
	static int STARTTIME_HOSPOT_SELECT;  //no MA node at first
	static bool TEST_HOTSPOT_SIMILARITY;

	static double RATIO_MERGE_HOTSPOT;
	static double RATIO_NEW_HOTSPOT;
	static double RATIO_OLD_HOTSPOT;


	/************************************ IHAR ************************************/

	static double LAMBDA;
	static int LIFETIME_POSITION;


	CHotspotSelect(){};
	~CHotspotSelect(){};

	//用于最终final结果的统计和记录
	static inline double getAverageHotspotCost()
	{
		if( COUNT_HOTSPOT_COST == 0 )
			return -1;
		else
			return double(SUM_HOTSPOT_COST) / double(COUNT_HOTSPOT_COST);
	}

	static inline double getAveragePercentMerge()
	{
		if( COUNT_PERCENT_MERGE == 0 )
			return 0.0;
		else
			return SUM_PERCENT_MERGE / COUNT_PERCENT_MERGE;
	}

	static inline double getAveragePercentOld()
	{
		if( COUNT_PERCENT_OLD == 0 )
			return 0.0;
		else
			return SUM_PERCENT_OLD / COUNT_PERCENT_OLD;
	}

	static inline double getAverageSimilarityRatio()
	{
		if( COUNT_SIMILARITY_RATIO == 0 )
			return -1;
		else
			return SUM_SIMILARITY_RATIO / COUNT_SIMILARITY_RATIO;
	}	

	//读取所有节点的当前位置，加入position列表（append），在每个地理位置信息收集时隙上调用
	//注意：必须在调用 UpdateNodeStatus() 之后调用
	static void CollectNewPositions(int currentTime);

	//执行热点选取
	static void HotspotSelect(int currentTime);

	// TODO: move print operation here
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

