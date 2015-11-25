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

//vector<CGASolution> CPreprocessor::merge(vector<CGASolution> &left, vector<CGASolution> &right, bool(*Comp)(CGASolution, CGASolution))
//{
//	vector<CGASolution> result;
//	vector<CGASolution>::size_type li = 0;
//	vector<CGASolution>::size_type ri = 0;
//	while(li < left.size()
//		&& ri < right.size())
//	{
//		if(! Comp(left[li], right[ri]) )
//			result.push_back(right[ri++]);
//		else
//			result.push_back(left[li++]);
//	}
//	while(li < left.size())
//		result.push_back(left[li++]);
//	while(ri < right.size())
//		result.push_back(right[ri++]);
//	return result;
//}
//
//vector<CGASolution> CPreprocessor::mergeSort(vector<CGASolution> &v, bool(*Comp)(CGASolution, CGASolution))
//{
//	if(v.size() == 0)
//		return vector<CGASolution>();
//	if(v.size() == 1)
//		return vector<CGASolution>(1, v[0]);
//
//	vector<CGASolution>::iterator mid = v.begin() + v.size() / 2;
//	vector<CGASolution> left(v.begin(), mid);
//	vector<CGASolution> right(mid, v.end());
//	left = mergeSort(left, Comp);
//	right = mergeSort(right, Comp);
//
//	return merge(left, right, Comp);
//}

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
			if(CBase::getDistance(*result, *CPosition::positions[i]) <= TRANS_RANGE)
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
			if(CBase::getDistance(*result, *CPosition::positions[i]) <= TRANS_RANGE)
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
			if(CBase::getDistance(*hotspot, *CPosition::positions[i]) <= TRANS_RANGE)
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
	CPosition::nPositions = CPosition::positions.size();
	CHotspot::nHotspotCandidates = CHotspot::hotspotCandidates.size();
	g_degreeForPositions = (int *)calloc(CPosition::nPositions, sizeof(int));
	g_degreeForHotspots = (int *)calloc(CHotspot::nHotspotCandidates, sizeof(int));
	
	if(g_degreeForHotspots == NULL || g_degreeForPositions == NULL)
	{
		cout<<endl<<"Error: CPreprocessor::GenerateDegrees() 内存分配失败"<<endl;
		_PAUSE;
		return;
	}
	//计算度数
	for(int ihotspot = 0; ihotspot < CHotspot::hotspotCandidates.size(); ihotspot++)
	{
		vector<CPosition *> tmp_positions = CHotspot::hotspotCandidates[ihotspot]->getCoveredPositions();
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
	//for(int i = 0; i < CHotspot::nHotspotCandidates; i++)
	//	cout<<g_degreeForHotspots[i]<<" ";
	//cout<<endl;
	for(int i = 0; i < CPosition::nPositions; i++)
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
	//释放动态分配的内存
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
	CPosition::nPositions = CPosition::positions.size();
	CHotspot::nHotspotCandidates = CHotspot::hotspotCandidates.size();
	//重新分配内存
	try
	{
		g_coverMatrix = new int*[CPosition::nPositions];
		for(int i = 0; i < CPosition::nPositions; i++)
		{
			g_coverMatrix[i] = new int[CHotspot::nHotspotCandidates];
			memset(g_coverMatrix[i], 0, CHotspot::nHotspotCandidates * sizeof(int));
		}
	}
	catch(exception e)
	{
		cout<<endl<<"Error: CPreprocessor::GenerateCoverMatrix() 内存分配失败"<<endl;
		_PAUSE;
		return;
	}
	//构造cover矩阵
	for(int ihotspot = 0; ihotspot < CHotspot::hotspotCandidates.size(); ihotspot++)
	{
		vector<CPosition *> tmp_positions = CHotspot::hotspotCandidates[ihotspot]->getCoveredPositions();
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

	//储存当前的矩阵大小，用于下次释放内存
	g_old_nPositions = CPosition::nPositions;
	g_old_nHotspots = CHotspot::nHotspotCandidates;
}

//实际上没有使用这个函数
void CPreprocessor::RemoveBadHotspots()
{
	if(CHotspot::hotspotCandidates.empty())
		return;

	int n_removed;
	//FIXME:
	//删除数目由删除策略决定
	n_removed = CHotspot::nHotspotCandidates / 2;

	vector<CHotspot *>::iterator ihotspot = CHotspot::hotspotCandidates.begin();
	bool removed;
	//遍历所有符合条件的候选hotspot
	for(int i = 0; i < n_removed; i++)
	{
		removed = true;
		vector<CPosition *> tmp_positions = (*ihotspot)->getCoveredPositions();
		for(vector<CPosition *>::iterator ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
		{
			int index_pos = getIndexOfPosition(*ipos);
			if(index_pos == -1)
				continue;

			if(g_degreeForPositions[index_pos] < 2)  //检查是否冗余
			{
				removed = false;
				break;
			}
		}
		//如果是冗余的hotspot，执行删除
		if(removed)
		{
			//修改g_degreeForPositions
			for(vector<CPosition *>::iterator ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
			{
				int index_pos = getIndexOfPosition(*ipos);
				if(index_pos != -1)
					g_degreeForPositions[index_pos]--;
			}
			delete *ihotspot;
			ihotspot = CHotspot::hotspotCandidates.erase(ihotspot);  //执行删除，重新获取迭代器
		}
		else
		{
			ihotspot++;
		}
	}
	//修改hotspot数，更新相关信息
	GenerateDegrees();
	GenerateCoverMatrix();
}

//FIXME:排除孤立的position以提高GA优化效率，降低复杂度
void CPreprocessor::RemoveIsolatePositions()
{
	if(CHotspot::hotspotCandidates.empty())
		return;

	//重置flag
	for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); ipos++)
		(*ipos)->setFlag(false);
	for(vector<CHotspot *>::iterator ihotspot = CHotspot::hotspotCandidates.begin(); ihotspot != CHotspot::hotspotCandidates.end(); ihotspot++)
		(*ihotspot)->setFlag(false);
	//遍历所有符合条件的候选hotspot
	for(vector<CHotspot *>::iterator ihotspot = CHotspot::hotspotCandidates.begin(); ihotspot != CHotspot::hotspotCandidates.end(); ihotspot++)
	{
		//FIXME:只处理cover数为1的hotspot
		if((*ihotspot)->getNCoveredPosition() > 1)
			break;
		if((*ihotspot)->getFlag())
			continue;

		//检验是否冗余
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
		//如果hotspot是冗余的，直接删除
		if(redundant)
			(*ihotspot)->setFlag(true);
		//否则，标记cover的position，稍后移入g_tmpPosition，并直接选中这个hotspot
		else
		{
			CHotspot::selectedHotspots.push_back(*ihotspot);  //选中
			(*ihotspot)->setFlag(true);
			for(ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
				(*ipos)->setFlag(true);
		}
	}
	//将标记的hotspot删除
	for(vector<CHotspot *>::iterator ihotspot = CHotspot::hotspotCandidates.begin(); ihotspot != CHotspot::hotspotCandidates.end(); )
	{
		if((*ihotspot)->getFlag())
		{	
			ihotspot = CHotspot::hotspotCandidates.erase(ihotspot);
		}
		else
			ihotspot++;
	}
	//将标记的position移入g_tmpPositions
	for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
	{	
		if((*ipos)->getFlag())
		{
			g_tmpPositions.push_back(*ipos);
			ipos = CPosition::positions.erase(ipos);
		}
		else
			ipos++;

	}
	//修改hotspot和position数，更新相关信息
	GenerateDegrees();
	GenerateCoverMatrix();	
}

void CPreprocessor::PutBackAllPositions()
{
	//FIXME:排除孤立的position以提高GA优化效率，降低复杂度
	//将之前暂时删除的position放回
	if(! g_tmpPositions.empty())
	{
		CPosition::positions = merge(CPosition::positions, g_tmpPositions);
		g_tmpPositions.clear();
	}
}

void CPreprocessor::AdjustRemoteHotspots()
{
	if(CHotspot::hotspotCandidates.empty())
		return;

	vector<CHotspot *>::iterator ihotspot;
	//遍历所有只含有一个position的候选hotspot
	for(ihotspot = CHotspot::hotspotCandidates.begin(); ihotspot != CHotspot::hotspotCandidates.end(); ihotspot++)
	{
		if((*ihotspot)->getNCoveredPosition() > 1)
			break;
		
		int index_pos = getIndexOfPosition( ( (*ihotspot)->getCoveredPositions() ).at(0) );
		if(index_pos == -1)
			continue;

		double max_distance = 0;
		int pos_max_distance = -1;
		
		//寻找2*range以内最远的position
		for(int i = index_pos - 1; i >= 0; i--)
		{
			//若水平距离已超出2*range，则可以直接停止搜索
			if(fabs((*ihotspot)->getX() - CPosition::positions[i]->getX()) > 2 * TRANS_RANGE)
				break;
			if(CBase::getDistance(**ihotspot, *CPosition::positions[i]) <=  2 * TRANS_RANGE
				&& CBase::getDistance(**ihotspot, *CPosition::positions[i]) > max_distance)
			{
				pos_max_distance = i;
			}
		}
		for(int i = index_pos + 1; i < CPosition::nPositions; i++)
		{
			//若水平距离已超出2*range，则可以直接停止搜索
			if(fabs((*ihotspot)->getX() - CPosition::positions[i]->getX()) > 2 * TRANS_RANGE)
				break;
			if(CBase::getDistance(**ihotspot, *CPosition::positions[i]) <=  2 * TRANS_RANGE
				&& CBase::getDistance(**ihotspot, *CPosition::positions[i]) > max_distance)
			{
				pos_max_distance = i;
			}
		}

		//若没有符合条件的position，结束
		if(pos_max_distance == -1)
			continue;
		//将找到的最远position加入这个hotspot的覆盖列表，将hotspot中心调整到两点间的中点
		(*ihotspot)->addPosition(CPosition::positions[pos_max_distance]);
		(*ihotspot)->recalculateCenter();
		//重置flag
		for(int i = 0; i < CPosition::nPositions; i++)
			CPosition::positions[i]->setFlag(false);
		CPosition::positions[pos_max_distance]->setFlag(true);

		//循环，直到没有新的position被加入
		bool modified = false;
		do
		{
			modified = false;
			//对新的hotspot重心，再次遍历position
			for(int i = pos_max_distance - 1; i >= 0; i--)
			{
				if(CPosition::positions[i]->getFlag())
					continue;
				//若水平距离已超出range，则可以直接停止搜索
				if(fabs((*ihotspot)->getX() - CPosition::positions[i]->getX()) > TRANS_RANGE)
					break;
				if(CBase::getDistance(**ihotspot, *CPosition::positions[i]) <= TRANS_RANGE)
				{
					(*ihotspot)->addPosition(CPosition::positions[i]);
					CPosition::positions[i]->setFlag(true);
					modified = true;
				}
			}
			for(int i = pos_max_distance + 1; i < CPosition::nPositions; i++)
			{
				if(CPosition::positions[i]->getFlag())
					continue;
				//若水平距离已超出range，则可以直接停止搜索
				if(fabs(CPosition::positions[i]->getX() - (*ihotspot)->getX()) > TRANS_RANGE)
					break;
				if(CBase::getDistance(**ihotspot, *CPosition::positions[i]) <= TRANS_RANGE)
				{
					(*ihotspot)->addPosition(CPosition::positions[i]);
					CPosition::positions[i]->setFlag(true);
					modified = true;
				}
			}

			//重新计算hotspot的重心
			if(modified)
				(*ihotspot)->recalculateCenter();

		}while(modified);
	}

	//重新排序，更新相关信息
	CHotspot::hotspotCandidates = mergeSort(CHotspot::hotspotCandidates, ascendByRatio);
	GenerateDegrees();
	GenerateCoverMatrix();
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

	//更新相关信息
	GenerateDegrees();
	////GA: 
	//GenerateCoverMatrix();
}

void CPreprocessor::UpdateDegrees()
{
	//更新相关信息
	GenerateDegrees();
	//GA:（由于未采用GA算法，其所需的cover矩阵暂不生成）
}

void CPreprocessor::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
{
	ofstream toFile("hotspots.txt");
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
}
