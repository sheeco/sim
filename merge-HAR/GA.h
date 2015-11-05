#pragma once
#include "GASolution.h"

/** GA��ز��� **/

extern int POPULATION_SIZE;
extern int MAX_UNIMPROVED_NUM;
extern int MAX_SOLUTION_NUM;
extern double PROB_CROSSOVER;  //�������
extern double PROB_MUTATION;  //�������

extern int CO_MUTATION_FINAL;
extern int CO_MUTATION_CHILDREN;
extern double CO_MUTATION_GRADIENT;

extern int g_nPositions;
extern int g_nHotspotCandidates;
extern int** g_coverMatrix;
extern int* g_degreeForPositions;
extern int* g_degreeForHotspots;
extern vector<CHotspot*> g_hotspotCandidates;
extern vector<CPosition*> g_positions;

class CGA
{
private:
	CGASolution BestSolution;
	vector<CGASolution> pSolution;  //�����ĳ�ʼ��
	int NumberofSolution;
	int NumberofUnImproved;  //���Ž�û�и��Ƶļ���
	int MutatedBits;  //λ�����size

	int current_op1;
	int current_op2;
	int runninground;
	int worst_first;
	int worst_second;
	double sumFitnessValue;

public:

	CGA(void);
	~CGA(void);

	vector<CHotspot *> getSelectedHotspots();

	//��ʼ���������main��������
	void GenerateInitialSolutions();

	CGASolution Optimize();

	//������ѡ�񣬵���pool��СΪn�����漰���Ľ�����Ϊ2n
	void SelectTwoByTournament(int n);

	//�����̶�ѡ���������壬���� <var>current_op1</var> �� <var>current_op2</var> ָ������
	void SelectTwoByRoulette();

	//�ҵ���ǰ��Ⱥ����Ӧ����õĸ��壬����BestSolution
	//��������BestUnImproved����ÿ�ε������̵�������һ�Σ�ע�ⲻ���ظ�����
	bool FindBestSolution();

	//�ҵ���ǰ��Ⱥ����Ӧ�������������������滻
	//��<var>worst_first</var>��<var>worst_second</var>ָ������
	void FindWorstSolution();

	bool ReplaceWorstSolution(CGASolution &output1, CGASolution &output2);

	void ReplaceBelowAverageSolution(CGASolution &output1, CGASolution &output2);

	//Variable Mutation Rate (by Beasley)
	void UpdateMutationRate();

	//�ж�һ�������Ƿ��뵱ǰ��Ⱥ�еĸ�����ͬ
	bool IfExists(CGASolution it);

	//�ж��Ƿ���Ͻ������������ﵽMAX_UNIMPROVED_NUM��MAX_SOLUTION_NUM
	bool OptCompleted();

	void finalstatus();

};

