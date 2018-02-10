#include "HotspotSelect.h"
#include "Configuration.h"
#include "PostSelect.h"
#include "NodeRepair.h"
#include "FileHelper.h"
#include "PrintHelper.h"

//vector<CHotspot *> CHotspotSelect::copy_hotspotCandidates;
vector<CPosition *> CHotspotSelect::uncoveredPositions;
vector<CHotspot *> CHotspotSelect::unselectedHotspots;
vector<CHotspot *> CHotspotSelect::hotspotsAboveAverage;
vector<CHotspot *> CHotspotSelect::selectedHotspots;

int CHotspotSelect::SUM_HOTSPOT_COST = 0;
int CHotspotSelect::COUNT_HOTSPOT_COST = 0;
double CHotspotSelect::SUM_PERCENT_MERGE = 0;
int CHotspotSelect::COUNT_PERCENT_MERGE = 0;
double CHotspotSelect::SUM_PERCENT_OLD = 0;
int CHotspotSelect::COUNT_PERCENT_OLD = 0;
double CHotspotSelect::SUM_SIMILARITY_RATIO = 0;
int CHotspotSelect::COUNT_SIMILARITY_RATIO = 0;


void CHotspotSelect::updateHotspotCandidates()
{
	////制作候选hotspot集的副本
	//if( ! CHotspot::hotspotCandidates.empty())
	//{
	//	for(vector<CHotspot *>::iterator ihotspot = CHotspot::hotspotCandidates.begin(); ihotspot != CHotspot::hotspotCandidates.end(); ++ihotspot)
	//	{
	//		CHotspot *temp_hotspot = new CHotspot(**ihotspot);
	//		copy_hotspotCandidates.push_back(temp_hotspot);
	//	}
	//}
	unselectedHotspots = CHotspot::hotspotCandidates;	
	hotspotsAboveAverage.clear();
	selectedHotspots.clear();
}

//CHotspotSelect::~CHotspotSelect()
//{
//	//if( ! copy_hotspotCandidates.empty())
//	//	FreePointerVector(copy_hotspotCandidates);
//}

void CHotspotSelect::updateStatus()
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

void CHotspotSelect::CollectNewPositions(int currentTime)
{
	if( ! ( currentTime % configs.hs.SLOT_POSITION_UPDATE == 0 ) )
		return ;
	CPosition* temp_pos = nullptr;

	//用于筛选出新的死亡节点
	static vector<int> idNodes = CNode::getIdNodes();
	vector<int> deadNodes = idNodes;
	RemoveFromList(deadNodes, CNode::getIdNodes());
	idNodes = CNode::getIdNodes();
	vector<CNode*> nodes = CNode::getNodes();

	//遍历所有节点，获取当前位置，生成相应的CPosition类，添加到CPosition::positions中
	for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
	{
		temp_pos = new CPosition();
		CCoordinate location = (*inode)->getLocation();
		temp_pos->setLocation(location, currentTime);
		temp_pos->setNode( (*inode)->getID() );
		temp_pos->generateID();
		CPosition::positions.push_back(temp_pos);
	}

	//删除死亡节点的position记录
	if( ! deadNodes.empty() )
	{
		for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
		{
			if( IfExists( deadNodes, (*ipos)->getNode() ) )
			{
				delete *ipos;
				ipos = CPosition::positions.erase(ipos);
			}
			else
				++ipos;
		}
	}

	//IHAR: 删除过期的position记录
	if( configs.HOTSPOT_SELECT == config::_improved )
	{
		int threshold = currentTime - configs.ihs.LIFETIME_POSITION;
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
					++ipos;
			}
		}
	}

	CPosition::nPositions = CPosition::positions.size();
	uncoveredPositions = CPosition::positions;
}

void CHotspotSelect::SaveOldSelectedHotspots(int currentTime)
{
	if( currentTime <= configs.hs.STARTTIME_HOSPOT_SELECT )
		return;

	//释放上一轮选取中未被选中的废弃热点
	if( !CHotspot::hotspotCandidates.empty() )
		FreePointerVector(CHotspot::hotspotCandidates);

	/************ 注意：不论执行HAR, IHAR, merge-HAR，都缓存上一轮热点选取的结果；
				  HAR中不会使用到，IHAR中将用于比较前后两轮选取的热点的相似度，
				  merge-HAR中将用于热点归并。                            ********************/

	//将上一轮选中的热点集合保存到CHotspot::oldSelectedHotspots
	//暂时不释放旧的CHotspot::oldSelectedHotspots
	//if( !CHotspot::oldSelectedHotspots.empty() )
	//	FreePointerVector(CHotspot::oldSelectedHotspots);
	CHotspot::oldSelectedHotspots[currentTime - configs.hs.SLOT_HOTSPOT_UPDATE] = CHotspot::selectedHotspots;
	//仅清空g_selectedHotspot，不释放内存
	CHotspot::selectedHotspots.clear();
}

