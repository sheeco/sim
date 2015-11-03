#include "GreedySelection.h"


CGreedySelection::CGreedySelection()
{
	//制作候选hotspot集的副本
	if(! g_hotspotCandidates.empty())
	{
		for(vector<CHotspot *>::iterator ihotspot = g_hotspotCandidates.begin(); ihotspot != g_hotspotCandidates.end(); ihotspot++)
		{
			CHotspot *tmp_hotspot = new CHotspot(**ihotspot);
			copy_hotspotCandidates.push_back(tmp_hotspot);
		}
	}
	copy_hotspotCandidates = g_hotspotCandidates;
	//copy_oldSelectedHotspots = g_oldSelectedHotspots;
	uncoveredPositions = g_positions;
	unselectedHotspots = g_hotspotCandidates;
}

CGreedySelection::~CGreedySelection(void)
{
	//if(! copy_hotspotCandidates.empty())
	//	CPreprocessor::freePointerVector(copy_hotspotCandidates);
}

void CGreedySelection::UpdateStatus()
{
	hotspotsAboveAverage.clear();
	//对剩余hotspot按cover数排序
	unselectedHotspots = CPreprocessor::mergeSort(unselectedHotspots, CPreprocessor::largerByNCoveredPositions);

	if(unselectedHotspots.empty())
		return;

	int max_nCover = unselectedHotspots.at(unselectedHotspots.size() - 1)->getNCoveredPosition();
	if(max_nCover == 0)
		return;

	for(int i = unselectedHotspots.size() - 1; i >= 0; i--)
	{
		double cover = unselectedHotspots[i]->getNCoveredPosition();

		//merge-HAR: ratio
		cover = pow( unselectedHotspots[i]->getRatioByCandidateType(), unselectedHotspots[i]->getAge() ) * cover;

		if( cover / max_nCover < 0.5)
			break;
		else
			hotspotsAboveAverage.push_back(unselectedHotspots[i]);
	}
}

