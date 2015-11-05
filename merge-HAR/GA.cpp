#include "GA.h"
#include "Preprocessor.h"

int debug_int = 500;

CGA::CGA(void)
{
	this->NumberofSolution = POPULATION_SIZE;
	this->pSolution.clear();
	for (int i = 0; i < POPULATION_SIZE; i++)
	{
		this->pSolution.push_back(CGASolution());
	}
	this->BestSolution = CGASolution();
	this->NumberofUnImproved = 0;
	this->MutatedBits = 1;
	this->current_op1 = 0;
	this->current_op2 = 0;
	this->runninground=0;
	this->worst_first = 0;
	this->worst_second = 0;
	this->sumFitnessValue = 0;
}

CGA::~CGA(void)
{
	pSolution.swap(vector<CGASolution>());
}

vector<CHotspot *> CGA::getSelectedHotspots()
{
	vector<CHotspot *> selectedHotspots;
	int *choices = BestSolution.getChoices();
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		if(choices[i])
			selectedHotspots.push_back(g_hotspotCandidates[i]);
	}
	return selectedHotspots;
}

//��ʼ���������main��������
void CGA::GenerateInitialSolutions()
{
//DBG:
	cout<<"��ʼ��������..."<<endl;

	for (unsigned int i = 0; i < pSolution.size(); i++)
	{
		pSolution[i].BuildSolutionRandomly();
		pSolution[i].GreedyFix();
		pSolution[i].RemoveRedundantHotspotRandomly();
	}
//DBG:
	cout<<"��ʼ��������ɣ�"<<endl;

	FindBestSolution();
	FindWorstSolution();
}

