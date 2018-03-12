#pragma once

#ifndef __HOTSPOT_SELECT_H__
#define __HOTSPOT_SELECT_H__

#include "SortHelper.h"
#include "Algorithm.h"


//HAR中原始的贪婪hotspot选取方法的包装
class CHotspotSelect :
	virtual public CAlgorithm
{
private:

	static vector<CPosition *> positions;
	//	//保存从候选position集合中删除的position，测试用
	//	static vector<CPosition *> deletedPositions;

	static vector<CHotspot *> hotspotCandidates;
	static vector<CHotspot *> unselectedHotspots;  //未被选中的hotspot集合
	static vector<CHotspot *> hotspotsAboveAverage;  //ratio高于1/2的hotspot集合
	static vector<CHotspot *> selectedHotspots;  //选中的hotspot集合，即整个贪婪算法的输出

	//上一次贪婪选取最终得到的热点集合，保留
	//注意：merge操作得到的输出hotspot应该使用CHotspot::hotspotCandidates中的实例修改得到，不可保留对CHotspot::oldSelectedHotspots中实例的任何引用
	//，因为在merge结束后将被free
	static map<int, vector<CHotspot *>> oldSelectedHotspots;

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


	//更新hotspotsAboveAverage，在每一次迭代后调用
	static void updateAboveAverageList();

	//保存过期的选中热点，释放上一轮选取中未被选中的废弃热点
	static void SaveOldSelectedHotspots(int now);

	//根据到目前为止的所有position记录，构建候选hotspot列表
	//由main函数，在每个hotspot更新时隙上调用
	static void BuildCandidateHotspots(int now);

	//执行贪婪选取过程，返回选取结果
	//注意：返回的信息将在CHotspotSelect类析构之后失效，应及时保存
	static void GreedySelect(int now);

	//merge-HAR: 
	//执行热点归并过程，归并得到的新热点直接替换旧热点放在copy_hotspotCandidates中，用于执行后续的原贪婪选取算法
	//注意：对于每一个旧热点，为其遍历寻找最佳归并；已经被归并过的热点不再参与其他归并；
	static void MergeHotspots(int now);

	//比较此次热点选取的结果与上一次选取结果之间的相似度
	static void CompareWithOldHotspots(int now);


public:

	static int STARTTIME_HOTSPOT_SELECT;
	static int SLOT_POSITION_UPDATE;
	static int SLOT_HOTSPOT_UPDATE;
	static int LIFETIME_POSITION;
	static bool TEST_HOTSPOT_SIMILARITY;

	static void Init();

	CHotspotSelect(){};
	~CHotspotSelect(){};

	static vector<CHotspot*> getSelectedHotspots()
	{
		return selectedHotspots;
	}
	static vector<CHotspot*> getSelectedHotspots(int forTime)
	{
		if(!selectedHotspots.empty()
		   && forTime == selectedHotspots[0]->getTime())
			return selectedHotspots;
		else if(oldSelectedHotspots.find(forTime) != oldSelectedHotspots.end())
			return oldSelectedHotspots[forTime];
		else
			throw string("CHotspotSelect::getSelectedHotspots(" + STRING(forTime) + ") : Cannot find selected hotspots for given time !");
	}

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

	static void RemovePositionsForDeadNodes(vector<int> deadNodes, int now)
	{
		static vector<int> idDeadNodes;
		//用于筛选出新的死亡节点
		vector<int> newIds = deadNodes;
		RemoveFromList(newIds, idDeadNodes);
		//删除死亡节点的position记录
		if(newIds.empty())
			return;
		else
		{
			for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); )
			{
				if(IfExists(newIds, ( *ipos )->getNode()))
				{
					delete *ipos;
					ipos = positions.erase(ipos);
				}
				else
					++ipos;
			}
			AddToListUniquely(idDeadNodes, newIds);
		}

	}

	//读取所有节点的当前位置，加入position列表（append），在每个地理位置信息收集时隙上调用
	//注意：必须在调用 UpdateNodeStatus() 之后调用
	static void CollectNewPositions(int now, vector<CNode*> nodes)
	{
		if(!( now % SLOT_POSITION_UPDATE == 0 ))
			return;

		CPosition* temp_pos = nullptr;

		//遍历所有节点，获取当前位置，生成相应的CPosition类，添加到positions中
		for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
		{
			temp_pos = new CPosition();
			CCoordinate location = ( *inode )->getLocation();
			temp_pos->setLocation(location, now);
			temp_pos->setNode(( *inode )->getID());
			temp_pos->generateID();
			positions.push_back(temp_pos);
		}

		//IHAR: 删除过期的position记录
		if(getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved)
		{
			int threshold = now - LIFETIME_POSITION;
			if(threshold > 0)
			{
				for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); )
				{
					if(( *ipos )->getTime() < threshold)
					{
						delete *ipos;
						ipos = positions.erase(ipos);
					}
					else
						++ipos;
				}
			}
		}
	}

	//选取hotspot完成后，将被覆盖的每一个position分配到唯一一个hotspot
	static void assignPositionsToHotspots(vector<CHotspot *> &hotspots)
	{
		vector<CHotspot *> temp_hotspots = hotspots;
		hotspots.clear();

		while(!temp_hotspots.empty())
		{
			temp_hotspots = CSortHelper::mergeSort(temp_hotspots, CSortHelper::ascendByRatio);
			//FIXME:尽量多 / 平均？
			CHotspot *selected_hotspot = temp_hotspots.back();
			if(selected_hotspot->getNCoveredPosition() == 0)
				break;
			temp_hotspots.pop_back();
			hotspots.push_back(selected_hotspot);
			vector<CPosition *> positions = selected_hotspot->getCoveredPositions();
			for(vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin(); ihotspot != temp_hotspots.end(); ++ihotspot)
			{
				( *ihotspot )->removePositionList(positions);
				( *ihotspot )->updateStatus();
			}
		}
	}

	//执行热点选取，返回选取结果
	static vector<CHotspot *> HotspotSelect(vector<int> idNodes, int now);

	static void PrintInfo(int now);
	static void PrintFinal(int now);

};

#endif // __HOTSPOT_SELECT_H__
