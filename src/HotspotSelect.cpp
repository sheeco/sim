#include "HotspotSelect.h"
#include "Configuration.h"
#include "PostSelect.h"
#include "NodeRepair.h"
#include "FileHelper.h"
#include "PrintHelper.h"

vector<CPosition *> CHotspotSelect::positions;

vector<CHotspot *> CHotspotSelect::hotspotCandidates;
//vector<CHotspot *> CHotspotSelect::copy_hotspotCandidates;
vector<CHotspot *> CHotspotSelect::unselectedHotspots;
vector<CHotspot *> CHotspotSelect::hotspotsAboveAverage;
vector<CHotspot *> CHotspotSelect::selectedHotspots;
map<int, vector<CHotspot *>> CHotspotSelect::oldSelectedHotspots;

int CHotspotSelect::STARTTIME_HOTSPOT_SELECT = INVALID;
int CHotspotSelect::SLOT_POSITION_UPDATE = INVALID;
int CHotspotSelect::SLOT_HOTSPOT_UPDATE = INVALID;

int CHotspotSelect::SUM_HOTSPOT_COST = 0;
int CHotspotSelect::COUNT_HOTSPOT_COST = 0;
double CHotspotSelect::SUM_PERCENT_MERGE = 0;
int CHotspotSelect::COUNT_PERCENT_MERGE = 0;
double CHotspotSelect::SUM_PERCENT_OLD = 0;
int CHotspotSelect::COUNT_PERCENT_OLD = 0;
double CHotspotSelect::SUM_SIMILARITY_RATIO = 0;
int CHotspotSelect::COUNT_SIMILARITY_RATIO = 0;


//CHotspotSelect::~CHotspotSelect()
//{
//	//if( ! copy_hotspotCandidates.empty())
//	//	FreePointerVector(copy_hotspotCandidates);
//}

void CHotspotSelect::updateAboveAverageList()
{
	hotspotsAboveAverage.clear();
	//对剩余hotspot按ratio数从小到大排序（根据-balanced-ratio选项，可能是新的ratio计算或直接使用nCoveredPosition的值）
	for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ++ihotspot)
		(*ihotspot)->updateStatus();
	unselectedHotspots = CSortHelper::mergeSort(unselectedHotspots, CSortHelper::ascendByRatio);

	if(unselectedHotspots.empty())
		return;

	double max_ratio = unselectedHotspots.at(unselectedHotspots.size() - 1)->getRatio();
	if(max_ratio == 0)
		return;

	for(int i = unselectedHotspots.size() - 1; i >= 0; i--)
	{
		double ratio = unselectedHotspots[i]->getRatio();

		//merge-HAR: ratio
		ratio *= pow( unselectedHotspots[i]->getRatioByTypeHotspotCandidate(), unselectedHotspots[i]->getAge() );

		if( ratio / max_ratio < 0.5 )
			break;
		else
			hotspotsAboveAverage.push_back(unselectedHotspots[i]);
	}
}

void CHotspotSelect::SaveOldSelectedHotspots(int now)
{
	if( now <= SLOT_HOTSPOT_UPDATE )
		return;

	//释放上一轮选取中未被选中的废弃热点
	if( !unselectedHotspots.empty() )
		FreePointerVector(unselectedHotspots);

	/************ 注意：不论执行HAR, IHAR, merge-HAR，都缓存上一轮热点选取的结果；
				  HAR和IHAR中将用于比较前后两轮选取的热点的相似度，
				  merge-HAR中将用于热点归并。                            ********************/

	//将上一轮选中的热点集合保存到oldSelectedHotspots
	//暂时不释放旧的oldSelectedHotspots
	//if( !oldSelectedHotspots.empty() )
	//	FreePointerVector(oldSelectedHotspots);
	oldSelectedHotspots[now - SLOT_HOTSPOT_UPDATE] = selectedHotspots;
	//仅清空selectedHotspot，不释放内存
	selectedHotspots.clear();
}