CGASolution CGA::Optimize()
{
	//FIXME: setting of final mutation bits number
	//CO_MUTATION_FINAL =  ((g_nHotspotCandidates / 10) > 10)? (g_nHotspotCandidates / 10) : 10;
	CO_MUTATION_FINAL = 10;

	double bet1 = 0;
	int bet2 = 0;
	bool improved = false;  //�Ƿ������Ⱥ
	CGASolution *output1, *output2;  //���ڱ��潻�桢�����Ľ��

	while(!OptCompleted())//�Ż�δ�ﵽ�ٽ�����
	{
		cout<<"************************************************************************"<<endl;
		cout<<"Optimization "<< runninground <<" begins."<<endl;
		//if(runninground % debug_int == 0)
		//{
		//	savetofile<<"************************************************************************"<<endl;
		//	savetofile<<"Optimization "<< runninground <<endl;
		//}
//DBG:
		//cout<<"pSolution.size: "<<pSolution.size()<<endl;
		//if(pSolution.size() == 0)
		//	_PAUSE;
		//cout<<"Fitness Values:\n";
		//for(int i = 0; i< pSolution.size(); i++)
		//	cout<< pSolution[i].getFitnessValue()<<" | ";

		output1 = new CGASolution();
		output2 = new CGASolution();

		//ѡ����������
		SelectTwoByTournament(2);
//DBG:
		//printf("Selected indivs: %d, %d\n", current_op1, current_op2);
		//pSolution[current_op1].PrintSolution();
		//pSolution[current_op2].PrintSolution();

		bet1 = RandomFloat(0, 1);

//DBG:
		//cout<<"bet1: "<<bet1<<endl;

		//���������ʵʩ����
		if(bet1 <= PROB_CROSSOVER)
		{
			//���ʵʩ���������е�һ��
			bet2 = RandomInt(0, 1);
//DBG:
			//cout<<"bet2 : "<<bet2<<endl;

			switch(bet2)
			{
			case 0:
				cout<<"Fusion Crossover"<<endl;
				CGASolution::FusionCrossover(pSolution[current_op1], pSolution[current_op2], *output1);
				CGASolution::FusionCrossover(pSolution[current_op1], pSolution[current_op2], *output2);
				break;
			//case 1:
			//	//cout<<"�ǶԳ�ƥ�佻��"<<endl;
			//	UnSymmCrossover(pSolution[current_op1], pSolution[current_op2], *output1, *output2);
			//	break;
			//case 2:
			//	//cout<<"��·����"<<endl;
			//	RouteCrossover(pSolution[current_op1], pSolution[current_op2], *output1, *output2);
			//	break;
			default:
				break;
			}

//DBG:
			//printf("Output1:\n");
			//output1->PrintSolution();
			//printf("Output2:\n");
			//output2->PrintSolution();

		}//end if performing crossover
		//�����ʵʩ���棬ֱ�Ӹ��Ʊ�ѡ�е��������壬����ʵʩ����
		else
		{
//DBG:
			cout<<"No Crossover"<<endl;

			*output1 = pSolution[current_op1];
			*output2 = pSolution[current_op2];
		}

		//���������ʵʩ����
		bet1 = RandomFloat(0, 1);
//DBG:
		//cout<<"bet1: "<<bet1<<endl;

		if(bet1 <= PROB_MUTATION)
		{
			bet2 = RandomInt(0, 1);
//DBG:
			//cout<<"bet2 : "<<bet2<<endl;

			switch (bet2)
			{
			case 0:
				cout<<"Bit Mutation (x"<<MutatedBits<<")"<<endl;
				output1->BitMutation(MutatedBits);
				output2->BitMutation(MutatedBits);
				break;
			default:
				break;
			}

//DBG:

			//printf("output3: \n");
			//output3->PrintSolution();
			//printf("output4: \n");
			//output4->PrintSolution();

		}//end if performing mutation
		//�����ʵʩ���죬ֱ�Ӹ���output1, output2��Ϊ���ε����Ľ��
		else
		{
//DBG:
			cout<<"No Mutation"<<endl;
		}

		output1->GreedyFix();
		output1->RemoveRedundantHotspotRandomly();
		output2->GreedyFix();
		output2->RemoveRedundantHotspotRandomly();

		/*���output1, output2��ԭ��Ⱥ��Ⱦɫ�岻��ͬ��
		  ����Ӧ��ֵ���ڻ����ԭ��Ⱥ�����ĸ��壬����и���*/
		ReplaceWorstSolution(*output1, *output2);
		improved = FindBestSolution();
	
		NumberofSolution += 2;
		if(improved)
			NumberofUnImproved = 0;
		else
			NumberofUnImproved++;

//DBG:
		//if(improved || runninground % debug_int == 0)
		{
			cout<<"\nBest Solution Now: "<<endl;
			//cout<<BestSolution.toString()<<endl;
			cout<<"fitness: "<<BestSolution.getFitnessValue()<<endl;
		}
		//if(runninground % debug_int == 0)
		//{
		//	savetofile<<"\nBest Solution Now: "<<endl;
		//	//savetofile<<BestSolution.toString()<<endl;
		//	savetofile<<"fitness: "<<BestSolution.getFitnessValue()<<endl;
		//}
		UpdateMutationRate();
		improved = false;
		delete output1;
		delete output2;

		runninground++; 
	}
	//�Ż���������ӡ���ս��
	finalstatus();
	return BestSolution;
}

//������ѡ�񣬵���pool��СΪn�����漰���Ľ�����Ϊ2n
void CGA::SelectTwoByTournament(int n)
{
	if(n > POPULATION_SIZE / 2)
		n = POPULATION_SIZE / 2;
	vector<int> pool = CPreprocessor::RandomIntList(0, POPULATION_SIZE, 2 * n);
	vector<int>::iterator middle = pool.begin() + n;
	vector<int> left(pool.begin(), middle);
	vector<int> right(middle, pool.end());
	double best_fitness = WORST_FITNESS;
	for(int i = 0; i < n; i++)
	{
		if(CGASolution::betterThan( pSolution[left[i]].getFitnessValue(), best_fitness ) )
		{
			best_fitness = pSolution[left[i]].getFitnessValue();
			current_op1 = left[i];
		}
	}
	best_fitness = WORST_FITNESS;
	for(int i = 0; i < n; i++)
	{
		if(CGASolution::betterThan( pSolution[right[i]].getFitnessValue(), best_fitness ) )
		{
			best_fitness = pSolution[right[i]].getFitnessValue();
			current_op2 = right[i];
		}
	}
}

