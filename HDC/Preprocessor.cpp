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
			cout<<"Error @ RandomIntList() : tmp < 0"<<endl;
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

bool CPreprocessor::ascendByLocationX(CHotspot *left, CHotspot *right)
{
	return (*left > *right);
}

bool CPreprocessor::ascendByRatio(CHotspot *left, CHotspot *right)
{
	//将执行强制类型转换，只能传入CHotspot类
	//包括ratio的计算
	return ( ( (CHotspot *) left)->calculateRatio() ) > ( ( (CHotspot *) right)->calculateRatio() );
}

template <class E>
vector<E> CPreprocessor::merge(vector<E> left, vector<E> right, bool(*Comp)(E, E))
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
vector<E> CPreprocessor::mergeSort(vector<E> v, bool(*Comp)(E, E))
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

vector<CData> CPreprocessor::merge(vector<CData> left, vector<CData> right, bool(*Comp)(CData, CData))
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

vector<CData> CPreprocessor::mergeSort(vector<CData> v, bool(*Comp)(CData, CData))
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

vector<CPosition *> CPreprocessor::merge(vector<CPosition *> left, vector<CPosition *> right)
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

vector<CPosition *> CPreprocessor::mergeSort(vector<CPosition *> v)
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

vector<CNode *> CPreprocessor::merge(vector<CNode *> left, vector<CNode *> right)
{
	vector<CNode *> result;
	vector<CNode *>::size_type li = 0;
	vector<CNode *>::size_type ri = 0;
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

vector<CNode *> CPreprocessor::mergeSort(vector<CNode *> v)
{
	if(v.size() == 0)
		return vector<CNode *>();
	if(v.size() == 1)
		return vector<CNode *>(1, v[0]);

	vector<CNode *>::iterator mid = v.begin() + v.size() / 2;
	vector<CNode *> left(v.begin(), mid);
	vector<CNode *> right(mid, v.end());
	left = mergeSort(left);
	right = mergeSort(right);

	return merge(left, right);
}

vector<CHotspot *> CPreprocessor::merge(vector<CHotspot *> left, vector<CHotspot *> right, bool(*Comp)(CHotspot *, CHotspot *))
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

vector<CHotspot *> CPreprocessor::mergeSort(vector<CHotspot *> v, bool(*Comp)(CHotspot *, CHotspot *))
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

vector<CHotspot> CPreprocessor::mergeByDeliveryCount(vector<CHotspot> left, vector<CHotspot> right, int endTime)
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

vector<CHotspot> CPreprocessor::mergeSortByDeliveryCount(vector<CHotspot> v, int endTime)
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
			//在mHAR中，应该考虑是否将这些热点排除在归并之外
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
			//如果权值低于最小值，直接删除，MIN_POSITION_WEIGHT默认值为1，即不会删除任何position
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

void CPreprocessor::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
{
	ofstream toFile("hotspots.txt");
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
}
