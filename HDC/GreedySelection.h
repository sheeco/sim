#pragma once

#include "Preprocessor.h"
#include "Algorithm.h"

using namespace std;

extern bool DO_MERGE_HAR;
extern double GAMMA;
extern int startTimeForHotspotSelection;

extern string INFO_LOG;

//HAR��ԭʼ��̰��hotspotѡȡ�����İ�װ
class CGreedySelection :
	public CAlgorithm
{
private:
	vector<CHotspot *> copy_hotspotCandidates;  //����CHotspot::hotspotCandidates��̰��ѡȡ�����л��޸�hotspot����Ϣ
	vector<CPosition *> uncoveredPositions;  //������δ��cover��position
	vector<CHotspot *> unselectedHotspots;  //δ��ѡ�е�hotspot����
	vector<CHotspot *> hotspotsAboveAverage;  //ratio����1/2��hotspot����
	vector<CHotspot *> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����
	
	//����Ƿ������ѡȡ�õ��Ϸ��Ľ�
	inline bool isValid()
	{
		if( (! selectedHotspots.empty()) && uncoveredPositions.empty() )
			return true;
		else
			return false;
	}
	//����hotspotsAboveAverage����ÿһ�ε��������
	void UpdateStatus();

public:

	CGreedySelection();
	//CGreedySelection(vector<CHotspot *> &hotspotsCandidates);
	////merge_HAR: 
	//CGreedySelection(vector<CHotspot *> &hotspotsCandidates, vector<CHotspot *> &oldSelectedHotspots);

	~CGreedySelection(void);

	//���ļ��ж�ȡ���нڵ�ĵ�ǰλ�ã�����position�б�append��
	//��main��������ÿ������λ����Ϣ�ռ�ʱ϶�ϵ���
	static void CollectNewPositions(int time);

	//���ݵ�ĿǰΪֹ������position��¼��������ѡhotspot�б�
	//��main��������ÿ��hotspot����ʱ϶�ϵ���
	static void BuildCandidateHotspots(int time);

	//ִ��̰��ѡȡ���̣�����ѡȡ���
	//ע�⣺���ص���Ϣ����CGreedySelection������֮��ʧЧ��Ӧ��ʱ����
	void GreedySelect(int time);

	//merge-HAR: 
	//ִ���ȵ�鲢���̣��鲢�õ������ȵ�ֱ���滻���ȵ����copy_hotspotCandidates�У�����ִ�к�����ԭ̰��ѡȡ�㷨
	//ע�⣺����ÿһ�����ȵ㣬Ϊ�����Ѱ����ѹ鲢���Ѿ����鲢�����ȵ㲻�ٲ��������鲢��
	void mergeHotspots(int time);

	vector<CHotspot *> getSelectedHotspots()
	{
		return selectedHotspots;
	}

	int getCost();
	string toString();
};