void CHotspotSelect::BuildCandidateHotspots(int currentTime)
{
	CPrintHelper::PrintDoing("CANDIDATE BUILDING");

	//将所有position按x坐标排序，以便简化遍历操作
	CPosition::positions = CSortHelper::mergeSort(CPosition::positions);

	//从每个position出发生成一个候选hotspot
	for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); ++ipos)
		CHotspot::hotspotCandidates.push_back(new CHotspot(*ipos, currentTime));

	////将所有候选hotspot按x坐标排序
	//CHotspot::hotspotCandidates = mergeSort(CHotspot::hotspotCandidates, ascendByLocationX);

	//将所有候选hotspot按ratio排序，由小到大
	CHotspot::hotspotCandidates = CSortHelper::mergeSort(CHotspot::hotspotCandidates, CSortHelper::ascendByRatio);

	updateHotspotCandidates();
	CPrintHelper::PrintDone();
}

void CHotspotSelect::GreedySelect(int currentTime)
{
	CPrintHelper::PrintDoing("GREEDY SELECT");

	do
	{
		updateStatus();

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
					if(fabs(hotspotsAboveAverage[i]->getX() - (*ipos)->getX()) > configs.trans.RANGE_TRANS)
						continue;
					if(CBasicEntity::withinRange(*hotspotsAboveAverage[i], **ipos, configs.trans.RANGE_TRANS ))
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

	CPrintHelper::PrintDone();
}

void CHotspotSelect::MergeHotspots(int currentTime)
{
	CPrintHelper::PrintDoing("HOTSPOT MERGE");

	vector<CHotspot *> mergeResult;
	int mergeCount = 0;
	int oldCount = 0;
	//stringstream temp;

	//sort new hotspots by x coordinates
	CHotspot::hotspotCandidates = CSortHelper::mergeSort(CHotspot::hotspotCandidates, CSortHelper::ascendByLocationX);

	vector<CHotspot*> lastSelectedHotspots = CHotspot::oldSelectedHotspots[currentTime - configs.hs.SLOT_HOTSPOT_UPDATE];
	for(vector<CHotspot *>::iterator iOld = lastSelectedHotspots.begin(); iOld != lastSelectedHotspots.end(); /* ++iOld*/ )
	{
		CHotspot *best_merge = nullptr;

		//traversal for best merge pair
		int max_cover = -1;
		int index_max_hotspot = -1;
		int i = 0;
		for(vector<CHotspot *>::iterator iNew = CHotspot::hotspotCandidates.begin(); iNew != CHotspot::hotspotCandidates.end(); ++iNew, ++i)
		{
			//for (x within range)
			if( (*iNew)->getX() + 2 * configs.trans.RANGE_TRANS <= (*iOld)->getX() )
				continue;
			if( (*iOld)->getX() + 2 * configs.trans.RANGE_TRANS <= (*iNew)->getX() )
				break;
			//try merge
			if( CBasicEntity::withinRange(**iOld, **iNew, 2 * configs.trans.RANGE_TRANS ) )
			{
				//FIXE: currentTime copied from old or new ?
				CCoordinate location( ( (*iOld)->getX() + (*iNew)->getX() ) / 2 , ( (*iOld)->getY() + (*iNew)->getY() ) / 2);
				CHotspot *merge = new CHotspot(location, currentTime);
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
			vector<CHotspot *>::iterator usedHotspotCandidate = CHotspot::hotspotCandidates.begin() + index_max_hotspot;
			delete *usedHotspotCandidate;
			CHotspot::hotspotCandidates.erase(usedHotspotCandidate);
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
			//iOld = CHotspot::oldSelectedHotspots.erase(iOld);
		}
	}

	CHotspot::hotspotCandidates.insert( CHotspot::hotspotCandidates.end(), mergeResult.begin(), mergeResult.end() );

	////手动释放
	//FreePointerVector(CHotspot::oldSelectedHotspots);
	CHotspot::selectedHotspots.clear();

	//copy_hotspotCandidates = CHotspot::hotspotCandidates;
	uncoveredPositions = CPosition::positions;
	unselectedHotspots = CHotspot::hotspotCandidates;

	CPrintHelper::PrintDone();
}

vector<CHotspot *> CHotspotSelect::assignPositionsToHotspots(vector<CHotspot *> hotspots)
{
	vector<CHotspot *> temp_hotspots = hotspots;
	vector<CHotspot *> result_hotspots;
	while( !temp_hotspots.empty() )
	{
		temp_hotspots = CSortHelper::mergeSort(temp_hotspots, CSortHelper::ascendByRatio);
		//FIXME:尽量多 / 平均？
		CHotspot *selected_hotspot = temp_hotspots.at(temp_hotspots.size() - 1);
		if( selected_hotspot->getNCoveredPosition() == 0 )
			break;
		temp_hotspots.pop_back();
		result_hotspots.push_back(selected_hotspot);
		vector<CPosition *> positions = selected_hotspot->getCoveredPositions();
		for( vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin(); ihotspot != temp_hotspots.end(); ++ihotspot )
		{
			( *ihotspot )->removePositionList(positions);
			( *ihotspot )->updateStatus();
		}
	}

	return result_hotspots;
}

void CHotspotSelect::HotspotSelect(int currentTime)
{
	if( ! ( currentTime % configs.hs.SLOT_HOTSPOT_UPDATE == 0 
		&& currentTime >= configs.hs.STARTTIME_HOSPOT_SELECT ) )
		return;

//	if( TEST_LEARN )
//		DecayPositionsWithoutDeliveryCount(currentTime);

	CPrintHelper::PrintHeading(currentTime, "HOTSPOT SELECT");

	SaveOldSelectedHotspots(currentTime);

	BuildCandidateHotspots(currentTime);

	/**************************** 热点归并过程(merge-HAR) *****************************/
	if( configs.HOTSPOT_SELECT == config::_merge )
		MergeHotspots(currentTime);  //操作 CHotspot 类内变量

	// 以下函数操作类内变量

	/********************************** 贪婪选取 *************************************/
	GreedySelect(currentTime);

	/********************************* 后续选取过程 ***********************************/
	CPostSelect postSelector(selectedHotspots, unselectedHotspots);
	selectedHotspots = postSelector.PostSelect(currentTime);


	/***************************** 疏漏节点修复过程(IHAR) ******************************/
	if( configs.HOTSPOT_SELECT == config::_improved )
	{
		CNodeRepair repair(selectedHotspots, unselectedHotspots);  //传入引用
		selectedHotspots = repair.RepairPoorNodes(currentTime);
	}

	//分配每个position到唯一一个热点，并计算最终选取出的hotspot的cover的node，以备使用
	selectedHotspots = CHotspotSelect::assignPositionsToHotspots(selectedHotspots);

	//将选取结果存入CHotspot
	//注意：未被选中的热点必须放入 CHotspot::hotspotCandidates 便于之后统一释放，或手动释放
	CHotspot::hotspotCandidates = unselectedHotspots;
	CHotspot::selectedHotspots = selectedHotspots;

	//清理类内变量
	selectedHotspots.clear();
	unselectedHotspots.clear();
	//FreePointerVector( copy_hotspotCandidates );
	hotspotsAboveAverage.clear();
	uncoveredPositions.clear();

	CPrintHelper::PrintAttribute("Hotspot", CHotspot::selectedHotspots.size());

	//比较相邻两次热点选取的相似度
	if( configs.hs.TEST_HOTSPOT_SIMILARITY )
	{
		CompareWithOldHotspots(currentTime);
	}

}

void CHotspotSelect::CompareWithOldHotspots(int currentTime)
{
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots[currentTime - configs.hs.SLOT_HOTSPOT_UPDATE], CHotspot::selectedHotspots);
	double oldArea = CHotspot::oldSelectedHotspots[currentTime - configs.hs.SLOT_HOTSPOT_UPDATE].size() * AreaCircle( configs.trans.RANGE_TRANS) - CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots[currentTime - configs.hs.SLOT_HOTSPOT_UPDATE]);
	double newArea = CHotspot::selectedHotspots.size() * AreaCircle( configs.trans.RANGE_TRANS) - CHotspot::getOverlapArea(CHotspot::selectedHotspots);

	ofstream similarity( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_HOTSPOT_SIMILARITY, ios::app);
	if( currentTime == configs.hs.STARTTIME_HOSPOT_SELECT + configs.hs.SLOT_HOTSPOT_UPDATE )
	{
		similarity << endl << endl << configs.log.INFO_LOG << endl ;
		similarity << configs.log.INFO_HOTSPOT_SIMILARITY;
	}
	similarity << currentTime << TAB << ( overlapArea / oldArea ) << TAB << ( overlapArea / newArea ) << TAB
			   << overlapArea << TAB << oldArea << TAB << newArea << endl;
	similarity.close();

	//用于计算最终选取出的热点的前后相似度的历史平均值信息
	SUM_SIMILARITY_RATIO += overlapArea / oldArea;
	++COUNT_SIMILARITY_RATIO;
}