void CHotspotSelect::BuildCandidateHotspots(int now)
{
	CPrintHelper::PrintDoing("CANDIDATE BUILDING");

	//将所有position按x坐标排序，以便简化遍历操作
	positions = CSortHelper::mergeSort(positions);

	//从每个position出发生成一个候选hotspot
	for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
		hotspotCandidates.push_back(CHotspot::generateHotspot((*ipos)->getLocation(), positions, now));

	////将所有候选hotspot按x坐标排序
	//hotspotCandidates = mergeSort(hotspotCandidates, ascendByLocationX);

	//将所有候选hotspot按ratio排序，由小到大
	hotspotCandidates = CSortHelper::mergeSort(hotspotCandidates, CSortHelper::ascendByRatio);

	CPrintHelper::PrintDoing(STRING(hotspotCandidates.size()) + " candidates");
	CPrintHelper::PrintDone();
}

void CHotspotSelect::GreedySelect(int now)
{
	CPrintHelper::PrintDoing("GREEDY SELECT");

	unselectedHotspots = hotspotCandidates;
	hotspotCandidates.clear();
	hotspotsAboveAverage.clear();
	selectedHotspots.clear();

	vector<CPosition*> uncoveredPositions = positions;
	do
	{
		updateAboveAverageList();

		int index_best_hotspot = -1;
		double best_ratio = 0;
		//遍历所有ratio高于GAMMA水平(1/2)的hotspot，调整其中心
		for(int i = 0; i < hotspotsAboveAverage.size(); ++i)
		{
			bool modified = false;
			vector<CPosition *> temp_positions = hotspotsAboveAverage[i]->getCoveredPositions();

			//重置flag
			for(vector<CPosition *>::iterator ipos = uncoveredPositions.begin(); ipos != uncoveredPositions.end(); ++ipos)
				(*ipos)->setFlag(false);
			for(vector<CPosition *>::iterator ipos = temp_positions.begin(); ipos != temp_positions.end(); ++ipos)
				(*ipos)->setFlag(true);

			do
			{
				modified = false;
				for(vector<CPosition *>::iterator ipos = uncoveredPositions.begin(); ipos != uncoveredPositions.end(); ++ipos)
				{
					if((*ipos)->getFlag() == true)
						continue;
					if(fabs(hotspotsAboveAverage[i]->getX() - (*ipos)->getX()) > getConfig<int>("trans", "range_trans"))
						continue;
					if(CBasicEntity::withinRange(*hotspotsAboveAverage[i], **ipos, getConfig<int>("trans", "range_trans") ))
					{
						hotspotsAboveAverage[i]->addPosition(*ipos);
						(*ipos)->setFlag(true);
						modified = true;
					}
				}
				if(modified)
					hotspotsAboveAverage[i]->updateStatus();
			}while(modified);

			double ratio = hotspotsAboveAverage[i]->getRatio();

			//merge-HAR: ratio
			ratio *= pow( hotspotsAboveAverage[i]->getRatioByTypeHotspotCandidate(), hotspotsAboveAverage[i]->getAge() );

			if( ratio > best_ratio)
			{
				index_best_hotspot = i;
				best_ratio = ratio;
			}
		}  //调整中心结束

		CHotspot *best_hotspot;
		if( index_best_hotspot == -1 || index_best_hotspot == unselectedHotspots.size() )
		{
			//throw string("CHotspotSelect::GreedySelection() : index_max_hotspot == -1");
			
			//在merge-HAR中可能出现此情况，剩余的未选中热点中有一部分由于是旧热点，系数得到累积之后达不到GAMMA指示的水平
			//此时，直接选中ratio最大的候选热点
			index_best_hotspot = unselectedHotspots.size() - 1;
			best_hotspot = unselectedHotspots[index_best_hotspot];
		}
		else
		{
			//选中当前ratio最大的hotspot
			best_hotspot = hotspotsAboveAverage[index_best_hotspot];
		}

		selectedHotspots.push_back(best_hotspot);
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ++ihotspot)
		{
			if(*ihotspot == best_hotspot)
			{
				unselectedHotspots.erase(ihotspot);
				break;
			}
		}
		
		//将这个hotspot覆盖列表中的所有position从其他hotspot的列表中、从uncoveredPositions中移除
		vector<CPosition *> temp_positions = best_hotspot->getCoveredPositions();
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ++ihotspot)
			(*ihotspot)->removePositionList(temp_positions);

	
		for(int j = 0; j < temp_positions.size(); ++j)
		{
			CPosition *temp_pos = temp_positions[j];
			for(vector<CPosition *>::iterator ipos = uncoveredPositions.begin(); ipos != uncoveredPositions.end(); ++ipos)
			{
				if(*ipos == temp_pos)
				{
					uncoveredPositions.erase(ipos);
					break;
				}
			}
		}
	}while(! uncoveredPositions.empty());

	CPrintHelper::PrintDoing(STRING(selectedHotspots.size()) + " hotspots");
	CPrintHelper::PrintDone();
}

