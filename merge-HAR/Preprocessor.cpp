#include "Preprocessor.h"

CPreprocessor::CPreprocessor(void)
{
}

CPreprocessor::~CPreprocessor(void)
{
}

vector<int> CPreprocessor::RandomIntList(int min, int max, int size)
{
	vector<int> result;
	int tmp = -1;
	if(size > (max - min))
	{
		vector<int> tmp_order;
		for(int i = min; i < max; i++)
		{
			tmp_order.push_back(i);
		}
		while(! tmp_order.empty())
		{
			vector<int>::iterator it = tmp_order.begin();
			int bet = RandomInt(0, tmp_order.size());
			tmp = tmp_order.at(bet);
			result.push_back(tmp);
			tmp_order.erase(it + bet);
		}
		return result;
	}

	bool duplicate;
	if(size == 1)
	{
		result.push_back(min);
		return result;
	}

	for(int i = 0; i < size; i++)
	{
		do
		{
			duplicate = false;
			tmp = RandomInt(min, max);
			for(int j = 0; j < result.size(); j++)
			{
				if(result[j] == tmp)
				{
					duplicate = true;
					break;
				}
			}
		}while(duplicate);
		if(tmp < 0)
		{
			cout<<"Error: RandomIntList() tmp < 0"<<endl;
			_PAUSE;
		}
		else
			result.push_back(tmp);
	}
	return result;
}

void CPreprocessor::freePointerVector(vector<CPosition *> &v)
{
	vector<CPosition *>::iterator it;
	for(it = v.begin(); it != v.end(); it++)
	{
		if(*it != NULL)
			delete *it;
	}
	v.clear();
}

void CPreprocessor::freePointerVector(vector<CHotspot *> &v)
{
	vector<CHotspot *>::iterator it;
	for(it = v.begin(); it != v.end(); it++)
	{
		if(*it != NULL)
			delete *it;
	}
	v.clear();
}

void CPreprocessor::CollectNewPositions(int time)
{
	CPosition* tmp_pos = NULL;

	//�������нڵ㣬��ȡ��ǰλ�ã�������Ӧ��CPosition�࣬��ӵ�g_positions��
	for(int i = 0; i < NUM_NODE; i++)
	{
		tmp_pos = new CPosition();
		double x = 0, y = 0;
		CFileParser::getPositionFromFile(i, time, x, y);
		tmp_pos->setLocation(x, y, time);
		tmp_pos->setNode(i);
		tmp_pos->generateID();
		if(tmp_pos->getID() == -1)
		{
			cout<<endl<<"Error: CPreprocessor::BuildCandidateHotspots() �ļ���������"<<endl;
			_PAUSE;
			break;
		}
		else
		{
			g_positions.push_back(tmp_pos);
		}
	}
	
	//IHAR: 
	if(DO_IHAR)
	{
		int threshold = time - MAX_MEMORY_TIME;
		if(threshold > 0)
		{
			for(vector<CPosition *>::iterator ipos = g_positions.begin(); ipos != g_positions.end(); )
			{
				if((*ipos)->getTime() < threshold)
				{
					delete *ipos;
					ipos = g_positions.erase(ipos);
				}
				else
					ipos++;
			}
		}
	}

	g_nPositions = g_positions.size();
}

bool CPreprocessor::largerByLocationX(CBase *left, CBase *right)
{
	return (*left > *right);
}

bool CPreprocessor::largerByNCoveredPositions(CBase *left, CBase *right)
{
	//��ִ��ǿ������ת����ֻ�ܴ���CHotspot��
	return ( ( (CHotspot *) left)->getNCoveredPosition() ) > ( ( (CHotspot *) right)->getNCoveredPosition() );
}

