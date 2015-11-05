#include "GASolution.h"
#include "Preprocessor.h"


CGASolution::CGASolution()
{
	if(g_nPositions != 0
		&& g_nHotspotCandidates != 0)
	{
		choices = (int *)calloc(g_nHotspotCandidates, sizeof(int));
		nCoverForPositions = (int *)calloc(g_nPositions, sizeof(int));
		nNewCoverForHotspots = (int *)calloc(g_nHotspotCandidates, sizeof(int));
	}
	if(choices == NULL
		|| nCoverForPositions == NULL
		|| nNewCoverForHotspots == NULL)
	{
		cout<<"Error: CGASolution::CGASolution() �ڴ����ʧ�ܣ�"<<endl;
		MEMORYSTATUS ms; //��¼���ݿռ���Ϣ�Ľṹ�����
		GlobalMemoryStatus(&ms);

		cout<<"�����ڴ��ܴ�С��"<<(float)ms.dwTotalPhys/1024/1024<<"MB"<<endl; //�ܵ������ڴ��С
		cout<<"���������ڴ��С��"<<(float)(ms.dwTotalPhys - ms.dwAvailPhys)/1024/1024<<"MB"<<endl; //���������ڴ��С
		cout<<"���������ڴ��С��"<<(float)ms.dwAvailPhys/1024/1024<<"MB"<<endl; //���������ڴ��С
		cout<<"�����ڴ��ܴ�С��"<<(float)ms.dwTotalVirtual/1024/1024<<"MB"<<endl; //�ܵ������ڴ��С
		cout<<"���������ڴ��С��"<<(float)(ms.dwTotalVirtual-ms.dwAvailVirtual)/1024/1024<<"MB"<<endl; //���������ڴ��С
		cout<<"���������ڴ��С��"<<(float)ms.dwAvailVirtual/1024/1024<<"MB"<<endl; //���������ڴ��С

		_PAUSE;
	}
	valid = false;
	cost = 0;
	birthGeneration = -1;
	rank = 0;
	fitness = WORST_FITNESS;

	typeCrossover = 0;
}

CGASolution::CGASolution(const CGASolution& it)
{
	if(g_nPositions != 0
		&& g_nHotspotCandidates != 0)
	{
		choices = (int *)calloc(g_nHotspotCandidates, sizeof(int));
		nCoverForPositions = (int *)calloc(g_nPositions, sizeof(int));
		nNewCoverForHotspots = (int *)calloc(g_nHotspotCandidates, sizeof(int));
	}
	if(choices == NULL
		|| nCoverForPositions == NULL
		|| nNewCoverForHotspots == NULL)
	{
		cout<<"Error: CGASolution::CGASolution() �ڴ����ʧ�ܣ�"<<endl;
		_PAUSE;
	}

	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		this->choices[i] = it.choices[i];
		this->nNewCoverForHotspots[i] = it.nNewCoverForHotspots[i];
	}
	for(int i = 0; i < g_nPositions; i++)
	{
		this->nCoverForPositions[i] = it.nCoverForPositions[i];
	}

	this->valid = it.valid;
	this->cost = it.cost;
	this->birthGeneration = it.birthGeneration;
	this->rank = it.rank;
	this->fitness = it.fitness;

	this->typeCrossover = it.typeCrossover;
}

