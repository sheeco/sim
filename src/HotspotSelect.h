#pragma once

#include "SortHelper.h"
#include "Algorithm.h"


//HAR��ԭʼ��̰��hotspotѡȡ�����İ�װ
class CHotspotSelect :
	public CAlgorithm
{
private:

	static vector<CHotspot *> copy_hotspotCandidates;  //����CHotspot::hotspotCandidates��̰��ѡȡ�����л��޸�hotspot����Ϣ
	static vector<CPosition *> uncoveredPositions;  //������δ��cover��position
	static vector<CHotspot *> unselectedHotspots;  //δ��ѡ�е�hotspot����
	static vector<CHotspot *> hotspotsAboveAverage;  //ratio����1/2��hotspot����
	static vector<CHotspot *> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����
	
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


	//����Ƿ������ѡȡ�õ��Ϸ��Ľ�
	static inline bool isValid()
	{
		if( (! selectedHotspots.empty()) && uncoveredPositions.empty() )
			return true;
		else
			return false;
	}
	//�ڹ�����ѡ�ȵ�֮�����
	static void updateHotspotCandidates();
	//����hotspotsAboveAverage����ÿһ�ε��������
	static void updateStatus();


public:

	static bool TEST_HOTSPOT_SIMILARITY;

	CHotspotSelect(){};
	~CHotspotSelect(){};

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

	//���ļ��ж�ȡ���нڵ�ĵ�ǰλ�ã�����position�б�append��
	//��main��������ÿ������λ����Ϣ�ռ�ʱ϶�ϵ���
	static void CollectNewPositions(int currentTime);

	//���ݵ�ĿǰΪֹ������position��¼��������ѡhotspot�б�
	//��main��������ÿ��hotspot����ʱ϶�ϵ���
	static void BuildCandidateHotspots(int currentTime);

	//ִ��̰��ѡȡ���̣�����ѡȡ���
	//ע�⣺���ص���Ϣ����CHotspotSelect������֮��ʧЧ��Ӧ��ʱ����
	static void GreedySelect(int currentTime);

	//merge-HAR: 
	//ִ���ȵ�鲢���̣��鲢�õ������ȵ�ֱ���滻���ȵ����copy_hotspotCandidates�У�����ִ�к�����ԭ̰��ѡȡ�㷨
	//ע�⣺����ÿһ�����ȵ㣬Ϊ�����Ѱ����ѹ鲢���Ѿ����鲢�����ȵ㲻�ٲ��������鲢��
	static void MergeHotspots(int currentTime);

	//�Ƚϴ˴��ȵ�ѡȡ�Ľ������һ��ѡȡ���֮������ƶ�
	static void CompareWithOldHotspots(int currentTime);

	// TODO: move print operation here
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