//�����̶�ѡ���������壬���� <var>current_op1</var> �� <var>current_op2</var> ָ������
void CGA::SelectTwoByRoulette()
{
	double bet1, bet2;
	double curr = 0;
	int pos1 = -1, pos2 = -1;
	unsigned int i, j;
	int begin_pos = 0;
	
	bet1 = (double)RandomFloat(0, sumFitnessValue);
	begin_pos = RandomInt(0, pSolution.size());
//DBG:
	//cout<<"SelectTwo::sumFitnessValue = "<<sumFitnessValue<<endl;
	//cout<<"SelectTwo::bet1 = "<<bet1<<endl;

	for(i = begin_pos, j = 0; j < pSolution.size(); j++, i = (i + 1) % pSolution.size())
	{
		//FIXME:�����Ӧ�Ⱥ����޸ģ�����Ҳ����Ҫ����Ӧ���޸�
		curr += BestSolution.getFitnessValue() / pSolution[i].getFitnessValue();
		//curr += pSolution[i].getFitnessValue()/BestSolution.getFitnessValue();
		if(curr >= bet1)
		{
			pos1 = i;
			break;
		}
	}
	if(pos1 == -1)
	{
		pos1 = (begin_pos - 1 + pSolution.size()) % pSolution.size();
	}
	do
	{
		curr = 0;
		bet2 = (double)RandomFloat(0, sumFitnessValue);
		begin_pos = RandomInt(0, pSolution.size());
		for(i = begin_pos, j = 0; j < pSolution.size(); j++, i = (i + 1) % pSolution.size())
		{
		//FIXME:�����Ӧ�Ⱥ����޸ģ�����Ҳ����Ҫ����Ӧ���޸�
			curr += BestSolution.getFitnessValue() / pSolution[i].getFitnessValue();
			//curr += pSolution[i].getFitnessValue()/BestSolution.getFitnessValue();
			if(curr >= bet2)
			{
				pos2 = i;
				break;
			}
		}
	}while(pos1 == pos2 || pos2 == -1);
	if(pos1 != -1 && pos2 != -1)
	{
		current_op1 = pos1;
		current_op2 = pos2;
	}
	else
	{
		cout<<endl<<"Error: CGA::SelectTwoByRoulette() "<<"pos1: "<<pos1<<", pos2: "<<pos2<<endl;
		_PAUSE;
	}
}

//�ҵ���ǰ��Ⱥ����Ӧ����õĸ��壬����BestSolution
//��������BestUnImproved����ÿ�ε������̵�������һ�Σ�ע�ⲻ���ظ�����
bool CGA::FindBestSolution()
{
	unsigned int i;
	bool improved = false;
	double best_fitness_value = 0;
	this->sumFitnessValue = 0;
	for (i=0; i < pSolution.size(); i++)
	{
		if(CGASolution::betterThan( pSolution[i].getFitnessValue(), BestSolution.getFitnessValue() ) )
		{
			BestSolution = pSolution[i];
			improved = true;
		}
	}
	if (improved)
	{
			NumberofUnImproved = 0;
			//BestSolution.PrintSolution();
	}
	else
		NumberofUnImproved++;
			
	best_fitness_value = BestSolution.getFitnessValue();
	for (i=0; i < pSolution.size(); i++)
	{
		//���������Ӧ�ȵ��ܺͣ��������̶�ѡ��
		//FIXME:�����Ӧ�Ⱥ����޸ģ�����Ҳ����Ҫ����Ӧ���޸�
		this->sumFitnessValue += BestSolution.getFitnessValue() / pSolution[i].getFitnessValue();
	}
	return improved;
}

//�ҵ���ǰ��Ⱥ����Ӧ�������������������滻
//��<var>worst_first</var>��<var>worst_second</var>ָ������
void CGA::FindWorstSolution()
{
	unsigned int i;
	unsigned int bet = -1;
	unsigned int worst1 = -1, worst2 = -1;
	double value_worst1 = BEST_FITNESS;
	double value_worst2 = BEST_FITNESS;
	vector<int> tmp;
	for (i = 0; i < pSolution.size(); i++)
	{
		if( CGASolution::worseThan( pSolution[i].getFitnessValue(), value_worst1 ) )
		{
			worst1 = i;
			value_worst1 = pSolution[i].getFitnessValue();
			tmp.clear();
			tmp.push_back(i);
		}
		else if( CGASolution::equalTo( pSolution[i].getFitnessValue(), value_worst1 ) )
		{
			tmp.push_back(i);
		}
	}
	if(tmp.size() > 1)
	{
		bet = RandomInt(0, tmp.size());
		worst1 = tmp.at(bet);
		bet = RandomInt(0, tmp.size());
		worst2 = tmp.at(bet);
	}
	else
	{
		tmp.clear();
		for (i = 0; i < pSolution.size(); i++)
		{
			if(i == worst1)
				continue;
			if( CGASolution::worseThan( pSolution[i].getFitnessValue(), value_worst2 ) )
			{
				worst2 = i;
				value_worst2 = pSolution[i].getFitnessValue();
				tmp.clear();
				tmp.push_back(i);
			}
			else if( CGASolution::equalTo( pSolution[i].getFitnessValue(), value_worst2 ) )
			{
				tmp.push_back(i);
			}
		}
		if(tmp.size() > 1)
		{
			bet = RandomInt(0, tmp.size());
			worst2 = tmp.at(bet);
		}
	}
	this->worst_first = worst1;
	this->worst_second = worst2;
}