const CGASolution& CGASolution::operator=(const CGASolution& it)
{
	if(this == &it)
		return *this;
	if(choices != NULL)
	{
		free(choices);
		choices = NULL;
	}
	if(nCoverForPositions != NULL)
	{
		free(nCoverForPositions);
		nCoverForPositions = NULL;
	}
	if(nNewCoverForHotspots != NULL)
	{
		free(nNewCoverForHotspots);
		nNewCoverForHotspots = NULL;
	}

	if(g_nPositions != 0
		&& g_nHotspotCandidates != 0)
	{
		choices = (int *)calloc(g_nHotspotCandidates, sizeof(int));
		nCoverForPositions = (int *)calloc(g_nPositions, sizeof(int));
		nNewCoverForHotspots = (int *)calloc(g_nHotspotCandidates, sizeof(int));
	}
	if(choices == NULL
		|| nCoverForPositions == NULL
		|| nNewCoverForHotspots == NULL)
	{
		cout<<"Error: CGASolution::operator=() �ڴ����ʧ�ܣ�"<<endl;
		MEMORYSTATUS ms; //��¼���ݿռ���Ϣ�Ľṹ�����
		GlobalMemoryStatus(&ms);

		cout<<"�����ڴ��ܴ�С��"<<(float)ms.dwTotalPhys/1024/1024<<"MB"<<endl; //�ܵ������ڴ��С
		cout<<"���������ڴ��С��"<<(float)(ms.dwTotalPhys - ms.dwAvailPhys)/1024/1024<<"MB"<<endl; //���������ڴ��С
		cout<<"���������ڴ��С��"<<(float)ms.dwAvailPhys/1024/1024<<"MB"<<endl; //���������ڴ��С
		cout<<"�����ڴ��ܴ�С��"<<(float)ms.dwTotalVirtual/1024/1024<<"MB"<<endl; //�ܵ������ڴ��С
		cout<<"���������ڴ��С��"<<(float)(ms.dwTotalVirtual-ms.dwAvailVirtual)/1024/1024<<"MB"<<endl; //���������ڴ��С
		cout<<"���������ڴ��С��"<<(float)ms.dwAvailVirtual/1024/1024<<"MB"<<endl; //���������ڴ��С

		_PAUSE;
	}

	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		this->choices[i] = it.choices[i];
		this->nNewCoverForHotspots[i] = it.nNewCoverForHotspots[i];
	}
	for(int i = 0; i < g_nPositions; i++)
	{
		this->nCoverForPositions[i] = it.nCoverForPositions[i];
	}

	this->valid = it.valid;
	this->cost = it.cost;
	this->birthGeneration = it.birthGeneration;
	this->rank = it.rank;
	this->fitness = it.fitness;

	this->typeCrossover = it.typeCrossover;

	return *this;
}

CGASolution::~CGASolution(void)
{
	if(choices != NULL)
		free(choices);
	choices = NULL;
	if(nCoverForPositions != NULL)
		free(nCoverForPositions);
	nCoverForPositions = NULL;
	if(nNewCoverForHotspots != NULL)
		free(nNewCoverForHotspots);
	nNewCoverForHotspots = NULL;

	//cout<<"CGASolution������"<<endl;
}

bool CGASolution::sameAs(CGASolution it)
{
	int* hotspots_1 = this->choices;
	int* hotspots_2 = it.choices;
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		if(hotspots_1[i] != hotspots_2[i])
			return false;
	}
	return true;
}

bool CGASolution::betterThan(double left, double right)
{
	return ( left < right );
}

bool CGASolution::worseThan(double left, double right)
{
	return ( left > right );
}

bool CGASolution::equalTo(double left, double right)
{
	return ( left == right );
}

void CGASolution::BuildSolutionRandomly()
{
	memset(choices, 0, g_nHotspotCandidates * sizeof(int));
	for(int i = 0; i < g_nPositions; i++)
	{
		vector<int> tmp_hotspots = getHotspotsForPosition(i);
		int bet_hotspot = RandomInt(0, tmp_hotspots.size());
		choices[tmp_hotspots[bet_hotspot]] = 1;
	}
	this->UpdateStatus();
	this->RemoveRedundantHotspotRandomly();

	birthGeneration = 0;
	//_ASSERTE(_CrtCheckMemory());
}

