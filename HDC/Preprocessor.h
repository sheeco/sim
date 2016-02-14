#pragma once

#include "Hotspot.h"
#include "Processor.h"
#include "Node.h"

using namespace std;

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


public:

	/** �������� **/
	//��min��max�ķ�Χ������size�����ظ��������
	static vector<int> RandomIntList(int min, int max, int size);

	//�ͷ�ָ��vector
	static void freePointerVector(vector<CPosition *> &v);
	static void freePointerVector(vector<CHotspot *> &v);	
	
	//���ڹ鲢����ĺ���
	template <class E>
	static vector<E> merge(vector<E> left, vector<E> right, bool(*Comp)(E, E));
	template <class E>
	static vector<E> mergeSort(vector<E> v, bool(*Comp)(E, E));
	//CData����ʱ������
	static vector<CData> merge(vector<CData> left, vector<CData> right, bool(*Comp)(CData, CData));
	static vector<CData> mergeSort(vector<CData> v, bool(*Comp)(CData, CData));
	//CPosition�ఴ��x��������
	static vector<CPosition *> merge(vector<CPosition *> left, vector<CPosition *> right);
	static vector<CPosition *> mergeSort(vector<CPosition *> v);
	//CNode�ఴ��x��������
	static vector<CNode *> merge(vector<CNode *> left, vector<CNode *> right);
	static vector<CNode *> mergeSort(vector<CNode *> v);
	//CHotspot�ఴ��x�������ratio����
	static vector<CHotspot *> merge(vector<CHotspot *> left, vector<CHotspot *> right, bool(*Comp)(CHotspot *, CHotspot *));
	static vector<CHotspot *> mergeSort(vector<CHotspot *> v, bool(*Comp)(CHotspot *, CHotspot *));
	//CHotspot�ྲ̬��������( endTime - 900, endTime )�ڼ��Ͷ�ݼ����Ľ�������
	static vector<CHotspot> mergeByDeliveryCount(vector<CHotspot> left, vector<CHotspot> right, int endTime);
	static vector<CHotspot> mergeSortByDeliveryCount(vector<CHotspot> v, int endTime);
	//������Ϊ��������mergeSort������Comparison����
	static bool ascendByLocationX(CHotspot *left, CHotspot *right);
	static bool ascendByRatio(CHotspot *left, CHotspot *right);
	static bool descendByInt(int left, int right){	return left > right;	};
	static bool ascendByInt(int left, int right){	return left < right;	};
	static bool ascendByData(CData left, CData right){	return left < right;	};

	/** Ԥ����������� **/

	/** Optional Functions����main��������**/

	static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

