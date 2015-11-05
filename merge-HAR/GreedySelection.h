#pragma once
#include "Preprocessor.h"

using namespace std;

extern bool DO_MERGE_HAR;
extern int startTimeForHotspotSelection;
extern int g_nPositions;
extern vector<CPosition*> g_positions;
extern vector<CHotspot*> g_hotspotCandidates;
extern vector<CHotspot*> g_oldSelectedHotspots;

extern string logInfo;

//HAR��ԭʼ��̰��hotspotѡȡ�����İ�װ
class CGreedySelection
{
private:
	vector<CHotspot *> copy_hotspotCandidates;  //����g_hotspotCandidates��̰��ѡȡ�����л��޸�hotspot����Ϣ
	vector<CPosition *> uncoveredPositions;  //������δ��cover��position
	vector<CHotspot *> unselectedHotspots;  //δ��ѡ�е�hotspot����
	vector<CHotspot *> hotspotsAboveAverage;  //ratio����1/2��hotspot����
	vector<CHotspot *> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����

	////merge-HAR: 
	////����g_oldSelectedHotspots����һ��̰��ѡȡ�õ�������selectedHotspots�����
	////ע�⣺��Ҫ�ֶ��ͷű���Ұָ��
	//vector<CHotspot*> copy_oldSelectedHotspots;  
	
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