void CHotspotSelect::PrintInfo(int currentTime)
{
	if( ! ( currentTime % configs.hs.SLOT_HOTSPOT_UPDATE  == 0
		    && currentTime >= configs.hs.STARTTIME_HOSPOT_SELECT ) )
		return;
	
	//热点个数
	ofstream hotspot( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_HOTSPOT, ios::app);
	if(currentTime == configs.hs.STARTTIME_HOSPOT_SELECT)
	{
		hotspot << endl << configs.log.INFO_LOG << endl ;
		hotspot << configs.log.INFO_HOTSPOT ;
	}
	hotspot << currentTime << TAB << CHotspot::selectedHotspots.size() << endl; 
	hotspot.close();

	
	//热点位置
	ofstream hotspot_details(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_HOTSPOT_DETAILS, ios::app);
	if( currentTime == configs.hs.STARTTIME_HOSPOT_SELECT )
	{
		hotspot_details << endl << configs.log.INFO_LOG << endl;
		hotspot_details << configs.log.INFO_HOTSPOT_DETAILS;
	}
	for(vector<CHotspot *>::iterator ihotspot = CHotspot::selectedHotspots.begin(); ihotspot != CHotspot::selectedHotspots.end(); ++ihotspot)
		hotspot_details << currentTime << TAB << (*ihotspot)->getID() << TAB << (*ihotspot)->getX() << TAB << (*ihotspot)->getY() << endl;
	hotspot_details.close();


	//节点在热点内的百分比（从热点选取开始时开始统计）
	ofstream at_hotspot( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_VISIT, ios::app);
	if(currentTime == configs.hs.STARTTIME_HOSPOT_SELECT)
	{
		at_hotspot << endl << configs.log.INFO_LOG << endl ; 
		at_hotspot << configs.log.INFO_VISIT ;
	}
	at_hotspot << currentTime << TAB << CNode::getPercentVisiterAtHotspot() << TAB << CNode::getVisiterAtHotspot() << TAB << CNode::getVisiter() << endl;
	at_hotspot.close();


	//用于计算热点个数历史平均值
	SUM_HOTSPOT_COST += CHotspot::selectedHotspots.size();
	++COUNT_HOTSPOT_COST;

	if( configs.HOTSPOT_SELECT == config::_merge )
	{
		//热点归并过程统计信息（在最终选取出的热点集合中）
		if( configs.HOTSPOT_SELECT == config::_merge )
		{
			int mergeCount = 0;
			int oldCount = 0;
			int newCount = 0;
			ofstream merge( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_MERGE, ios::app);
			ofstream merge_details( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_MERGE_DETAILS, ios::app);

			if(currentTime == configs.hs.STARTTIME_HOSPOT_SELECT)
			{
				merge << endl << configs.log.INFO_LOG << endl ;
				merge << configs.log.INFO_MERGE ;
				merge_details << endl << configs.log.INFO_LOG << endl ;
				merge_details << configs.log.INFO_MERGE_DETAILS ;
			}
			merge_details << currentTime << TAB;

			//热点类型及年龄统计信息
			for(vector<CHotspot *>::iterator ihotspot = CHotspot::selectedHotspots.begin(); ihotspot != CHotspot::selectedHotspots.end(); ++ihotspot)
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
			int total = CHotspot::selectedHotspots.size();
			merge << currentTime << TAB << mergeCount << TAB << double( mergeCount ) / double( total ) << TAB << oldCount << TAB 
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

void CHotspotSelect::PrintFinal(int currentTime)
{
	//最终final输出（补充）
	ofstream final( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_FINAL, ios::app);
	//final << CNode::getPercentEncounterActiveAtHotspot() << TAB ;
	//final << CData::getPercentDeliveryAtHotspot() << TAB ;
	if( configs.HOTSPOT_SELECT == config::_merge )
		final << getAveragePercentMerge() << TAB << getAveragePercentOld() << TAB ;
	if( configs.hs.TEST_HOTSPOT_SIMILARITY )
		final << getAverageSimilarityRatio() << TAB ;
	final.close();
	
}
