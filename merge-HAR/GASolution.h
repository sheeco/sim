#pragma once

#include "Hotspot.h"
#include "Algorithm.h"

extern int** g_coverMatrix;
extern int* g_degreeForPositions;
extern int* g_degreeForHotspots;

//FIXME:�����Ӧ�Ⱥ����޸ģ�����Ҳ����Ҫ����Ӧ���޸�
#define WORST_FITNESS 999999
#define BEST_FITNESS 0

class CGASolution : 
	public CAlgorithm
{
private:
	int* choices;  //0-1 array (1 for selected hotspots; 0 otherwise)
	int* nCoverForPositions;  //ÿ��position�����ǵĴ���
	int* nNewCoverForHotspots;  //����ÿ��hotspot�����������ǵġ���ǰδ�����ǵ�position��Ŀ
	bool valid;  //�Ƿ�Ϸ��⣬������position��������һ��hotspot������
	int cost;  //��hotspot����
	int birthGeneration;  //����generation
	int rank;  //�����н��е���Ӧ����������CGA���㲢��ֵ�����ڻ���rank��ѡ�����
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

	//����Ƿ��ǺϷ��⣬����֮ǰӦ��ȷ�����ù�UpdateStatus()
	inline bool isValid()
	{
		return valid;
	}

	//������Ӧ�ȵ�ֵ������֮ǰӦ��ȷ�����ù�UpdateStatus()
	inline double getFitnessValue()
	{
		return fitness;
	}

	bool sameAs(CGASolution it);

	//FIXME:�����Ӧ�Ⱥ����޸ģ�����Ҳ����Ҫ����Ӧ���޸�
	static bool betterThan(double left, double right);
	static bool worseThan(double left, double right);
	static bool equalTo(double left, double right);

	void BuildSolutionRandomly();

	//���½����Ϣ����ÿ�����������������ѵ��ã���������ĵط���Ҫ�ֶ�����
	void UpdateStatus();

	vector<int> getHotspotsForPosition(int index_pos);
	vector<int> getPositionsForHotspot(int index_hotspot);

	//Beasley��heuristic feasibility operator����CGA::Optimize()����
	//crossover��mutation���������ڲ�����������ӵĵ���
	void GreedyFix();

	void RemoveRedundantHotspotRandomly();

	static void FusionCrossover(CGASolution &parent_1, CGASolution &parent_2, CGASolution &output1);

	static void UniformCrossover(CGASolution &parent_1, CGASolution &parent_2, CGASolution &output1);

	void BitMutation(int nBits);

	string toString();

	//DBG:
	void printDegrees();
};