void CGreedySelection::GreedySelect(int time)
{
	int mergeCount = 0;
	int oldCount = 0;
	int newCount = 0;
	ofstream merge("merge.txt", ios::app);
	ofstream merge_statistics("merge-statistics.txt", ios::app);
	if( DO_MERGE_HAR )
	{
		if(time == startTimeForHotspotSelection)
		{
			merge << logInfo;
			merge_statistics << logInfo;
			merge << "#Time" << TAB << "#MergeHotspotCount" << TAB << "#OldHotspotCount" << TAB << "#NewHotspotCount";
			merge_statistics << "#Time" << TAB << "#HotspotType/#CoverCount" << endl;
		}
		merge_statistics << time << TAB;
	}
	do
	{
		this->UpdateStatus();

		int index_max_hotspot = -1;
		int max_cover = 0;
		//遍历所有ratio高于1/2的hotspot，调整其中心
		for(int i = 0; i < hotspotsAboveAverage.size(); i++)
		{
			bool modified = false;
			vector<CPosition *> tmp_positions = hotspotsAboveAverage[i]->getCoveredPositions();

			//重置flag
			for(vector<CPosition *>::iterator ipos = uncoveredPositions.begin(); ipos != uncoveredPositions.end(); ipos++)
				(*ipos)->setFlag(false);
			for(vector<CPosition *>::iterator ipos = tmp_positions.begin(); ipos != tmp_positions.end(); ipos++)
				(*ipos)->setFlag(true);

			do
			{
				modified = false;
				for(vector<CPosition *>::iterator ipos = uncoveredPositions.begin(); ipos != uncoveredPositions.end(); ipos++)
				{
					if((*ipos)->getFlag() == true)
						continue;
					if(fabs(hotspotsAboveAverage[i]->getX() - (*ipos)->getX()) > TRANS_RANGE)
						continue;
					if(CBase::getDistance(*hotspotsAboveAverage[i], **ipos) <= TRANS_RANGE)
					{
						hotspotsAboveAverage[i]->addPosition(*ipos);
						(*ipos)->setFlag(true);
						modified = true;
					}
				}
				if(modified)
					hotspotsAboveAverage[i]->recalculateCenter();
			}while(modified);

			double cover = hotspotsAboveAverage[i]->getNCoveredPosition();

			//merge-HAR: ratio
			cover = pow( hotspotsAboveAverage[i]->getRatioByCandidateType(), hotspotsAboveAverage[i]->getAge() ) * cover;

			if( cover >= max_cover)
			{
				index_max_hotspot = i;
				max_cover = cover;
			}
		}  //调整中心结束

		if(index_max_hotspot == -1)
		{
			cout<<"Error: CGreedySelection::GreedySelection() index_max_hotspot == -1"<<endl;
			_PAUSE;
		}

		//选中当前cover数最大的hotspot
		CHotspot *best_hotspot = hotspotsAboveAverage[index_max_hotspot];

		if(DO_MERGE_HAR)
		{
			if( best_hotspot->getCandidateType() == TYPE_MERGE_HOTSPOT )
			{
				merge_statistics << "M/" << best_hotspot->getAge() << TAB;
				mergeCount++;
			}
			else if( best_hotspot->getCandidateType() == TYPE_OLD_HOTSPOT )
			{
				merge_statistics << "O/" << best_hotspot->getAge() << TAB;
				oldCount++;
			}
			else
			{
				merge_statistics << "N/" << best_hotspot->getAge() << TAB;
				newCount++;
			}
		}
		selectedHotspots.push_back(best_hotspot);
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ihotspot++)
		{
			if(*ihotspot == best_hotspot)
			{
				unselectedHotspots.erase(ihotspot);
				break;
			}
		}
		
		//将这个hotspot覆盖列表中的所有position从其他hotspot的列表中、从uncoveredPositions中移除
		vector<CPosition *> tmp_positions = hotspotsAboveAverage[index_max_hotspot]->getCoveredPositions();
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ihotspot++)
		{
			(*ihotspot)->removePositionList(tmp_positions);
		}
	
		for(int j = 0; j < tmp_positions.size(); j++)
		{
			CPosition *tmp_pos = tmp_positions[j];
			for(vector<CPosition *>::iterator ipos = uncoveredPositions.begin(); ipos != uncoveredPositions.end(); ipos++)
			{
				if(*ipos == tmp_pos)
				{
					uncoveredPositions.erase(ipos);
					break;
				}
			}
		}
	}while(! uncoveredPositions.empty());

	//将选取结果存入全局变量
	//注意：未被选中的热点必须放入g_hotspotCandidates便于之后统一释放，或手动释放
	g_hotspotCandidates = unselectedHotspots;
	g_selectedHotspots = selectedHotspots;

	if(DO_MERGE_HAR)
	{
		merge << time << TAB << mergeCount << TAB << oldCount << TAB << newCount << endl;
		merge_statistics << endl;
	}

	merge.close();
	merge_statistics.close();
}

int CGreedySelection::getCost()
{
	return selectedHotspots.size();
}

string CGreedySelection::toString()
{
	ostringstream os;
	int i;
	for(i = 0; i < selectedHotspots.size(); i++)
		os<<selectedHotspots[i]->toString(false)<<endl;
	os<<"[ Total Cost ] "<<i<<endl;
	return os.str();
}

