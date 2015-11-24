#include "GreedySelection.h"

CGreedySelection::CGreedySelection()
{
	//������ѡhotspot���ĸ���
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
	//��ʣ��hotspot��ratio����С�������򣨸���-balanced-ratioѡ��������µ�ratio�����ֱ��ʹ��nCoveredPosition��ֵ��
	for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ihotspot++)
		(*ihotspot)->updateStatus();
	unselectedHotspots = CPreprocessor::mergeSort(unselectedHotspots, CPreprocessor::largerByRatio);

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

		if( ratio / max_ratio < GAMA)
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
		//��������ratio����GAMAˮƽ(1/2)��hotspot������������
		for(int i = 0; i < hotspotsAboveAverage.size(); i++)
		{
			bool modified = false;
			vector<CPosition *> tmp_positions = hotspotsAboveAverage[i]->getCoveredPositions();

			//����flag
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
		}  //�������Ľ���

		CHotspot *best_hotspot;
		if( index_best_hotspot == -1 || index_best_hotspot == unselectedHotspots.size() )
		{
			//cout<<"Error: CGreedySelection::GreedySelection() index_max_hotspot == -1"<<endl;
			
			//��merge-HAR�п��ܳ��ִ������ʣ���δѡ���ȵ�����һ���������Ǿ��ȵ㣬ϵ���õ��ۻ�֮��ﲻ��GAMAָʾ��ˮƽ
			//��ʱ��ֱ��ѡ��ratio���ĺ�ѡ�ȵ�
			index_best_hotspot = unselectedHotspots.size() - 1;
			best_hotspot = unselectedHotspots[index_best_hotspot];
		}
		else
		{
			//ѡ�е�ǰratio����hotspot
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
		
		//�����hotspot�����б��е�����position������hotspot���б��С���uncoveredPositions���Ƴ�
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

	//��ѡȡ�������ȫ�ֱ���
	//ע�⣺δ��ѡ�е��ȵ�������CHotspot::hotspotCandidates����֮��ͳһ�ͷţ����ֶ��ͷ�
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
	CPreprocessor::mergeSort(CHotspot::hotspotCandidates, CPreprocessor::largerByLocationX);

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

	////�ֶ��ͷ�
	//CPreprocessor::freePointerVector(CHotspot::oldSelectedHotspots);
	CHotspot::selectedHotspots.clear();

	copy_hotspotCandidates = CHotspot::hotspotCandidates;
	uncoveredPositions = CPosition::positions;
	unselectedHotspots = CHotspot::hotspotCandidates;

}