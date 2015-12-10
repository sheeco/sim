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

	//�������нڵ㣬��ȡ��ǰλ�ã�������Ӧ��CPosition�࣬��ӵ�CPosition::positions��
	for(vector<int>::iterator i = CNode::idNodes.begin(); i != CNode::idNodes.end(); i++)
	{
		tmp_pos = new CPosition();
		double x = 0, y = 0;
		CFileParser::getPositionFromFile(*i, time, x, y);
		tmp_pos->setLocation(x, y, time);
		tmp_pos->setNode( *i );
		tmp_pos->generateID();
		if(tmp_pos->getID() == -1)
		{
			cout<<endl<<"Error: CPreprocessor::BuildCandidateHotspots() �ļ���������"<<endl;
			_PAUSE;
			break;
		}
		else
		{
			CPosition::positions.push_back(tmp_pos);
		}
	}
	
	//IHAR: ɾ�����ڵ�position��¼
	if(DO_IHAR)
	{
		int threshold = time - MAX_MEMORY_TIME;
		if(threshold > 0)
		{
			for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
			{
				if((*ipos)->getTime() < threshold)
				{
					delete *ipos;
					ipos = CPosition::positions.erase(ipos);
				}
				else
					ipos++;
			}
		}
	}

	CPosition::nPositions = CPosition::positions.size();
}

bool CPreprocessor::ascendByLocationX(CHotspot *left, CHotspot *right)
{
	return (*left > *right);
}

bool CPreprocessor::ascendByRatio(CHotspot *left, CHotspot *right)
{
	//��ִ��ǿ������ת����ֻ�ܴ���CHotspot��
	//����ratio�ļ���
	return ( ( (CHotspot *) left)->calculateRatio() ) > ( ( (CHotspot *) right)->calculateRatio() );
}

