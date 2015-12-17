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

	//遍历所有节点，获取当前位置，生成相应的CPosition类，添加到CPosition::positions中
	for(vector<int>::iterator i = CNode::getIdNodes().begin(); i != CNode::getIdNodes().end(); i++)
	//vector<int> idNodes = CNode::getIdNodes();
	//for(vector<int>::iterator i = idNodes.begin(); i != idNodes.end(); i++)
	{
		tmp_pos = new CPosition();
		double x = 0, y = 0;
		CFileParser::getPositionFromFile(*i, time, x, y);
		tmp_pos->moveTo(x, y, time);
		tmp_pos->setNode( *i );
		tmp_pos->generateID();
		if(tmp_pos->getID() == -1)
		{
			cout<<endl<<"Error: CPreprocessor::BuildCandidateHotspots() 文件解析出错！"<<endl;
			_PAUSE;
			break;
		}
		else
		{
			CPosition::positions.push_back(tmp_pos);
		}
	}
	
	//IHAR: 删除过期的position记录
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

	//重置flag
	for(int i = 0; i < CPosition::nPositions; i++)
		CPosition::positions[i]->setFlag(false);
	pos->setFlag(true);

	int index_pos = getIndexOfPosition(pos);
	bool modified;
	//循环，直到没有新的position被加入
	do
	{
		modified = false;
		//对新的hotspot重心，再次遍历position
		for(int i = index_pos - 1; i >= 0; i--)
		{
			if(CPosition::positions[i]->getFlag())
				continue;
			//若水平距离已超出range，则可以直接停止搜索
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
			//若水平距离已超出range，则可以直接停止搜索
			if(fabs(CPosition::positions[i]->getX() - result->getX()) > TRANS_RANGE)
				break;
			if(CBasicEntity::getDistance(*result, *CPosition::positions[i]) <= TRANS_RANGE)
			{
				result->addPosition(CPosition::positions[i]);
				CPosition::positions[i]->setFlag(true);
				modified = true;
			}
		}

		//重新计算hotspot的重心
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

	//重置flag
	for(int i = 0; i < CPosition::nPositions; i++)
		CPosition::positions[i]->setFlag(false);

	bool modified;
	//循环，直到没有新的position被加入
	do
	{
		modified = false;
		//对新的hotspot重心，再次遍历position
		for(int i = 0; i < CPosition::nPositions; i++)
		{
			if(CPosition::positions[i]->getFlag())
				continue;
			if( CPosition::positions[i]->getX() + TRANS_RANGE < hotspot->getX() )
				continue;
			//若水平距离已超出range，则可以直接停止搜索
			if( hotspot->getX() + TRANS_RANGE < CPosition::positions[i]->getX() )
				break;
			if(CBasicEntity::getDistance(*hotspot, *CPosition::positions[i]) <= TRANS_RANGE)
			{
				hotspot->addPosition(CPosition::positions[i]);
				CPosition::positions[i]->setFlag(true);
				modified = true;
			}
		}

		//重新计算hotspot的重心
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

void CPreprocessor::BuildCandidateHotspots(int time)
{
	//释放上一轮选取中未被选中的废弃热点
	if(! CHotspot::hotspotCandidates.empty())
		freePointerVector(CHotspot::hotspotCandidates);

	/************ 注意：不论执行HAR, IHAR, merge-HAR，都缓存上一轮热点选取的结果；
	              HAR中不会使用到，IHAR中将用于比较前后两轮选取的热点的相似度，
			      merge-HAR中将用于热点归并。                            ********************/

	//将上一轮选中的热点集合保存到CHotspot::oldSelectedHotspots
	//并释放旧的CHotspot::oldSelectedHotspots
	if(! CHotspot::oldSelectedHotspots.empty())
		freePointerVector(CHotspot::oldSelectedHotspots);
	CHotspot::oldSelectedHotspots = CHotspot::selectedHotspots;
	//仅清空g_selectedHotspot，不释放内存
	CHotspot::selectedHotspots.erase(CHotspot::selectedHotspots.begin(), CHotspot::selectedHotspots.end());


	//将所有position按x坐标排序，以便简化遍历操作
	CPosition::positions = mergeSort(CPosition::positions);

	//从每个position出发生成一个候选hotspot
	for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); ipos++)
		CHotspot::hotspotCandidates.push_back(GenerateHotspotFromPosition(*ipos, time));

	////将所有候选hotspot按x坐标排序
	//CHotspot::hotspotCandidates = mergeSort(CHotspot::hotspotCandidates, ascendByLocationX);

	//将所有候选hotspot按ratio排序，由小到大
	CHotspot::hotspotCandidates = mergeSort(CHotspot::hotspotCandidates, ascendByRatio);

}

void CPreprocessor::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
{
	ofstream toFile("hotspots.txt");
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
}
