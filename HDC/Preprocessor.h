#pragma once

#include "Hotspot.h"
#include "FileParser.h"
#include "Processor.h"
#include "Node.h"

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

//一些预处理函数包装成的类
class CPreprocessor :
	public CProcessor
{
private:
	CPreprocessor(void);
	~CPreprocessor(void);

	//根据position指针得到它在全局变量CPosition::positions中的下标
	//应在CPosition::positions完成排序之后调用，并且此后不能再改变其中的元素顺序
	static int getIndexOfPosition(CPosition* pos);

	//从pos出发生成一个初始hotspot，并完成此候选hotspot的构建
	static CHotspot* GenerateHotspotFromPosition(CPosition *pos, int time);

public:
	/** 辅助函数 **/
	//在min到max的范围内生成size个不重复的随机数
	static vector<int> CPreprocessor::RandomIntList(int min, int max, int size);

	//释放指针vector
	static void freePointerVector(vector<CPosition *> &v);
	static void freePointerVector(vector<CHotspot *> &v);	
	
	//用于归并排序的函数
	template <class E>
	static vector<E> merge(vector<E> left, vector<E> right, bool(*Comp)(E, E));
	template <class E>
	static vector<E> mergeSort(vector<E> v, bool(*Comp)(E, E));
	//CData按照时间排序
	static vector<CData> merge(vector<CData> left, vector<CData> right, bool(*Comp)(CData, CData));
	static vector<CData> mergeSort(vector<CData> v, bool(*Comp)(CData, CData));
	//CPosition类按照x坐标排序
	static vector<CPosition *> merge(vector<CPosition *> left, vector<CPosition *> right);
	static vector<CPosition *> mergeSort(vector<CPosition *> v);
	//CNode类按照x坐标排序
	static vector<CNode *> merge(vector<CNode *> left, vector<CNode *> right);
	static vector<CNode *> mergeSort(vector<CNode *> v);
	//CHotspot类按照x坐标或者ratio排序
	static vector<CHotspot *> merge(vector<CHotspot *> left, vector<CHotspot *> right, bool(*Comp)(CHotspot *, CHotspot *));
	static vector<CHotspot *> mergeSort(vector<CHotspot *> v, bool(*Comp)(CHotspot *, CHotspot *));
	//CHotspot类静态拷贝按照( endTime - 900, endTime )期间的投递技术的降序排列
	static vector<CHotspot> mergeByDeliveryCount(vector<CHotspot> left, vector<CHotspot> right, int endTime);
	static vector<CHotspot> mergeSortByDeliveryCount(vector<CHotspot> v, int endTime);
	//用于作为参数传入mergeSort函数的Comparison函数
	static bool ascendByLocationX(CHotspot *left, CHotspot *right);
	static bool ascendByRatio(CHotspot *left, CHotspot *right);
	static bool descendByInt(int left, int right){	return left > right;	};
	static bool ascendByInt(int left, int right){	return left < right;	};
	static bool ascendByData(CData left, CData right){	return left < right;	};

	/** 预处理操作函数 **/
	//从文件中读取所有节点的当前位置，加入position列表（append）
	//由main函数，在每个地理位置信息收集时隙上调用
	static void CollectNewPositions(int time);

	//merge-HAR: 
	//由merge函数调用
	static CHotspot* GenerateHotspotFromCoordinates(double x, double y, int time);

	//根据到目前为止的所有position记录，构建候选hotspot列表
	//由main函数，在每个hotspot更新时隙上调用
	static void BuildCandidateHotspots(int time);

	/** Optional Functions，由main函数调用**/
	/** 注意：都必须在调用BuildCandidateHotspots之后再调用 **/

	//在每一次贪婪选择之前调用，将从CHotspot::oldSelectedHotspots中寻找投递计数为0的热点删除放入CHotspot::deletedHotspots
	//并删除其对应的所有position放入CPosition::deletedPositions
	static void DecayPositionsWithoutDeliveryCount();

	static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

