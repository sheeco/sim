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

	//遍历所有节点，获取当前位置，生成相应的CPosition类，添加到g_positions中
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
			cout<<endl<<"Error: CPreprocessor::BuildCandidateHotspots() 文件解析出错！"<<endl;
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
	//将执行强制类型转换，只能传入CHotspot类
	return ( ( (CHotspot *) left)->getNCoveredPosition() ) > ( ( (CHotspot *) right)->getNCoveredPosition() );
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

	//重置flag
	for(int i = 0; i < g_nPositions; i++)
		g_positions[i]->setFlag(false);
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
			if(g_positions[i]->getFlag())
				continue;
			//若水平距离已超出range，则可以直接停止搜索
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
			//若水平距离已超出range，则可以直接停止搜索
			if(fabs(g_positions[i]->getX() - result->getX()) > TRANS_RANGE)
				break;
			if(CBase::getDistance(*result, *g_positions[i]) <= TRANS_RANGE)
			{
				result->addPosition(g_positions[i]);
				g_positions[i]->setFlag(true);
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
	for(int i = 0; i < g_nPositions; i++)
		g_positions[i]->setFlag(false);

	bool modified;
	//循环，直到没有新的position被加入
	do
	{
		modified = false;
		//对新的hotspot重心，再次遍历position
		for(int i = 0; i < g_nPositions; i++)
		{
			if(g_positions[i]->getFlag())
				continue;
			if( g_positions[i]->getX() + TRANS_RANGE < hotspot->getX() )
				continue;
			//若水平距离已超出range，则可以直接停止搜索
			if( hotspot->getX() + TRANS_RANGE < g_positions[i]->getX() )
				break;
			if(CBase::getDistance(*hotspot, *g_positions[i]) <= TRANS_RANGE)
			{
				hotspot->addPosition(g_positions[i]);
				g_positions[i]->setFlag(true);
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
	g_nPositions = g_positions.size();
	g_nHotspotCandidates = g_hotspotCandidates.size();
	g_degreeForPositions = (int *)calloc(g_nPositions, sizeof(int));
	g_degreeForHotspots = (int *)calloc(g_nHotspotCandidates, sizeof(int));
	
	if(g_degreeForHotspots == NULL || g_degreeForPositions == NULL)
	{
		cout<<endl<<"Error: CPreprocessor::GenerateDegrees() 内存分配失败"<<endl;
		_PAUSE;
		return;
	}
	//计算度数
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
	g_nPositions = g_positions.size();
	g_nHotspotCandidates = g_hotspotCandidates.size();
	//重新分配内存
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
		cout<<endl<<"Error: CPreprocessor::GenerateCoverMatrix() 内存分配失败"<<endl;
		_PAUSE;
		return;
	}
	//构造cover矩阵
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

	//储存当前的矩阵大小，用于下次释放内存
	g_old_nPositions = g_nPositions;
	g_old_nHotspots = g_nHotspotCandidates;
}

//实际上没有使用这个函数
void CPreprocessor::RemoveBadHotspots()
{
	if(g_hotspotCandidates.empty())
		return;

	int n_removed;
	//FIXME:
	//删除数目由删除策略决定
	n_removed = g_nHotspotCandidates / 2;

	vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin();
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
			ihotspot = g_hotspotCandidates.erase(ihotspot);  //执行删除，重新获取迭代器
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
	if(g_hotspotCandidates.empty())
		return;

	//重置flag
	for(vector<CPosition *>::iterator ipos = g_positions.begin(); ipos != g_positions.end(); ipos++)
		(*ipos)->setFlag(false);
	for(vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); ihotspot++)
		(*ihotspot)->setFlag(false);
	//遍历所有符合条件的候选hotspot
	for(vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); ihotspot++)
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
			g_selectedHotspots.push_back(*ihotspot);  //选中
			(*ihotspot)->setFlag(true);
			for(ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
				(*ipos)->setFlag(true);
		}
	}
	//将标记的hotspot删除
	for(vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); )
	{
		if((*ihotspot)->getFlag())
		{	
			ihotspot = g_hotspotCandidates.erase(ihotspot);
		}
		else
			ihotspot++;
	}
	//将标记的position移入g_tmpPositions
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
		g_positions = merge(g_positions, g_tmpPositions);
		g_tmpPositions.clear();
	}
}