void CHotspotSelect::MergeHotspots(int now)
{
	CPrintHelper::PrintDoing("HOTSPOT MERGE");

	vector<CHotspot *> mergeResult;
	int mergeCount = 0;
	int oldCount = 0;
	//stringstream temp;

	//sort new hotspots by x coordinates
	hotspotCandidates = CSortHelper::mergeSort(hotspotCandidates, CSortHelper::ascendByLocationX);

	vector<CHotspot*> lastSelectedHotspots = oldSelectedHotspots[now - SLOT_HOTSPOT_UPDATE];
	for(vector<CHotspot *>::iterator iOld = lastSelectedHotspots.begin(); iOld != lastSelectedHotspots.end(); /* ++iOld*/ )
	{
		CHotspot *best_merge = nullptr;

		//traversal for best merge pair
		int max_cover = -1;
		int index_max_hotspot = -1;
		int i = 0;
		for(vector<CHotspot *>::iterator iNew = hotspotCandidates.begin(); iNew != hotspotCandidates.end(); ++iNew, ++i)
		{
			//for (x within range)
			if( (*iNew)->getX() + 2 * getConfig<int>("trans", "range_trans") <= (*iOld)->getX() )
				continue;
			if( (*iOld)->getX() + 2 * getConfig<int>("trans", "range_trans") <= (*iNew)->getX() )
				break;
			//try merge
			if( CBasicEntity::withinRange(**iOld, **iNew, 2 * getConfig<int>("trans", "range_trans") ) )
			{
				//FIXE: now copied from old or new ?
				CCoordinate location( ( (*iOld)->getX() + (*iNew)->getX() ) / 2 , ( (*iOld)->getY() + (*iNew)->getY() ) / 2);
				CHotspot *merge = new CHotspot(location, now);
				//for merge statistics
				++mergeCount;
				//temp << (*iOld)->getNCoveredPosition() << "/" << (*iNew)->getNCoveredPosition() << "/" << merge->getNCoveredPosition() << "," << merge->getAge() << TAB;

				//update the best merge index
				int current_cover = merge->getNCoveredPosition();
				if( current_cover >= max_cover)
				{
					max_cover = current_cover;
					index_max_hotspot = i;
					if(best_merge != nullptr)
						delete best_merge;
					best_merge = merge;
				}
			}	
		}

		//pop out the best merge pair and free it
		if(index_max_hotspot != -1)
		{
			vector<CHotspot *>::iterator usedHotspotCandidate = hotspotCandidates.begin() + index_max_hotspot;
			delete *usedHotspotCandidate;
			hotspotCandidates.erase(usedHotspotCandidate);
			//push the merge result into mergeResult and set type to merge type
			best_merge->setCandidateType(CHotspot::_merge_hotspot);
			best_merge->setAge( (*iOld)->getAge() + 1 );
			mergeResult.push_back(best_merge);

			++iOld;
		}
		//if no legal merge found, save the old hotspot as a candidate
		else
		{
			//CHotspot *old = CSortHelper::GenerateHotspotFromCoordinates( (*iOld)->getX() ,
			//															   (*iOld)->getY() ,
			//															   (*iOld)->getTime() );

			CHotspot *old = *iOld;
			++oldCount;
			old->setCandidateType(CHotspot::_old_hotspot);
			old->setAge( old->getAge() + 1 );

			mergeResult.push_back(old);
			//erase this old hotspot from g_oldSelectedHotspot, or it will be misfreed !
			//iOld = oldSelectedHotspots.erase(iOld);
		}
	}

	hotspotCandidates.insert( hotspotCandidates.end(), mergeResult.begin(), mergeResult.end() );

	CPrintHelper::PrintDone();
}