void CGASolution::UpdateStatus()
{
	memset(nCoverForPositions, 0, g_nPositions * sizeof(int));
	memset(nNewCoverForHotspots, 0, g_nHotspotCandidates * sizeof(int));

	int hotspotCount = 0;
	//��������ѡ�е�hotspot
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		if(choices[i] == 0)
			continue;
		if(choices[i] < 0)
		{
			cout<<"Error: CGASolution::UpdateStatus() choices["<<i<<"] = "<<choices[i]<<endl;
			_PAUSE;
		}

		hotspotCount++;
		for(int j = 0; j < g_nPositions; j++)
		{
			int tmp = g_coverMatrix[j][i];
			////������position���״α����ǣ��������hotspot��nNewCoverForHotspotֵ
			//if(nCoverForPositions[j] == 0 
			//	&& tmp == 1)
			//	nNewCoverForHotspots[i]++;
			//����position�ĸ��Ǽ���
			nCoverForPositions[j] += tmp;
		}
	}
	this->cost = hotspotCount;  //����cost


	this->valid = true;
	for(int i = 0; i < g_nPositions; i++)
	{
		if(nCoverForPositions[i] == 0)
		{
			this->valid = false;  //�������һ��positionδ�����ǣ���valid��Ϊfalse
			break;
		}
	}

	//FIXME:
	//������Ӧ��
	this->fitness = cost;

	//_ASSERTE(_CrtCheckMemory());
}

vector<int> CGASolution::getHotspotsForPosition(int index_pos)
{
	vector<int> result;
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		if(g_coverMatrix[index_pos][i] == 1)
			result.push_back(i);
	}
//DBG:
	if(result.empty())
	{
		cout<<"Error: CGASolution::getHotspotsForPosition() no hotspots found for g_positions["<<index_pos<<"]"<<endl;
		_PAUSE;
	}
	return result;
}

vector<int> CGASolution::getPositionsForHotspot(int index_hotspot)
{
	vector<int> result;
	for(int i = 0; i < g_nPositions; i++)
	{
		if(g_coverMatrix[i][index_hotspot] == 1)
			result.push_back(i);
	}
//DBG:
	if(result.empty())
	{
		cout<<"Error: CGASolution::getPositionsForHotspot() no positions found for g_hotspotCandidates["<<index_hotspot<<"]"<<endl;
		_PAUSE;
	}
	return result;
}

void CGASolution::GreedyFix()
{
	if(this->valid)
		return;

	for(int i = 0; i < g_nPositions; i++)
	{
		if(nCoverForPositions[i] > 0)
			continue;

		vector<int> tmp_hotspots = getHotspotsForPosition(i);
		int ihotspot = tmp_hotspots.at(tmp_hotspots.size() - 1);  //ѡȡcover������hotspot
		
		choices[ihotspot] = 1;
		vector<int> tmp_positions = getPositionsForHotspot(ihotspot);
		for(int j = 0; j < tmp_positions.size(); j++)
		{
			nCoverForPositions[tmp_positions[j]]++;
		}
	}

	this->UpdateStatus();
}

void CGASolution::RemoveRedundantHotspotRandomly()
{
	bool redundant = true;
//DBG:
	//printDegrees();
	vector<int> tmp_order;
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		tmp_order.push_back(i);
	}
	while(! tmp_order.empty())
	{
		//���ѡ��һ��hotspot���м��
		int bet = RandomInt(0, tmp_order.size());
		int i_hotspot = tmp_order[bet];

		if(choices[i_hotspot] == 1)
		{
			//����Ƿ�����
			redundant = true;
			vector<int> tmp_positions = getPositionsForHotspot(i_hotspot);
			for(int j = 0; j < tmp_positions.size(); j++)
			{
				if(nCoverForPositions[tmp_positions[j]] < 2)
				{
					redundant = false;
					break;
				}
			}
			//������࣬��ӽ���ɾ�������������position�ĸ�����
			if(redundant)
			{
				choices[i_hotspot] = 0;
				for(int k = 0; k < tmp_positions.size(); k++)
				{
					nCoverForPositions[tmp_positions[k]]--;
				}
//DBG:
				//printDegrees();
			}
		}
		else if(choices[i_hotspot] < 0)
		{
			cout<<"Error: CGASolution::RemoveRedundantHotspotsRandomly() choices["<<i_hotspot<<"] = "<<choices[i_hotspot]<<endl;
			_PAUSE;
		}
		//Ĩ���Ѽ������±�
		vector<int>::iterator tmp_iterator = tmp_order.begin();
		tmp_order.erase(tmp_iterator + bet);
	}
	this->UpdateStatus();
	//_ASSERTE(_CrtCheckMemory());
}