bool CGA::ReplaceWorstSolution(CGASolution &output1, CGASolution &output2)
{
		FindWorstSolution();
		bool improved = false;
		CGASolution *better, *worse;
		if( CGASolution::betterThan(output1.getFitnessValue(), output2.getFitnessValue()) )
		{
			better = &output1;
			worse = &output2;
		}
		else
		{
			better = &output2;
			worse = &output1;
		}

		vector<CGASolution>::iterator replaced;
		if(! CGASolution::worseThan( worse->getFitnessValue(), pSolution[worst_first].getFitnessValue() )
			&& !IfExists(*worse) )
		{
//DBG:
			printf("*worse replace indiv %d, ", worst_second);
			//pSolution[worst_second].printDegrees();
			//worse->printDegrees();

			replaced = pSolution.begin() + worst_second;
			replaced = pSolution.erase(replaced);
			pSolution.insert(replaced, *worse);
			if(CGASolution::betterThan( worse->getFitnessValue(), pSolution[worst_second].getFitnessValue() ) )
				improved = true;
		}
		if(! CGASolution::worseThan( better->getFitnessValue(), pSolution[worst_first].getFitnessValue() )
			&& !IfExists(*better) )
		{
//DBG:
			printf("*better replace indiv %d\n", worst_first);
			//pSolution[worst_first].printDegrees();
			//better->printDegrees();

			replaced = pSolution.begin() + worst_first;
			replaced = pSolution.erase(replaced);
			pSolution.insert(replaced, *better);
			
			if(CGASolution::betterThan( better->getFitnessValue(), pSolution[worst_first].getFitnessValue() ) )
				improved = true;
		}

		return improved;
}

//FIXME: to be completed
void CGA::ReplaceBelowAverageSolution(CGASolution &output1, CGASolution &output2)
{

}

//Variable Mutation Rate (by Beasley)
void CGA::UpdateMutationRate()
{
	double newRate = CO_MUTATION_FINAL / ( 1 + exp( -4 * CO_MUTATION_GRADIENT * (NumberofSolution - CO_MUTATION_CHILDREN) / CO_MUTATION_FINAL) );
	MutatedBits = ROUND(newRate);	

//FIXME: at least one bit?
	if(MutatedBits == 0)
		MutatedBits = 1;
}

//�ж�һ�������Ƿ��뵱ǰ��Ⱥ�еĸ�����ͬ
bool CGA::IfExists(CGASolution it)
{
	unsigned int i;
	for(i = 0; i < pSolution.size(); i++)
	{
		if(pSolution[i].sameAs(it))
			return true;
	}
	return false;
}

//�ж��Ƿ���Ͻ������������ﵽMAX_UNIMPROVED_NUM��MAX_SOLUTION_NUM
bool CGA::OptCompleted()
{
	return ((NumberofSolution >= MAX_SOLUTION_NUM) && (NumberofUnImproved >= MAX_UNIMPROVED_NUM));
}

void CGA::finalstatus()
{
	cout<<"*************************************************"<<endl;
	cout<<"Number of Solutions is "<< NumberofSolution <<"."<<endl;
	cout<<"Number of UnImproved is "<< NumberofUnImproved <<"."<<endl;
	cout<<"*************************************************"<<endl;
	cout<<"Best Solution:"<<endl;
	cout<<BestSolution.toString()<<endl;
	cout<<"*************************************************"<<endl;
}
