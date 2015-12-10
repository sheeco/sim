#pragma once

#include "Hotspot.h"
#include "FileParser.h"
#include "Processor.h"

using namespace std;

extern bool DO_IHAR;
extern bool DO_MERGE_HAR;

extern int NUM_NODE;
extern int MAX_MEMORY_TIME;
extern double MIN_POSITION_WEIGHT;

extern int* g_degreeForPositions;
extern int* g_degreeForHotspots;
extern vector<CPosition*> g_tmpPositions;

extern int g_old_nPositions;
extern int g_old_nHotspots;

//һЩԤ��������װ�ɵ���
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

public:
	/** �������� **/
	//��min��max�ķ�Χ������size�����ظ��������
	static vector<int> CPreprocessor::RandomIntList(int min, int max, int size);

	//�ͷ�ָ��vector
	static void freePointerVector(vector<CPosition *> &v);
	static void freePointerVector(vector<CHotspot *> &v);	
	
	//���ڹ鲢����ĺ���
	template <class E>
	static vector<E> merge(vector<E> &left, vector<E> &right, bool(*Comp)(E, E));
	template <class E>
	static vector<E> mergeSort(vector<E> &v, bool(*Comp)(E, E));
	//CData����ʱ������
	static vector<CData> merge(vector<CData> &left, vector<CData> &right, bool(*Comp)(CData, CData));
	static vector<CData> mergeSort(vector<CData> &v, bool(*Comp)(CData, CData));
	//CPosition��ֻ�ܰ���x��������CHotspot����԰���x�������cover������
	static vector<CPosition *> merge(vector<CPosition *> &left, vector<CPosition *> &right);
	static vector<CPosition *> mergeSort(vector<CPosition *> &v);
	static vector<CHotspot *> merge(vector<CHotspot *> &left, vector<CHotspot *> &right, bool(*Comp)(CHotspot *, CHotspot *));
	static vector<CHotspot *> mergeSort(vector<CHotspot *> &v, bool(*Comp)(CHotspot *, CHotspot *));
	//CHotspot�ྲ̬��������( endTime - 900, endTime )�ڼ��Ͷ�ݼ����Ľ�������
	static vector<CHotspot> mergeByDeliveryCount(vector<CHotspot> &left, vector<CHotspot> &right, int endTime);
	static vector<CHotspot> mergeSortByDeliveryCount(vector<CHotspot> &v, int endTime);
	//������Ϊ��������mergeSort������Comparison����
	static bool ascendByLocationX(CHotspot *left, CHotspot *right);
	static bool ascendByRatio(CHotspot *left, CHotspot *right);
	static bool descend(int left, int right){	return left > right;	};
	static bool ascendByInt(int left, int right){	return left < right;	};
	static bool ascendByData(CData left, CData right){	return left < right;	};

	/** Ԥ����������� **/
	//���ļ��ж�ȡ���нڵ�ĵ�ǰλ�ã�����position�б�append��
	//��main��������ÿ������λ����Ϣ�ռ�ʱ϶�ϵ���
	static void CollectNewPositions(int time);

	//merge-HAR: 
	//��merge��������
	static CHotspot* GenerateHotspotFromCoordinates(double x, double y, int time);

	//���ݵ�ĿǰΪֹ������position��¼��������ѡhotspot�б�
	//��main��������ÿ��hotspot����ʱ϶�ϵ���
	static void BuildCandidateHotspots(int time);

	/** Optional Functions����main��������**/
	/** ע�⣺�������ڵ���BuildCandidateHotspots֮���ٵ��� **/

	//��ÿһ��̰��ѡ��֮ǰ���ã�����CHotspot::oldSelectedHotspots��Ѱ��Ͷ�ݼ���Ϊ0���ȵ�ɾ������CHotspot::deletedHotspots
	//��ɾ�����Ӧ������position����CPosition::deletedPositions
	static void DecayPositionsWithoutDeliveryCount();

	static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

