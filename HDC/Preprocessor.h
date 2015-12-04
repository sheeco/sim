#pragma once

#include "Hotspot.h"
#include "FileParser.h"
#include "GASolution.h"
#include "Processor.h"

using namespace std;

extern bool DO_IHAR;
extern bool DO_MERGE_HAR;

extern int NUM_NODE;
extern int MAX_MEMORY_TIME;
extern double MIN_POSITION_WEIGHT;

extern int** g_coverMatrix;
extern int* g_degreeForPositions;
extern int* g_degreeForHotspots;
extern vector<CPosition*> g_tmpPositions;

extern int g_old_nPositions;
extern int g_old_nHotspots;

//һЩԤ����������װ�ɵ���
class CPreprocessor :
	public CProcessor
{
private:
	CPreprocessor(void);
	~CPreprocessor(void);

	//����positionָ��õ�����ȫ�ֱ���CPosition::positions�е��±�
	//Ӧ��CPosition::positions�������֮����ã����Ҵ˺����ٸı����е�Ԫ��˳��
	static int getIndexOfPosition(CPosition* pos);

	//��pos��������һ����ʼhotspot������ɴ˺�ѡhotspot�Ĺ���
	static CHotspot* GenerateHotspotFromPosition(CPosition *pos, int time);

	//��hotspot�б���position�б����ɶ�����cover������Ϣ
	//BuildCandidateHotspots()��RemoveBadHotspots()�лᱻ����
	static void GenerateDegrees();
	static void GenerateCoverMatrix();

public:
	/** �������� **/
	//��min��max�ķ�Χ������size�����ظ��������
	static vector<int> CPreprocessor::RandomIntList(int min, int max, int size);

	//�ͷ�ָ��vector
	static void freePointerVector(vector<CPosition *> &v);
	static void freePointerVector(vector<CHotspot *> &v);	
	
	//���ڹ鲢����ĺ���
	static vector<int> merge(vector<int> &left, vector<int> &right, bool(*Comp)(int, int));
	static vector<int> mergeSort(vector<int> &v, bool(*Comp)(int, int));
	//CPosition��ֻ�ܰ���x��������CHotspot����԰���x�������cover������
	static vector<CPosition *> merge(vector<CPosition *> &left, vector<CPosition *> &right);
	static vector<CPosition *> mergeSort(vector<CPosition *> &v);
	static vector<CHotspot *> merge(vector<CHotspot *> &left, vector<CHotspot *> &right, bool(*Comp)(CHotspot *, CHotspot *));
	static vector<CHotspot *> mergeSort(vector<CHotspot *> &v, bool(*Comp)(CHotspot *, CHotspot *));
	//CHotspot�ྲ̬��������( endTime - 900, endTime )�ڼ��Ͷ�ݼ����Ľ�������
	static vector<CHotspot> mergeByDeliveryCount(vector<CHotspot> &left, vector<CHotspot> &right, int endTime);
	static vector<CHotspot> mergeSortByDeliveryCount(vector<CHotspot> &v, int endTime);
	//GASolution�ఴ��fitness����
	//static vector<CGASolution> merge(vector<CGASolution> &left, vector<CGASolution> &right, bool(*Comp)(CGASolution, CGASolution));
	//static vector<CGASolution> mergeSort(vector<CGASolution> &v, bool(*Comp)(CGASolution, CGASolution));
	//������Ϊ��������mergeSort������Comparison����
	static bool ascendByLocationX(CHotspot *left, CHotspot *right);
	static bool ascendByRatio(CHotspot *left, CHotspot *right);
	static bool descendByInt(int left, int right){	return left > right;	};

	/** Ԥ������������ **/
	//���ļ��ж�ȡ���нڵ�ĵ�ǰλ�ã�����position�б���append��
	//��main��������ÿ������λ����Ϣ�ռ�ʱ϶�ϵ���
	static void CollectNewPositions(int time);

	//merge-HAR: 
	//��merge��������
	static CHotspot* GenerateHotspotFromCoordinates(double x, double y, int time);

	//���ݵ�ĿǰΪֹ������position��¼��������ѡhotspot�б�
	//��main��������ÿ��hotspot����ʱ϶�ϵ���
	static void BuildCandidateHotspots(int time);

	static void UpdateDegrees();

	/** Optional Functions����main��������**/
	/** ע�⣺�������ڵ���BuildCandidateHotspots֮���ٵ��� **/
	
	//Ϊ�˼��ټ���������ѡȡ֮ǰ��ɾ��һЩ̫���hotspot
	//ɾ����Ŀ��n/2; sqrt(n); the worse half; all below average; ...
	static void RemoveBadHotspots();

	//IHAR:�ų�������position�����GA�Ż�Ч�ʣ����͸��Ӷȣ�δ���ã�
	static void RemoveIsolatePositions();
	static void PutBackAllPositions();  //��֮ǰ�Ƴ���positionȫ���Żأ�������PostSelector֮ǰ���ã���RemoveIsolatePositionsƥ�����

	//��ÿһ��̰��ѡ��֮ǰ���ã�����CHotspot::oldSelectedHotspots��Ѱ��Ͷ�ݼ���Ϊ0���ȵ�ɾ������CHotspot::deletedHotspots
	//��ɾ�����Ӧ������position����CPosition::deletedPositions
	static void DecayPositionsWithoutDeliveryCount();

	//����ֻ����һ��position��hotspot�����������ģ��������������position
	static void AdjustRemoteHotspots();

	static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};