void CHotspotSelect::CompareWithOldHotspots(int now)
{
	if( oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(oldSelectedHotspots[now - SLOT_HOTSPOT_UPDATE], selectedHotspots);
	double oldArea = oldSelectedHotspots[now - SLOT_HOTSPOT_UPDATE].size() 
		* AreaCircle( getConfig<int>("trans", "range_trans")) 
		- CHotspot::getOverlapArea(oldSelectedHotspots[now - SLOT_HOTSPOT_UPDATE]);
	double newArea = selectedHotspots.size() * AreaCircle( getConfig<int>("trans", "range_trans")) - CHotspot::getOverlapArea(selectedHotspots);

	ofstream similarity( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_similarity"), ios::app);
	if( now == STARTTIME_HOTSPOT_SELECT + SLOT_HOTSPOT_UPDATE )
	{
		similarity << endl << endl << getConfig<string>("log", "info_log") << endl ;
		similarity << getConfig<string>("log", "info_hotspot_similarity") << endl;
	}
	similarity << now << TAB << ( overlapArea / oldArea ) << TAB << ( overlapArea / newArea ) << TAB
			   << overlapArea << TAB << oldArea << TAB << newArea << endl;
	similarity.close();

	//用于计算最终选取出的热点的前后相似度的历史平均值信息
	SUM_SIMILARITY_RATIO += overlapArea / oldArea;
	++COUNT_SIMILARITY_RATIO;
}

void CHotspotSelect::Init()
{
	if(STARTTIME_HOTSPOT_SELECT == INVALID)
		STARTTIME_HOTSPOT_SELECT = getConfig<int>("hs", "starttime_hospot_select");
	if(SLOT_POSITION_UPDATE == INVALID)
		SLOT_POSITION_UPDATE = getConfig<int>("hs", "slot_position_update");
	if(SLOT_HOTSPOT_UPDATE == INVALID)
		SLOT_HOTSPOT_UPDATE = getConfig<int>("hs", "slot_hotspot_update");
}

vector<CHotspot *> CHotspotSelect::HotspotSelect(vector<int> idNodes, int now)
{
	if(!( now % SLOT_HOTSPOT_UPDATE == 0
		 && now >= STARTTIME_HOTSPOT_SELECT ))
		return vector<CHotspot*>();

	CPrintHelper::PrintHeading(now, "HOTSPOT SELECT");

	SaveOldSelectedHotspots(now);

	BuildCandidateHotspots(now);

	/**************************** 热点归并过程(merge-HAR) *****************************/
	if(getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge)
		MergeHotspots(now);  //操作 CHotspot 类内变量

							 /********************************** 贪婪选取 *************************************/
	GreedySelect(now);

	/********************************* 后续选取过程 ***********************************/
	CPostSelect::PostSelect(selectedHotspots, unselectedHotspots, idNodes);


	/***************************** 疏漏节点修复过程(IHAR) ******************************/
	if(getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved)
	{
		CNodeRepair::Repair(selectedHotspots, unselectedHotspots, idNodes, now);  //传入引用
	}

	//分配每个position到唯一一个热点，并计算最终选取出的hotspot的cover的node，以备使用
	CHotspotSelect::assignPositionsToHotspots(selectedHotspots);

	CPrintHelper::PrintAttribute("Hotspot", selectedHotspots.size());

	//比较相邻两次热点选取的相似度
	if(getConfig<bool>("hs", "test_hotspot_similarity"))
	{
		CompareWithOldHotspots(now);
	}

	return selectedHotspots;
}

void CHotspotSelect::PrintInfo(int now)
{
	if( ! ( now % SLOT_HOTSPOT_UPDATE  == 0
		    && now >= STARTTIME_HOTSPOT_SELECT ) )
		return;
	
	//热点个数
	ofstream hotspot( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot"), ios::app);
	if(now == STARTTIME_HOTSPOT_SELECT)
	{
		hotspot << endl << getConfig<string>("log", "info_log") << endl ;
		hotspot << getConfig<string>("log", "info_hotspot") << endl;
	}
	hotspot << now << TAB << selectedHotspots.size() << endl; 
	hotspot.close();

	
	//热点位置
	ofstream hotspot_details(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_details"), ios::app);
	if( now == STARTTIME_HOTSPOT_SELECT )
	{
		hotspot_details << endl << getConfig<string>("log", "info_log") << endl;
		hotspot_details << getConfig<string>("log", "info_hotspot_details") << endl;
	}
	for(vector<CHotspot *>::iterator ihotspot = selectedHotspots.begin(); ihotspot != selectedHotspots.end(); ++ihotspot)
		hotspot_details << now << TAB << (*ihotspot)->getID() << TAB << (*ihotspot)->getX() << TAB << (*ihotspot)->getY() << endl;
	hotspot_details.close();


	//节点在热点内的百分比（从热点选取开始时开始统计）
	ofstream at_hotspot( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_visit"), ios::app);
	if(now == STARTTIME_HOTSPOT_SELECT)
	{
		at_hotspot << endl << getConfig<string>("log", "info_log") << endl ; 
		at_hotspot << getConfig<string>("log", "info_visit") << endl;
	}
	at_hotspot << now << TAB << CHotspot::getPercentVisiterAtHotspot() << TAB << CHotspot::getVisiterAtHotspot() << TAB << CNode::getVisiter() << endl;
	at_hotspot.close();


	//用于计算热点个数历史平均值
	SUM_HOTSPOT_COST += selectedHotspots.size();
	++COUNT_HOTSPOT_COST;

	if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
	{
		//热点归并过程统计信息（在最终选取出的热点集合中）
		if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
		{
			int mergeCount = 0;
			int oldCount = 0;
			int newCount = 0;
			ofstream merge( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_merge"), ios::app);
			ofstream merge_details( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_merge_details"), ios::app);

			if(now == STARTTIME_HOTSPOT_SELECT)
			{
				merge << endl << getConfig<string>("log", "info_log") << endl ;
				merge << getConfig<string>("log", "info_merge") << endl;
				merge_details << endl << getConfig<string>("log", "info_log") << endl ;
				merge_details << getConfig<string>("log", "info_merge_details") << endl;
			}
			merge_details << now << TAB;

			//热点类型及年龄统计信息
			for(vector<CHotspot *>::iterator ihotspot = selectedHotspots.begin(); ihotspot != selectedHotspots.end(); ++ihotspot)
			{
				if( (*ihotspot)->getTypeHotspotCandidate() == CHotspot::_merge_hotspot )
				{
					merge_details << "M/" << (*ihotspot)->getAge() << TAB;
					++mergeCount;
				}
				else if( (*ihotspot)->getTypeHotspotCandidate() == CHotspot::_old_hotspot )
				{
					merge_details << "O/" << (*ihotspot)->getAge() << TAB;
					++oldCount;
				}
				else
				{
					merge_details << "N/" << (*ihotspot)->getAge() << TAB;
					++newCount;
				}
			}

			//三种热点所占的比例
			int total = selectedHotspots.size();
			merge << now << TAB << mergeCount << TAB << double( mergeCount ) / double( total ) << TAB << oldCount << TAB 
				<< double( oldCount ) / double( total ) << TAB << newCount << TAB << double( newCount ) / double( total ) << endl;

			//用于计算归并热点和旧热点所占比例的历史平均值信息
			SUM_PERCENT_MERGE += double( mergeCount ) / double( total );
			++COUNT_PERCENT_MERGE;
			SUM_PERCENT_OLD += double( oldCount ) / double( total );
			++COUNT_PERCENT_OLD;

			merge.close();
			merge_details.close();
		}
	}

}

void CHotspotSelect::PrintFinal(int now)
{
}
