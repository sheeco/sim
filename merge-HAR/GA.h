#pragma once
#include "GASolution.h"

/** GA相关参数 **/

extern int POPULATION_SIZE;
extern int MAX_UNIMPROVED_NUM;
extern int MAX_SOLUTION_NUM;
extern double PROB_CROSSOVER;  //交叉概率
extern double PROB_MUTATION;  //变异概率

extern int CO_MUTATION_FINAL;
extern int CO_MUTATION_CHILDREN;
extern double CO_MUTATION_GRADIENT;

extern int** g_coverMatrix;
extern int* g_degreeForPositions;
extern int* g_degreeForHotspots;

class CGA
{
private:
	CGASolution BestSolution;
	vector<CGASolution> pSolution;  //操作的初始解
	int NumberofSolution;
	int NumberofUnImproved;  //最优解没有改善的计数
	int MutatedBits;  //位变异的size

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

	//初始解产生，由main函数调用
	void GenerateInitialSolutions();

	CGASolution Optimize();

	//锦标赛选择，单个pool大小为n，即涉及到的解总数为2n
	void SelectTwoByTournament(int n);

	//按轮盘赌选择两个个体，并将 <var>current_op1</var> 和 <var>current_op2</var> 指向它们
	void SelectTwoByRoulette();

	//找到当前种群中适应度最好的个体，存入BestSolution
	//函数更新BestUnImproved，在每次迭代过程的最后调用一次，注意不可重复调用
	bool FindBestSolution();

	//找到当前种群中适应度最差的两个个体用于替换
	//将<var>worst_first</var>和<var>worst_second</var>指向它们
	void FindWorstSolution();

	bool ReplaceWorstSolution(CGASolution &output1, CGASolution &output2);

	void ReplaceBelowAverageSolution(CGASolution &output1, CGASolution &output2);

	//Variable Mutation Rate (by Beasley)
	void UpdateMutationRate();

	//判断一个个体是否与当前种群中的个体相同
	bool IfExists(CGASolution it);

	//判断是否符合结束条件，即达到MAX_UNIMPROVED_NUM和MAX_SOLUTION_NUM
	bool OptCompleted();

	void finalstatus();

};