template <class E>
vector<E> CPreprocessor::merge(vector<E> &left, vector<E> &right, bool(*Comp)(E, E))
{
	vector<E> result;
	vector<E>::size_type li = 0;
	vector<E>::size_type ri = 0;
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

template <class E>
vector<E> CPreprocessor::mergeSort(vector<E> &v, bool(*Comp)(E, E))
{
	if(v.size() == 0)
		return vector<E>();
	if(v.size() == 1)
		return vector<E>(1, v[0]);

	vector<E>::iterator mid = v.begin() + v.size() / 2;
	vector<E> left(v.begin(), mid);
	vector<E> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

vector<CData> CPreprocessor::merge(vector<CData> &left, vector<CData> &right, bool(*Comp)(CData, CData))
{
	vector<CData> result;
	vector<CData>::size_type li = 0;
	vector<CData>::size_type ri = 0;
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

vector<CData> CPreprocessor::mergeSort(vector<CData> &v, bool(*Comp)(CData, CData))
{
	if(v.size() == 0)
		return vector<CData>();
	if(v.size() == 1)
		return vector<CData>(1, v[0]);

	vector<CData>::iterator mid = v.begin() + v.size() / 2;
	vector<CData> left(v.begin(), mid);
	vector<CData> right(mid, v.end());
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

vector<CHotspot *> CPreprocessor::merge(vector<CHotspot *> &left, vector<CHotspot *> &right, bool(*Comp)(CHotspot *, CHotspot *))
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

vector<CHotspot *> CPreprocessor::mergeSort(vector<CHotspot *> &v, bool(*Comp)(CHotspot *, CHotspot *))
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

vector<CHotspot> CPreprocessor::mergeByDeliveryCount(vector<CHotspot> &left, vector<CHotspot> &right, int endTime)
{
	vector<CHotspot> result;
	vector<CHotspot>::size_type li = 0;
	vector<CHotspot>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		int lv = left[li].getDeliveryCount(endTime);
		int rv = right[ri].getDeliveryCount(endTime);
		if( lv < rv )
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

vector<CHotspot> CPreprocessor::mergeSortByDeliveryCount(vector<CHotspot> &v, int endTime)
{
	if(v.size() == 0)
		return vector<CHotspot>();
	if(v.size() == 1)
		return vector<CHotspot>(1, v[0]);

	vector<CHotspot>::iterator mid = v.begin() + v.size() / 2;
	vector<CHotspot> left(v.begin(), mid);
	vector<CHotspot> right(mid, v.end());
	left = mergeSortByDeliveryCount(left, endTime);
	right = mergeSortByDeliveryCount(right, endTime);

	return mergeByDeliveryCount(left, right, endTime);
}

int CPreprocessor::getIndexOfPosition(CPosition* pos)
{
	for(int i = 0; i < CPosition::nPositions; i++)
	{
		if(CPosition::positions[i] == pos)
			return i;
	}

	return -1;
}

CHotspot* CPreprocessor::GenerateHotspotFromPosition(CPosition *pos, int time)
{
	CHotspot* result = new CHotspot(pos, time);
	result->generateID();

	//����flag
	for(int i = 0; i < CPosition::nPositions; i++)
		CPosition::positions[i]->setFlag(false);
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
			if(CPosition::positions[i]->getFlag())
				continue;
			//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
			if(fabs(result->getX() - CPosition::positions[i]->getX()) > TRANS_RANGE)
				break;
			if(CBasicEntity::getDistance(*result, *CPosition::positions[i]) <= TRANS_RANGE)
			{
				result->addPosition(CPosition::positions[i]);
				CPosition::positions[i]->setFlag(true);
				modified = true;
			}
		}
		for(int i = index_pos + 1; i < CPosition::nPositions; i++)
		{
			if(CPosition::positions[i]->getFlag())
				continue;
			//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
			if(fabs(CPosition::positions[i]->getX() - result->getX()) > TRANS_RANGE)
				break;
			if(CBasicEntity::getDistance(*result, *CPosition::positions[i]) <= TRANS_RANGE)
			{
				result->addPosition(CPosition::positions[i]);
				CPosition::positions[i]->setFlag(true);
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
	for(int i = 0; i < CPosition::nPositions; i++)
		CPosition::positions[i]->setFlag(false);

	bool modified;
	//ѭ����ֱ��û���µ�position������
	do
	{
		modified = false;
		//���µ�hotspot���ģ��ٴα���position
		for(int i = 0; i < CPosition::nPositions; i++)
		{
			if(CPosition::positions[i]->getFlag())
				continue;
			if( CPosition::positions[i]->getX() + TRANS_RANGE < hotspot->getX() )
				continue;
			//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
			if( hotspot->getX() + TRANS_RANGE < CPosition::positions[i]->getX() )
				break;
			if(CBasicEntity::getDistance(*hotspot, *CPosition::positions[i]) <= TRANS_RANGE)
			{
				hotspot->addPosition(CPosition::positions[i]);
				CPosition::positions[i]->setFlag(true);
				modified = true;
			}
		}

		//���¼���hotspot������
		if(modified)
			hotspot->recalculateCenter();
	}while(modified);

	return hotspot;
}

void CPreprocessor::DecayPositionsWithoutDeliveryCount()
{
	vector<CPosition*> badPositions;
	if( CHotspot::oldSelectedHotspots.empty() )
		return;

	for(vector<CHotspot*>::iterator ihotspot = CHotspot::oldSelectedHotspots.begin(); ihotspot != CHotspot::oldSelectedHotspots.end(); )
	{
		if( (*ihotspot)->getDeliveryCount(currentTime) == 0 )
		{
			addToListUniquely( badPositions, (*ihotspot)->getCoveredPositions() );
			//free(*ihotspot);
			//��mHAR�У�Ӧ�ÿ����Ƿ���Щ�ȵ��ų��ڹ鲢֮��
			//CHotspot::deletedHotspots.push_back(*ihotspot);
			//ihotspot = CHotspot::oldSelectedHotspots.erase(ihotspot);
			ihotspot++;
		}
		else
			ihotspot++;
	}
	for(vector<CPosition*>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
	{
		if( ifExists(badPositions, *ipos) )
		{
			(*ipos)->decayWeight();
			//Reduce complexity
			RemoveFromList(badPositions, *ipos);
			//���Ȩֵ������Сֵ��ֱ��ɾ����MIN_POSITION_WEIGHTĬ��ֵΪ1��������ɾ���κ�position
			if( (*ipos)->getWeight() < MIN_POSITION_WEIGHT )
			{
				CPosition::deletedPositions.push_back(*ipos);
				ipos = CPosition::positions.erase(ipos);
			}
			else
				ipos++;
		}
		else
			ipos++;
	}
}

void CPreprocessor::BuildCandidateHotspots(int time)
{
	//�ͷ���һ��ѡȡ��δ��ѡ�еķ����ȵ�
	if(! CHotspot::hotspotCandidates.empty())
		freePointerVector(CHotspot::hotspotCandidates);

	/************ ע�⣺����ִ��HAR, IHAR, merge-HAR����������һ���ȵ�ѡȡ�Ľ����
	              HAR�в���ʹ�õ���IHAR�н����ڱȽ�ǰ������ѡȡ���ȵ�����ƶȣ�
			      merge-HAR�н������ȵ�鲢��                            ********************/

	//����һ��ѡ�е��ȵ㼯�ϱ��浽CHotspot::oldSelectedHotspots
	//���ͷžɵ�CHotspot::oldSelectedHotspots
	if(! CHotspot::oldSelectedHotspots.empty())
		freePointerVector(CHotspot::oldSelectedHotspots);
	CHotspot::oldSelectedHotspots = CHotspot::selectedHotspots;
	//�����g_selectedHotspot�����ͷ��ڴ�
	CHotspot::selectedHotspots.erase(CHotspot::selectedHotspots.begin(), CHotspot::selectedHotspots.end());


	//������position��x���������Ա�򻯱�������
	CPosition::positions = mergeSort(CPosition::positions);

	//��ÿ��position��������һ����ѡhotspot
	for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); ipos++)
		CHotspot::hotspotCandidates.push_back(GenerateHotspotFromPosition(*ipos, time));

	////�����к�ѡhotspot��x��������
	//CHotspot::hotspotCandidates = mergeSort(CHotspot::hotspotCandidates, ascendByLocationX);

	//�����к�ѡhotspot��ratio������С����
	CHotspot::hotspotCandidates = mergeSort(CHotspot::hotspotCandidates, ascendByRatio);

}

void CPreprocessor::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
{
	ofstream toFile("hotspots.txt");
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
}
