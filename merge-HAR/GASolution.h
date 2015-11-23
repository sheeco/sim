#pragma once
#include "Hotspot.h"

extern int** g_coverMatrix;
extern int* g_degreeForPositions;
extern int* g_degreeForHotspots;

//FIXME:如果适应度函数修改，这里也可能要做相应的修改
#define WORST_FITNESS 999999
#define BEST_FITNESS 0

class CGASolution
{
private:
	int* choices;  //0-1 array (1 for selected hotspots; 0 otherwise)
	int* nCoverForPositions;  //每个position被覆盖的次数
	int* nNewCoverForHotspots;  //对于每个hotspot，由它所覆盖的、此前未被覆盖的position数目
	bool valid;  //是否合法解，即所有position都被至少一个hotspot所覆盖
	int cost;  //即hotspot个数
	int birthGeneration;  //所属generation
	int rank;  //在所有解中的适应度排名，由CGA计算并赋值，用于基于rank的选择操作
	double fitness;

	int typeCrossover;

public:
	CGASolution();
	CGASolution(const CGASolution& it);
	const CGASolution& operator=(const CGASolution& it);

	~CGASolution(void);

	inline int* getChoices()
	{
		return choices;
	}
	inline int* getNCoverForPositions()
	{
		return nCoverForPositions;
	}
	inline int* getNNewCoverForHotspots()
	{
		return nNewCoverForHotspots;
	}
	inline int getBirthGeneration()
	{
		return birthGeneration;
	}
	inline void setBirthGeneration(int generation)
	{
		this->birthGeneration = generation;
	}
	inline int getRank()
	{
		return rank;
	}
	inline void setRank(int rank)
	{
		this->rank = rank;
	}

	//检查是否是合法解，调用之前应该确保调用过UpdateStatus()
	inline bool isValid()
	{
		return valid;
	}

	//返回适应度的值，调用之前应该确保调用过UpdateStatus()
	inline double getFitnessValue()
	{
		return fitness;
	}

	bool sameAs(CGASolution it);

	//FIXME:如果适应度函数修改，这里也可能要做相应的修改
	static bool betterThan(double left, double right);
	static bool worseThan(double left, double right);
	static bool equalTo(double left, double right);

	void BuildSolutionRandomly();

	//更新解的信息，在每个解操作函数的最后都已调用，除此以外的地方需要手动调用
	void UpdateStatus();

	vector<int> getHotspotsForPosition(int index_pos);
	vector<int> getPositionsForHotspot(int index_hotspot);

	//Beasley的heuristic feasibility operator，由CGA::Optimize()调用
	//crossover和mutation操作函数内不包含这个算子的调用
	void GreedyFix();

	void RemoveRedundantHotspotRandomly();

	static void FusionCrossover(CGASolution &parent_1, CGASolution &parent_2, CGASolution &output1);

	static void UniformCrossover(CGASolution &parent_1, CGASolution &parent_2, CGASolution &output1);

	void BitMutation(int nBits);

	string toString();

	//DBG:
	void printDegrees();
};