void CGreedySelection::mergeHotspots(int time)
{
	vector<CHotspot *> mergeResult;
	int mergeCount = 0;
	int oldCount = 0;
	stringstream tmp;
	ofstream merge_details("merge-details.txt", ios::app);
	if(time == startTimeForHotspotSelection)
	{
		merge_details << logInfo;
		merge_details << "#Time" << TAB << "#LegalMergeCount" << TAB << "#BestMergeCount" << TAB << "#NoMergeCount/#OldHotspotCount" << endl;
		merge_details << "#Time" << TAB << "#OldCover/#NewCover/#MergeCover,#MergeAge ..." << endl;
	}

	//sort new hotspots by x coordinates
	CPreprocessor::mergeSort(g_hotspotCandidates, CPreprocessor::largerByLocationX);

	for(vector<CHotspot *>::iterator iOld = g_oldSelectedHotspots.begin(); iOld != g_oldSelectedHotspots.end(); /* iOld++*/ )
	{
		CHotspot *best_merge = NULL;

		//traversal for best merge pair
		int max_cover = -1;
		int index_max_hotspot = -1;
		int i = 0;
		for(vector<CHotspot *>::iterator iNew = g_hotspotCandidates.begin(); iNew != g_hotspotCandidates.end(); iNew++, i++)
		{
			//for (x within range)
			if( (*iNew)->getX() + TRANS_RANGE < (*iOld)->getX() )
				continue;
			if( (*iOld)->getX() + TRANS_RANGE < (*iNew)->getX() )
				break;
			//try merge (new)
			if( CBase::getDistance(**iOld, **iNew) < 2 * TRANS_RANGE)
			{
				//FIXE: time copied from old or new ?
				CHotspot *merge = CPreprocessor::GenerateHotspotFromCoordinates( ( (*iOld)->getX() + (*iNew)->getX() ) / 2 ,
																				 ( (*iOld)->getY() + (*iNew)->getY() ) / 2 ,
																				   time );
				//for merge statistics
				mergeCount++;
				tmp << (*iOld)->getNCoveredPosition() << "/" << (*iNew)->getNCoveredPosition() << "/" << merge->getNCoveredPosition() << "," << merge->getAge() << TAB;

				//update the best merge index
				int current_cover = merge->getNCoveredPosition();
				if( current_cover >= max_cover)
				{
					max_cover = current_cover;
					index_max_hotspot = i;
					if(best_merge != NULL)
						delete best_merge;
					best_merge = merge;
				}
				merge_details << endl;
			}	
		}

		//pop out the best merge pair and free it
		if(index_max_hotspot != -1)
		{
			vector<CHotspot *>::iterator usedHotspotCandidate = g_hotspotCandidates.begin() + index_max_hotspot;
			delete *usedHotspotCandidate;
			g_hotspotCandidates.erase(usedHotspotCandidate);
			//push the merge result into mergeResult and set type to merge type
			best_merge->setCandidateType(TYPE_MERGE_HOTSPOT);
			best_merge->setAge( (*iOld)->getAge() + 1 );
			mergeResult.push_back(best_merge);		

			iOld++;
		}
		//if no legal merge found, save the old hotspot as a candidate
		else
		{
			//CHotspot *old = CPreprocessor::GenerateHotspotFromCoordinates( (*iOld)->getX() ,
			//															   (*iOld)->getY() ,
			//															   (*iOld)->getTime() );

			CHotspot *old = *iOld;
			oldCount++;
			old->setCandidateType(TYPE_OLD_HOTSPOT);
			old->setAge( old->getAge() + 1 );

			mergeResult.push_back(old);
			//erase this old hotspot from g_oldSelectedHotspot, or it will be misfreed !
			iOld = g_oldSelectedHotspots.erase(iOld);
		}
	}

	g_hotspotCandidates.insert( g_hotspotCandidates.end(), mergeResult.begin(), mergeResult.end() );

	////手动释放
	//CPreprocessor::freePointerVector(g_oldSelectedHotspots);
	g_selectedHotspots.clear();

	copy_hotspotCandidates = g_hotspotCandidates;
	uncoveredPositions = g_positions;
	unselectedHotspots = g_hotspotCandidates;

	merge_details << time << TAB << mergeCount << TAB << mergeResult.size() << TAB << oldCount << endl;
	merge_details << time << TAB << tmp << endl;
	merge_details.close();
}