void CPreprocessor::AdjustRemoteHotspots()
{
	if(g_hotspotCandidates.empty())
		return;

	vector<CHotspot *>::iterator ihotspot;
	//遍历所有只含有一个position的候选hotspot
	for(ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); ihotspot++)
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
			//若水平距离已超出2*range，则可以直接停止搜索
			if(fabs((*ihotspot)->getX() - g_positions[i]->getX()) > 2 * TRANS_RANGE)
				break;
			if(CBase::getDistance(**ihotspot, *g_positions[i]) <=  2 * TRANS_RANGE
				&& CBase::getDistance(**ihotspot, *g_positions[i]) > max_distance)
			{
				pos_max_distance = i;
			}
		}

		//若没有符合条件的position，结束
		if(pos_max_distance == -1)
			continue;
		//将找到的最远position加入这个hotspot的覆盖列表，将hotspot中心调整到两点间的中点
		(*ihotspot)->addPosition(g_positions[pos_max_distance]);
		(*ihotspot)->recalculateCenter();
		//重置flag
		for(int i = 0; i < g_nPositions; i++)
			g_positions[i]->setFlag(false);
		g_positions[pos_max_distance]->setFlag(true);

		//循环，直到没有新的position被加入
		bool modified = false;
		do
		{
			modified = false;
			//对新的hotspot重心，再次遍历position
			for(int i = pos_max_distance - 1; i >= 0; i--)
			{
				if(g_positions[i]->getFlag())
					continue;
				//若水平距离已超出range，则可以直接停止搜索
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
				//若水平距离已超出range，则可以直接停止搜索
				if(fabs(g_positions[i]->getX() - (*ihotspot)->getX()) > TRANS_RANGE)
					break;
				if(CBase::getDistance(**ihotspot, *g_positions[i]) <= TRANS_RANGE)
				{
					(*ihotspot)->addPosition(g_positions[i]);
					g_positions[i]->setFlag(true);
					modified = true;
				}
			}

			//重新计算hotspot的重心
			if(modified)
				(*ihotspot)->recalculateCenter();

		}while(modified);
	}

	//重新排序，更新相关信息
	g_hotspotCandidates = mergeSort(g_hotspotCandidates, largerByNCoveredPositions);
	GenerateDegrees();
	GenerateCoverMatrix();
}

void CPreprocessor::BuildCandidateHotspots(int time)
{
	//释放指针vector
	if(! g_hotspotCandidates.empty())
		freePointerVector(g_hotspotCandidates);

	//merge-HAR: merge-HAR中禁止在此释放，改为在主函数中手动释放
	if(! DO_MERGE_HAR)
		g_selectedHotspots.clear();

//DBG:
	//int dbg_sumID = 0;
	//int dbg_count = 0;
	//int dbg_tmp = 0;
	//for(vector<CPosition *>::iterator ipos = g_positions.begin(); ipos != g_positions.end(); ipos++)
	//{
		//cout<<(*ipos)->getID()<<" ";
		//dbg_sumID += (*ipos)->getID();
		//dbg_count++;
		//if(dbg_tmp == (*ipos)->getID())
		//	_PAUSE;
		//dbg_tmp = (*ipos)->getID();
	//	cout<<endl<<"count: "<<dbg_count<<", sum: "<<dbg_sumID<<";";
	//}
	//cout<<endl<<"sumID = "<<dbg_sumID<<", count = "<<dbg_count<<endl;
	
	//将所有position按x坐标排序，以便简化遍历操作
	g_positions = mergeSort(g_positions);

	//dbg_sumID = 0;
	//dbg_count = 0;

	//从每个position出发生成一个候选hotspot
	for(vector<CPosition *>::iterator ipos = g_positions.begin(); ipos != g_positions.end(); ipos++)
	{
		//cout<<(*ipos)->getID()<<" ";
		//dbg_sumID += (*ipos)->getID();
		//dbg_count++;
		//cout<<endl<<"count: "<<dbg_count<<", sum: "<<dbg_sumID<<";";

		g_hotspotCandidates.push_back(GenerateHotspotFromPosition(*ipos, time));
	}
	//cout<<endl<<"sumID = "<<dbg_sumID<<", count = "<<dbg_count<<endl;

	////将所有候选hotspot按x坐标排序
	//g_hotspotCandidates = mergeSort(g_hotspotCandidates, largerByLocationX);

	//将所有候选hotspot按cover数排序，由小到大
	g_hotspotCandidates = mergeSort(g_hotspotCandidates, largerByNCoveredPositions);

	//更新相关信息
	GenerateDegrees();
	////GA: 
	//GenerateCoverMatrix();
}

void CPreprocessor::UpdateDegrees()
{
	//更新相关信息
	GenerateDegrees();
}

void CPreprocessor::SaveHotspotsToFile(int time, vector<CHotspot *> hotspots)
{
	ofstream toFile("hotspots.txt");
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
		toFile<<time<< TAB <<(*ihotspot)->getX()<< TAB <<(*ihotspot)->getY()<< TAB <<(*ihotspot)->getID()<< TAB <<(*ihotspot)->getNCoveredPosition()<<endl;
}
