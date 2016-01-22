#include "GreedySelection.h"

CGreedySelection::CGreedySelection()
{
	//制作候选hotspot集的副本
	if(! CHotspot::hotspotCandidates.empty())
	{
		for(vector<CHotspot *>::iterator ihotspot = CHotspot::hotspotCandidates.begin(); ihotspot != CHotspot::hotspotCandidates.end(); ihotspot++)
		{
			CHotspot *tmp_hotspot = new CHotspot(**ihotspot);
			copy_hotspotCandidates.push_back(tmp_hotspot);
		}
	}
	copy_hotspotCandidates = CHotspot::hotspotCandidates;
	//copy_oldSelectedHotspots = CHotspot::oldSelectedHotspots;
	uncoveredPositions = CPosition::positions;
	unselectedHotspots = CHotspot::hotspotCandidates;
}

CGreedySelection::~CGreedySelection(void)
{
	//if(! copy_hotspotCandidates.empty())
	//	CPreprocessor::freePointerVector(copy_hotspotCandidates);
}

void CGreedySelection::UpdateStatus()
{
	hotspotsAboveAverage.clear();
	//对剩余hotspot按ratio数从小到大排序（根据-balanced-ratio选项，可能是新的ratio计算或直接使用nCoveredPosition的值）
	for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ihotspot++)
		(*ihotspot)->updateStatus();
	unselectedHotspots = CPreprocessor::mergeSort(unselectedHotspots, CPreprocessor::ascendByRatio);

	if(unselectedHotspots.empty())
		return;

	double max_ratio = unselectedHotspots.at(unselectedHotspots.size() - 1)->getRatio();
	if(max_ratio == 0)
		return;

	for(int i = unselectedHotspots.size() - 1; i >= 0; i--)
	{
		double ratio = unselectedHotspots[i]->getRatio();

		//merge-HAR: ratio
		ratio *= pow( unselectedHotspots[i]->getCoByCandidateType(), unselectedHotspots[i]->getAge() );

		if( ratio / max_ratio < GAMMA)
			break;
		else
			hotspotsAboveAverage.push_back(unselectedHotspots[i]);
	}
}

void CGreedySelection::GreedySelect(int time)
{
	do
	{
		this->UpdateStatus();

		int index_best_hotspot = -1;
		double best_ratio = 0;
		//遍历所有ratio高于GAMMA水平(1/2)的hotspot，调整其中心
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
					if(CBasicEntity::getDistance(*hotspotsAboveAverage[i], **ipos) <= TRANS_RANGE)
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
			ratio *= pow( hotspotsAboveAverage[i]->getCoByCandidateType(), hotspotsAboveAverage[i]->getAge() );

			if( ratio > best_ratio)
			{
				index_best_hotspot = i;
				best_ratio = ratio;
			}
		}  //调整中心结束

		CHotspot *best_hotspot;
		if( index_best_hotspot == -1 || index_best_hotspot == unselectedHotspots.size() )
		{
			//cout<<"Error: CGreedySelection::GreedySelection() index_max_hotspot == -1"<<endl;
			
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
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ihotspot++)
		{
			if(*ihotspot == best_hotspot)
			{
				unselectedHotspots.erase(ihotspot);
				break;
			}
		}
		
		//将这个hotspot覆盖列表中的所有position从其他hotspot的列表中、从uncoveredPositions中移除
		vector<CPosition *> tmp_positions = best_hotspot->getCoveredPositions();
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ihotspot++)
			(*ihotspot)->removePositionList(tmp_positions);

	
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
	//注意：未被选中的热点必须放入CHotspot::hotspotCandidates便于之后统一释放，或手动释放
	CHotspot::hotspotCandidates = unselectedHotspots;
	CHotspot::selectedHotspots = selectedHotspots;

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

	//sort new hotspots by x coordinates
	CHotspot::hotspotCandidates = CPreprocessor::mergeSort(CHotspot::hotspotCandidates, CPreprocessor::ascendByLocationX);

	for(vector<CHotspot *>::iterator iOld = CHotspot::oldSelectedHotspots.begin(); iOld != CHotspot::oldSelectedHotspots.end(); /* iOld++*/ )
	{
		CHotspot *best_merge = NULL;

		//traversal for best merge pair
		int max_cover = -1;
		int index_max_hotspot = -1;
		int i = 0;
		for(vector<CHotspot *>::iterator iNew = CHotspot::hotspotCandidates.begin(); iNew != CHotspot::hotspotCandidates.end(); iNew++, i++)
		{
			//for (x within range)
			if( (*iNew)->getX() + 2 * TRANS_RANGE <= (*iOld)->getX() )
				continue;
			if( (*iOld)->getX() + 2 * TRANS_RANGE <= (*iNew)->getX() )
				break;
			//try merge
			if( CBasicEntity::getDistance(**iOld, **iNew) < 2 * TRANS_RANGE)
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
			}	
		}

		//pop out the best merge pair and free it
		if(index_max_hotspot != -1)
		{
			vector<CHotspot *>::iterator usedHotspotCandidate = CHotspot::hotspotCandidates.begin() + index_max_hotspot;
			delete *usedHotspotCandidate;
			CHotspot::hotspotCandidates.erase(usedHotspotCandidate);
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
			iOld = CHotspot::oldSelectedHotspots.erase(iOld);
		}
	}

	CHotspot::hotspotCandidates.insert( CHotspot::hotspotCandidates.end(), mergeResult.begin(), mergeResult.end() );

	////手动释放
	//CPreprocessor::freePointerVector(CHotspot::oldSelectedHotspots);
	CHotspot::selectedHotspots.clear();

	copy_hotspotCandidates = CHotspot::hotspotCandidates;
	uncoveredPositions = CPosition::positions;
	unselectedHotspots = CHotspot::hotspotCandidates;

}