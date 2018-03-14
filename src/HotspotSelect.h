#pragma once

#ifndef __HOTSPOT_SELECT_H__
#define __HOTSPOT_SELECT_H__

#include "SortHelper.h"


//HAR��ԭʼ��̰��hotspotѡȡ�����İ�װ
class CHotspotSelect :
	virtual public CAlgorithm
{
private:

	static vector<CPosition *> positions;
	//	//����Ӻ�ѡposition������ɾ����position��������
	//	static vector<CPosition *> deletedPositions;

	static vector<CHotspot *> hotspotCandidates;
	static vector<CHotspot *> unselectedHotspots;  //δ��ѡ�е�hotspot����
	static vector<CHotspot *> hotspotsAboveAverage;  //ratio����1/2��hotspot����
	static vector<CHotspot *> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����

	//��һ��̰��ѡȡ���յõ����ȵ㼯�ϣ�����
	//ע�⣺merge�����õ������hotspotӦ��ʹ��CHotspot::hotspotCandidates�е�ʵ���޸ĵõ������ɱ�����CHotspot::oldSelectedHotspots��ʵ�����κ�����
	//����Ϊ��merge�����󽫱�free
	static map<int, vector<CHotspot *>> oldSelectedHotspots;

	//���ڼ�������ȡ�����ȵ���������ʷƽ��ֵ
	static int SUM_HOTSPOT_COST;
	static int COUNT_HOTSPOT_COST;
	//���ڼ�������ѡȡ�����ȵ㼯���У�merge��old�ȵ�ı�������ʷƽ��ֵ
	static double SUM_PERCENT_MERGE;
	static int COUNT_PERCENT_MERGE;
	static double SUM_PERCENT_OLD;
	static int COUNT_PERCENT_OLD;
	//���ڼ����ȵ�ǰ�����ƶȵ���ʷƽ��ֵ
	static double SUM_SIMILARITY_RATIO;
	static int COUNT_SIMILARITY_RATIO;


	//����hotspotsAboveAverage����ÿһ�ε��������
	static void updateAboveAverageList();

	//������ڵ�ѡ���ȵ㣬�ͷ���һ��ѡȡ��δ��ѡ�еķ����ȵ�
	static void SaveOldSelectedHotspots(int now);

	//���ݵ�ĿǰΪֹ������position��¼��������ѡhotspot�б�
	//��main��������ÿ��hotspot����ʱ϶�ϵ���
	static void BuildCandidateHotspots(int now);

	//ִ��̰��ѡȡ���̣�����ѡȡ���
	//ע�⣺���ص���Ϣ����CHotspotSelect������֮��ʧЧ��Ӧ��ʱ����
	static void GreedySelect(int now);

	//merge-HAR: 
	//ִ���ȵ�鲢���̣��鲢�õ������ȵ�ֱ���滻���ȵ����copy_hotspotCandidates�У�����ִ�к�����ԭ̰��ѡȡ�㷨
	//ע�⣺����ÿһ�����ȵ㣬Ϊ�����Ѱ����ѹ鲢���Ѿ����鲢�����ȵ㲻�ٲ��������鲢��
	static void MergeHotspots(int now);

	//�Ƚϴ˴��ȵ�ѡȡ�Ľ������һ��ѡȡ���֮������ƶ�
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

	//��������final�����ͳ�ƺͼ�¼
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
		//����ɸѡ���µ������ڵ�
		vector<int> newIds = deadNodes;
		RemoveFromList(newIds, idDeadNodes);
		//ɾ�������ڵ��position��¼
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

	//��ȡ���нڵ�ĵ�ǰλ�ã�����position�б�append������ÿ������λ����Ϣ�ռ�ʱ϶�ϵ���
	//ע�⣺�����ڵ��� UpdateNodeStatus() ֮�����
	static void CollectNewPositions(int now, vector<CNode*> nodes);

	//ѡȡhotspot��ɺ󣬽������ǵ�ÿһ��position���䵽Ψһһ��hotspot
	static void assignPositionsToHotspots(vector<CHotspot *> &hotspots)
	{
		vector<CHotspot *> temp_hotspots = hotspots;
		hotspots.clear();

		while(!temp_hotspots.empty())
		{
			temp_hotspots = CSortHelper::mergeSort(temp_hotspots, CSortHelper::ascendByRatio);
			//FIXME:������ / ƽ����
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

	//ִ���ȵ�ѡȡ������ѡȡ���
	static vector<CHotspot *> HotspotSelect(vector<int> idNodes, int now);

	static void PrintInfo(int now);
	static void PrintFinal(int now);

};

#endif // __HOTSPOT_SELECT_H__