void CGASolution::FusionCrossover(CGASolution &parent_1, CGASolution &parent_2, CGASolution &output1)
{
	int* choices_out = output1.getChoices();
	int* choices_p1 = parent_1.getChoices();
	int* choices_p2 = parent_2.getChoices();

	//����ⱻ���Ƶĸ�����cost�ɷ���
	double threshold = parent_2.getFitnessValue() / (parent_1.getFitnessValue() + parent_2.getFitnessValue());

	memset(choices_out, 0, g_nHotspotCandidates * sizeof(int));
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		double bet = RandomFloat(0, 1);
		if(bet < threshold)
			choices_out[i] = choices_p1[i];
		else
			choices_out[i] = choices_p2[i];
		if(choices_out[i] < 0 || choices_out[i] > 1)
		{
			cout<<"Error: CGASolution::FusionCrossover() output1.choices["<<i<<"] = "<<choices_out[i]<<endl;
			_PAUSE;
		}
	}
	output1.UpdateStatus();

//DBG:
	//output1.printDegrees();

	//output1.GreedyFix();
	//output1.RemoveRedundantHotspotRandomly();
}

void CGASolution::UniformCrossover(CGASolution &parent_1, CGASolution &parent_2, CGASolution &output1)
{
	int* choices_out = output1.getChoices();
	int* choices_p1 = parent_1.getChoices();
	int* choices_p2 = parent_2.getChoices();

	memset(choices_out, 0, g_nHotspotCandidates * sizeof(int));
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		int bet = RandomInt(0, 2);
		if(bet == 0)
			choices_out[i] = choices_p1[i];
		else
			choices_out[i] = choices_p2[i];
	}
	output1.UpdateStatus();

//DBG:
	//output1.printDegrees();

	//output1.GreedyFix();
	//output1.RemoveRedundantHotspotRandomly();
}

void CGASolution::BitMutation(int nBits)
{
	if(nBits == 0)
		return;
	if(nBits > g_nHotspotCandidates)
		nBits = g_nHotspotCandidates;

	vector<int> bits = CPreprocessor::RandomIntList(0, g_nHotspotCandidates, nBits);
	for(vector<int>::iterator ibit = bits.begin(); ibit != bits.end(); ibit++)
		choices[*ibit] = 1 - choices[*ibit];

	this->UpdateStatus();

//DBG:
	//this->printDegrees();

	//this->GreedyFix();
	//this->RemoveRedundantHotspotRandomly();
}

string CGASolution::toString()
{
	ostringstream os;
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		if(choices[i] == 1)
			os<<g_hotspotCandidates[i]->toString(false)<<endl;
		else if(choices[i] < 0)
		{
			cout<<"Error: CGASolution::toString() choices["<<i<<"] = "<<choices[i]<<endl;
			_PAUSE;
		}
	}
	os<<"cost: "<<this->cost<<endl;
	return os.str();
}

void CGASolution::printDegrees()
{
	for(int i = 0; i < g_nHotspotCandidates; i++)
	{
		cout<<choices[i]<<" ";
		if(choices[i] < 0 || choices[i] > 1)
			_PAUSE;
	}
	cout<<endl;
	for(int i = 0; i < g_nPositions; i++)
	{
		cout<<nCoverForPositions[i]<<" ";
		if(nCoverForPositions[i] < 0)
			_PAUSE;
	}
	cout<<endl;
}