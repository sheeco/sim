#pragma once

#include "Preprocessor.h"
#include "Algorithm.h"

using namespace std;

extern double GAMMA;
extern int startTimeForHotspotSelection;

extern string INFO_LOG;

//HAR��ԭʼ��̰��hotspotѡȡ�����İ�װ
class CGreedySelection :
	public CAlgorithm
{
private:
	static vector<CHotspot *> copy_hotspotCandidates;  //����CHotspot::hotspotCandidates��̰��ѡȡ�����л��޸�hotspot����Ϣ
	static vector<CPosition *> uncoveredPositions;  //������δ��cover��position
	static vector<CHotspot *> unselectedHotspots;  //δ��ѡ�е�hotspot����
	static vector<CHotspot *> hotspotsAboveAverage;  //ratio����1/2��hotspot����
	static vector<CHotspot *> selectedHotspots;  //ѡ�е�hotspot���ϣ�������̰���㷨�����
	
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

	CGreedySelection();

	~CGreedySelection(void);

	//���ļ��ж�ȡ���нڵ�ĵ�ǰλ�ã�����position�б�append��
	//��main��������ÿ������λ����Ϣ�ռ�ʱ϶�ϵ���
	static void CollectNewPositions(int time);

	//���ݵ�ĿǰΪֹ������position��¼��������ѡhotspot�б�
	//��main��������ÿ��hotspot����ʱ϶�ϵ���
	static void BuildCandidateHotspots(int time);

	//ִ��̰��ѡȡ���̣�����ѡȡ���
	//ע�⣺���ص���Ϣ����CGreedySelection������֮��ʧЧ��Ӧ��ʱ����
	static void GreedySelect(int time);

	//merge-HAR: 
	//ִ���ȵ�鲢���̣��鲢�õ������ȵ�ֱ���滻���ȵ����copy_hotspotCandidates�У�����ִ�к�����ԭ̰��ѡȡ�㷨
	//ע�⣺����ÿһ�����ȵ㣬Ϊ�����Ѱ����ѹ鲢���Ѿ����鲢�����ȵ㲻�ٲ��������鲢��
	static void MergeHotspots(int time);

};

