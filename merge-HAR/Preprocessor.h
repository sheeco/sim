#pragma once

#include "Hotspot.h"
#include "FileParser.h"
#include "GASolution.h"

using namespace std;

extern bool DO_IHAR;
extern bool DO_MERGE_HAR;

extern int NUM_NODE;

extern int g_nPositions;
extern int g_nHotspotCandidates;
extern int** g_coverMatrix;
extern int* g_degreeForPositions;
extern int* g_degreeForHotspots;
extern vector<CHotspot*> g_hotspotCandidates;
extern vector<CHotspot*> g_selectedHotspots;
extern vector<CPosition*> g_positions;
extern vector<CPosition*> g_tmpPositions;

extern int g_old_nPositions;
extern int g_old_nHotspots;

//一些预处理函数包装成的类
class CPreprocessor
{
private:
	CPreprocessor(void);
	~CPreprocessor(void);

	//根据position指针得到它在全局变量g_positions中的下标
	//应在g_positions完成排序之后调用，并且此后不能再改变其中的元素顺序
	static int getIndexOfPosition(CPosition* pos);

	//从pos出发生成一个初始hotspot，并完成此候选hotspot的构建
	static CHotspot* GenerateHotspotFromPosition(CPosition *pos, int time);

	//从hotspot列表和position列表生成度数和cover矩阵信息
	//BuildCandidateHotspots()和RemoveBadHotspots()中会被调用
	static void GenerateDegrees();
	static void GenerateCoverMatrix();

public:
	/** 辅助函数 **/
	//在min到max的范围内生成size个不重复的随机数
	static vector<int> CPreprocessor::RandomIntList(int min, int max, int size);

	//释放指针vector
	static void freePointerVector(vector<CPosition *> &v);
	static void freePointerVector(vector<CHotspot *> &v);	
	
	//用于归并排序的函数
	//CPosition类只能按照x坐标排序，CHotspot类可以按照x坐标或者cover数排序
	static vector<CPosition *> merge(vector<CPosition *> &left, vector<CPosition *> &right);
	static vector<CPosition *> mergeSort(vector<CPosition *> &v);
	static vector<CHotspot *> merge(vector<CHotspot *> &left, vector<CHotspot *> &right, bool(*Comp)(CBase *, CBase *));
	static vector<CHotspot *> mergeSort(vector<CHotspot *> &v, bool(*Comp)(CBase *, CBase *));
	static vector<CGASolution> merge(vector<CGASolution> &left, vector<CGASolution> &right, bool(*Comp)(CGASolution, CGASolution));
	static vector<CGASolution> mergeSort(vector<CGASolution> &v, bool(*Comp)(CGASolution, CGASolution));
	//用于作为参数传入mergeSort函数的Comparison函数
	static bool largerByLocationX(CBase *left, CBase *right);
	static bool largerByNCoveredPositions(CBase *left, CBase *right); //实际只能传入CHotspot类

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

	static void UpdateDegrees();

	/** Optional Functions，由main函数调用**/
	/** 注意：都必须在调用BuildCandidateHotspots之后再调用 **/
	
	//为了减少计算量，在选取之前先删除一些太差的hotspot
	//删除数目：n/2; sqrt(n); the worse half; all below average; ...
	static void RemoveBadHotspots();

	//IHAR:排除孤立的position以提高GA优化效率，降低复杂度（未采用）
	static void RemoveIsolatePositions();
	static void PutBackAllPositions();  //将之前移除的position全部放回，必须在PostSelector之前调用，和RemoveIsolatePositions匹配调用

	//对于只含有一个position的hotspot，修正其中心，尽量包含更多的position
	static void AdjustRemoteHotspots();

	static void SaveHotspotsToFile(int time, vector<CHotspot *> hotspots);

};