vector<int> CPreprocessor::merge(vector<int> &left, vector<int> &right, bool(*Comp)(int, int))
{
	vector<int> result;
	vector<int>::size_type li = 0;
	vector<int>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if(! Comp(left[li], right[ri]) )
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<int> CPreprocessor::mergeSort(vector<int> &v, bool(*Comp)(int, int))
{
	if(v.size() == 0)
		return vector<int>();
	if(v.size() == 1)
		return vector<int>(1, v[0]);

	vector<int>::iterator mid = v.begin() + v.size() / 2;
	vector<int> left(v.begin(), mid);
	vector<int> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

vector<CPosition *> CPreprocessor::merge(vector<CPosition *> &left, vector<CPosition *> &right)
{
	vector<CPosition *> result;
	vector<CPosition *>::size_type li = 0;
	vector<CPosition *>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if(*left[li] > *right[ri])
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CPosition *> CPreprocessor::mergeSort(vector<CPosition *> &v)
{
	if(v.size() == 0)
		return vector<CPosition *>();
	if(v.size() == 1)
		return vector<CPosition *>(1, v[0]);

	vector<CPosition *>::iterator mid = v.begin() + v.size() / 2;
	vector<CPosition *> left(v.begin(), mid);
	vector<CPosition *> right(mid, v.end());
	left = mergeSort(left);
	right = mergeSort(right);

	return merge(left, right);
}

vector<CHotspot *> CPreprocessor::merge(vector<CHotspot *> &left, vector<CHotspot *> &right, bool(*Comp)(CBase *, CBase *))
{
	vector<CHotspot *> result;
	vector<CHotspot *>::size_type li = 0;
	vector<CHotspot *>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if( Comp(left[li], right[ri]) )
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CHotspot *> CPreprocessor::mergeSort(vector<CHotspot *> &v, bool(*Comp)(CBase *, CBase *))
{
	if(v.size() == 0)
		return vector<CHotspot *>();
	if(v.size() == 1)
		return vector<CHotspot *>(1, v[0]);

	vector<CHotspot *>::iterator mid = v.begin() + v.size() / 2;
	vector<CHotspot *> left(v.begin(), mid);
	vector<CHotspot *> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

vector<CGASolution> CPreprocessor::merge(vector<CGASolution> &left, vector<CGASolution> &right, bool(*Comp)(CGASolution, CGASolution))
{
	vector<CGASolution> result;
	vector<CGASolution>::size_type li = 0;
	vector<CGASolution>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if(! Comp(left[li], right[ri]) )
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CGASolution> CPreprocessor::mergeSort(vector<CGASolution> &v, bool(*Comp)(CGASolution, CGASolution))
{
	if(v.size() == 0)
		return vector<CGASolution>();
	if(v.size() == 1)
		return vector<CGASolution>(1, v[0]);

	vector<CGASolution>::iterator mid = v.begin() + v.size() / 2;
	vector<CGASolution> left(v.begin(), mid);
	vector<CGASolution> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

int CPreprocessor::getIndexOfPosition(CPosition* pos)
{
	for(int i = 0; i < g_nPositions; i++)
	{
		if(g_positions[i] == pos)
			return i;
	}

	return -1;
}

CHotspot* CPreprocessor::GenerateHotspotFromPosition(CPosition *pos, int time)
{
	CHotspot* result = new CHotspot(pos, time);
	result->generateID();

	//����flag
	for(int i = 0; i < g_nPositions; i++)
		g_positions[i]->setFlag(false);
	pos->setFlag(true);

	int index_pos = getIndexOfPosition(pos);
	bool modified;
	//ѭ����ֱ��û���µ�position������
	do
	{
		modified = false;
		//���µ�hotspot���ģ��ٴα���position
		for(int i = index_pos - 1; i >= 0; i--)
		{
			if(g_positions[i]->getFlag())
				continue;
			//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
			if(fabs(result->getX() - g_positions[i]->getX()) > TRANS_RANGE)
				break;
			if(CBase::getDistance(*result, *g_positions[i]) <= TRANS_RANGE)
			{
				result->addPosition(g_positions[i]);
				g_positions[i]->setFlag(true);
				modified = true;
			}
		}
		for(int i = index_pos + 1; i < g_nPositions; i++)
		{
			if(g_positions[i]->getFlag())
				continue;
			//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
			if(fabs(g_positions[i]->getX() - result->getX()) > TRANS_RANGE)
				break;
			if(CBase::getDistance(*result, *g_positions[i]) <= TRANS_RANGE)
			{
				result->addPosition(g_positions[i]);
				g_positions[i]->setFlag(true);
				modified = true;
			}
		}

		//���¼���hotspot������
		if(modified)
			result->recalculateCenter();
	}while(modified);

	return result;
}

CHotspot* CPreprocessor::GenerateHotspotFromCoordinates(double x, double y, int time)
{
	CHotspot* hotspot = new CHotspot();
	hotspot->setX(x);
	hotspot->setY(y);
	hotspot->setTime(time);
	hotspot->generateID();

	//����flag
	for(int i = 0; i < g_nPositions; i++)
		g_positions[i]->setFlag(false);

	bool modified;
	//ѭ����ֱ��û���µ�position������
	do
	{
		modified = false;
		//���µ�hotspot���ģ��ٴα���position
		for(int i = 0; i < g_nPositions; i++)
		{
			if(g_positions[i]->getFlag())
				continue;
			if( g_positions[i]->getX() + TRANS_RANGE < hotspot->getX() )
				continue;
			//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
			if( hotspot->getX() + TRANS_RANGE < g_positions[i]->getX() )
				break;
			if(CBase::getDistance(*hotspot, *g_positions[i]) <= TRANS_RANGE)
			{
				hotspot->addPosition(g_positions[i]);
				g_positions[i]->setFlag(true);
				modified = true;
			}
		}

		//���¼���hotspot������
		if(modified)
			hotspot->recalculateCenter();
	}while(modified);

	return hotspot;
}

void CPreprocessor::GenerateDegrees()
{
	if(g_degreeForPositions != NULL)
	{
		free(g_degreeForPositions);
		g_degreeForPositions = NULL;
	}
	if(g_degreeForHotspots != NULL)
	{
		free(g_degreeForHotspots);
		g_degreeForHotspots = NULL;
	}
	g_nPositions = g_positions.size();
	g_nHotspotCandidates = g_hotspotCandidates.size();
	g_degreeForPositions = (int *)calloc(g_nPositions, sizeof(int));
	g_degreeForHotspots = (int *)calloc(g_nHotspotCandidates, sizeof(int));
	
	if(g_degreeForHotspots == NULL || g_degreeForPositions == NULL)
	{
		cout<<endl<<"Error: CPreprocessor::GenerateDegrees() �ڴ����ʧ��"<<endl;
		_PAUSE;
		return;
	}
	//�������
	for(int ihotspot = 0; ihotspot < g_hotspotCandidates.size(); ihotspot++)
	{
		vector<CPosition *> tmp_positions = g_hotspotCandidates[ihotspot]->getCoveredPositions();
		g_degreeForHotspots[ihotspot] = tmp_positions.size();
		for(int i = 0; i < tmp_positions.size(); i++)
		{
			int ipos;
			ipos = getIndexOfPosition(tmp_positions[i]);
			if(ipos != -1)
				g_degreeForPositions[ipos]++;
		}
	}

//DBG:
	//for(int i = 0; i < g_nHotspotCandidates; i++)
	//	cout<<g_degreeForHotspots[i]<<" ";
	//cout<<endl;
	for(int i = 0; i < g_nPositions; i++)
	{
		if(g_degreeForPositions[i] == 0)
		{	
			cout<<"Error: CPreprocessor::GenerateDegrees() g_degreeForPositions["<<i<<"] == 0"<<endl;
			_PAUSE;
		}
	}
}

void CPreprocessor::GenerateCoverMatrix()
{
	//�ͷŶ�̬������ڴ�
	if(g_coverMatrix != NULL)
	{
		for(int i = 0; i < g_old_nPositions; i++)
		{
			if(g_coverMatrix[i] != NULL)
				delete[] g_coverMatrix[i];
		}
		delete[] g_coverMatrix;
		g_coverMatrix = NULL;
	}
	g_nPositions = g_positions.size();
	g_nHotspotCandidates = g_hotspotCandidates.size();
	//���·����ڴ�
	try
	{
		g_coverMatrix = new int*[g_nPositions];
		for(int i = 0; i < g_nPositions; i++)
		{
			g_coverMatrix[i] = new int[g_nHotspotCandidates];
			memset(g_coverMatrix[i], 0, g_nHotspotCandidates * sizeof(int));
		}
	}
	catch(exception e)
	{
		cout<<endl<<"Error: CPreprocessor::GenerateCoverMatrix() �ڴ����ʧ��"<<endl;
		_PAUSE;
		return;
	}
	//����cover����
	for(int ihotspot = 0; ihotspot < g_hotspotCandidates.size(); ihotspot++)
	{
		vector<CPosition *> tmp_positions = g_hotspotCandidates[ihotspot]->getCoveredPositions();
		for(int i = 0; i < tmp_positions.size(); i++)
		{
			int ipos;
			ipos = getIndexOfPosition(tmp_positions[i]);
			if(ipos == -1)
				continue;
//DBG:
			if(g_coverMatrix[ipos][ihotspot] == 1)
			{
				cout<<"Error: CPreprocessor::GenerateMatrix() g_coverMatrix["<<ipos<<"]["<<ihotspot<<"] == 1 ( has been set to 1 before)"<<endl;
				_PAUSE;
			}
			g_coverMatrix[ipos][ihotspot] = 1;
		}
	}

	//���浱ǰ�ľ����С�������´��ͷ��ڴ�
	g_old_nPositions = g_nPositions;
	g_old_nHotspots = g_nHotspotCandidates;
}

//ʵ����û��ʹ���������
void CPreprocessor::RemoveBadHotspots()
{
	if(g_hotspotCandidates.empty())
		return;

	int n_removed;
	//FIXME:
	//ɾ����Ŀ��ɾ�����Ծ���
	n_removed = g_nHotspotCandidates / 2;

	vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin();
	bool removed;
	//�������з��������ĺ�ѡhotspot
	for(int i = 0; i < n_removed; i++)
	{
		removed = true;
		vector<CPosition *> tmp_positions = (*ihotspot)->getCoveredPositions();
		for(vector<CPosition *>::iterator ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
		{
			int index_pos = getIndexOfPosition(*ipos);
			if(index_pos == -1)
				continue;

			if(g_degreeForPositions[index_pos] < 2)  //����Ƿ�����
			{
				removed = false;
				break;
			}
		}
		//����������hotspot��ִ��ɾ��
		if(removed)
		{
			//�޸�g_degreeForPositions
			for(vector<CPosition *>::iterator ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
			{
				int index_pos = getIndexOfPosition(*ipos);
				if(index_pos != -1)
					g_degreeForPositions[index_pos]--;
			}
			delete *ihotspot;
			ihotspot = g_hotspotCandidates.erase(ihotspot);  //ִ��ɾ�������»�ȡ������
		}
		else
		{
			ihotspot++;
		}
	}
	//�޸�hotspot�������������Ϣ
	GenerateDegrees();
	GenerateCoverMatrix();
}

//FIXME:�ų�������position�����GA�Ż�Ч�ʣ����͸��Ӷ�
void CPreprocessor::RemoveIsolatePositions()
{
	if(g_hotspotCandidates.empty())
		return;

	//����flag
	for(vector<CPosition *>::iterator ipos = g_positions.begin(); ipos != g_positions.end(); ipos++)
		(*ipos)->setFlag(false);
	for(vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); ihotspot++)
		(*ihotspot)->setFlag(false);
	//�������з��������ĺ�ѡhotspot
	for(vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); ihotspot++)
	{
		//FIXME:ֻ����cover��Ϊ1��hotspot
		if((*ihotspot)->getNCoveredPosition() > 1)
			break;
		if((*ihotspot)->getFlag())
			continue;

		//�����Ƿ�����
		vector<CPosition *> tmp_positions = (*ihotspot)->getCoveredPositions();
		bool redundant = true;
		vector<CPosition *>::iterator ipos;
		for(ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
		{
			int index_pos = getIndexOfPosition(*ipos);
			if(index_pos == -1)
				continue;
			if(g_degreeForPositions[index_pos] < 2)
			{
				redundant = false;
				break;
			}
		}
		//���hotspot������ģ�ֱ��ɾ��
		if(redundant)
			(*ihotspot)->setFlag(true);
		//���򣬱��cover��position���Ժ�����g_tmpPosition����ֱ��ѡ�����hotspot
		else
		{
			g_selectedHotspots.push_back(*ihotspot);  //ѡ��
			(*ihotspot)->setFlag(true);
			for(ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
				(*ipos)->setFlag(true);
		}
	}
	//����ǵ�hotspotɾ��
	for(vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); )
	{
		if((*ihotspot)->getFlag())
		{	
			ihotspot = g_hotspotCandidates.erase(ihotspot);
		}
		else
			ihotspot++;
	}
	//����ǵ�position����g_tmpPositions
	for(vector<CPosition *>::iterator ipos = g_positions.begin(); ipos != g_positions.end(); )
	{	
		if((*ipos)->getFlag())
		{
			g_tmpPositions.push_back(*ipos);
			ipos = g_positions.erase(ipos);
		}
		else
			ipos++;

	}
	//�޸�hotspot��position�������������Ϣ
	GenerateDegrees();
	GenerateCoverMatrix();	
}

void CPreprocessor::PutBackAllPositions()
{
	//FIXME:�ų�������position�����GA�Ż�Ч�ʣ����͸��Ӷ�
	//��֮ǰ��ʱɾ����position�Ż�
	if(! g_tmpPositions.empty())
	{
		g_positions = merge(g_positions, g_tmpPositions);
		g_tmpPositions.clear();
	}
}

void CPreprocessor::AdjustRemoteHotspots()
{
	if(g_hotspotCandidates.empty())
		return;

	vector<CHotspot *>::iterator ihotspot;
	//��������ֻ����һ��position�ĺ�ѡhotspot
	for(ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); ihotspot++)
	{
		if((*ihotspot)->getNCoveredPosition() > 1)
			break;
		
		int index_pos = getIndexOfPosition( ( (*ihotspot)->getCoveredPositions() ).at(0) );
		if(index_pos == -1)
			continue;

		double max_distance = 0;
		int pos_max_distance = -1;
		
		//Ѱ��2*range������Զ��position
		for(int i = index_pos - 1; i >= 0; i--)
		{
			//��ˮƽ�����ѳ���2*range�������ֱ��ֹͣ����
			if(fabs((*ihotspot)->getX() - g_positions[i]->getX()) > 2 * TRANS_RANGE)
				break;
			if(CBase::getDistance(**ihotspot, *g_positions[i]) <=  2 * TRANS_RANGE
				&& CBase::getDistance(**ihotspot, *g_positions[i]) > max_distance)
			{
				pos_max_distance = i;
			}
		}
		for(int i = index_pos + 1; i < g_nPositions; i++)
		{
			//��ˮƽ�����ѳ���2*range�������ֱ��ֹͣ����
			if(fabs((*ihotspot)->getX() - g_positions[i]->getX()) > 2 * TRANS_RANGE)
				break;
			if(CBase::getDistance(**ihotspot, *g_positions[i]) <=  2 * TRANS_RANGE
				&& CBase::getDistance(**ihotspot, *g_positions[i]) > max_distance)
			{
				pos_max_distance = i;
			}
		}

		//��û�з���������position������
		if(pos_max_distance == -1)
			continue;
		//���ҵ�����Զposition�������hotspot�ĸ����б���hotspot���ĵ������������е�
		(*ihotspot)->addPosition(g_positions[pos_max_distance]);
		(*ihotspot)->recalculateCenter();
		//����flag
		for(int i = 0; i < g_nPositions; i++)
			g_positions[i]->setFlag(false);
		g_positions[pos_max_distance]->setFlag(true);

		//ѭ����ֱ��û���µ�position������
		bool modified = false;
		do
		{
			modified = false;
			//���µ�hotspot���ģ��ٴα���position
			for(int i = pos_max_distance - 1; i >= 0; i--)
			{
				if(g_positions[i]->getFlag())
					continue;
				//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
				if(fabs((*ihotspot)->getX() - g_positions[i]->getX()) > TRANS_RANGE)
					break;
				if(CBase::getDistance(**ihotspot, *g_positions[i]) <= TRANS_RANGE)
				{
					(*ihotspot)->addPosition(g_positions[i]);
					g_positions[i]->setFlag(true);
					modified = true;
				}
			}
			for(int i = pos_max_distance + 1; i < g_nPositions; i++)
			{
				if(g_positions[i]->getFlag())
					continue;
				//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
				if(fabs(g_positions[i]->getX() - (*ihotspot)->getX()) > TRANS_RANGE)
					break;
				if(CBase::getDistance(**ihotspot, *g_positions[i]) <= TRANS_RANGE)
				{
					(*ihotspot)->addPosition(g_positions[i]);
					g_positions[i]->setFlag(true);
					modified = true;
				}
			}

			//���¼���hotspot������
			if(modified)
				(*ihotspot)->recalculateCenter();

		}while(modified);
	}

	//�������򣬸��������Ϣ
	g_hotspotCandidates = mergeSort(g_hotspotCandidates, largerByNCoveredPositions);
	GenerateDegrees();
	GenerateCoverMatrix();
}

void CPreprocessor::BuildCandidateHotspots(int time)
{
	//�ͷ���һ��ѡȡ��δ��ѡ�еķ����ȵ�
	if(! g_hotspotCandidates.empty())
		freePointerVector(g_hotspotCandidates);

	/************ ע�⣺����ִ��HAR, IHAR, merge-HAR����������һ���ȵ�ѡȡ�Ľ����
	              HAR�в���ʹ�õ���IHAR�н����ڱȽ�ǰ������ѡȡ���ȵ�����ƶȣ�
			      merge-HAR�н������ȵ�鲢��                            ********************/

	//����һ��ѡ�е��ȵ㼯�ϱ��浽g_oldSelectedHotspots
	//���ͷžɵ�g_oldSelectedHotspots
	if(! g_oldSelectedHotspots.empty())
		freePointerVector(g_oldSelectedHotspots);
	g_oldSelectedHotspots = g_selectedHotspots;
	//�����g_selectedHotspot�����ͷ��ڴ�
	g_selectedHotspots.erase(g_selectedHotspots.begin(), g_selectedHotspots.end());


	//������position��x���������Ա�򻯱�������
	g_positions = mergeSort(g_positions);

	//��ÿ��position��������һ����ѡhotspot
	for(vector<CPosition *>::iterator ipos = g_positions.begin(); ipos != g_positions.end(); ipos++)
		g_hotspotCandidates.push_back(GenerateHotspotFromPosition(*ipos, time));

	////�����к�ѡhotspot��x��������
	//g_hotspotCandidates = mergeSort(g_hotspotCandidates, largerByLocationX);

	//�����к�ѡhotspot��cover��������С����
	g_hotspotCandidates = mergeSort(g_hotspotCandidates, largerByNCoveredPositions);

	//���������Ϣ
	GenerateDegrees();
	////GA: 
	//GenerateCoverMatrix();
}

void CPreprocessor::UpdateDegrees()
{
	//���������Ϣ
	GenerateDegrees();
	//GA:������δ����GA�㷨���������cover�����ݲ����ɣ�
}

void CPreprocessor::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
{
	ofstream toFile("hotspots.txt");
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
}
