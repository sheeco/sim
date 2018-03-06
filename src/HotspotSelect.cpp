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
	////������ѡhotspot���ĸ���
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
	//��ʣ��hotspot��ratio����С�������򣨸���-balanced-ratioѡ��������µ�ratio�����ֱ��ʹ��nCoveredPosition��ֵ��
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

void CHotspotSelect::CollectNewPositions(int now)
{
	if( ! ( now % getConfig<int>("hs", "slot_position_update") == 0 ) )
		return ;
	CPosition* temp_pos = nullptr;

	//����ɸѡ���µ������ڵ�
	static vector<int> idNodes = CNode::getIdNodes();
	vector<int> deadNodes = idNodes;
	RemoveFromList(deadNodes, CNode::getIdNodes());
	idNodes = CNode::getIdNodes();
	vector<CNode*> nodes = CNode::getAllNodes();

	//�������нڵ㣬��ȡ��ǰλ�ã�������Ӧ��CPosition�࣬��ӵ�CPosition::positions��
	for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
	{
		temp_pos = new CPosition();
		CCoordinate location = (*inode)->getLocation();
		temp_pos->setLocation(location, now);
		temp_pos->setNode( (*inode)->getID() );
		temp_pos->generateID();
		CPosition::positions.push_back(temp_pos);
	}

	//ɾ�������ڵ��position��¼
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

	//IHAR: ɾ�����ڵ�position��¼
	if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved )
	{
		int threshold = now - getConfig<int>("ihs", "lifetime_position");
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

void CHotspotSelect::SaveOldSelectedHotspots(int now)
{
	if( now <= getConfig<int>("hs", "starttime_hospot_select") )
		return;

	//�ͷ���һ��ѡȡ��δ��ѡ�еķ����ȵ�
	if( !CHotspot::hotspotCandidates.empty() )
		FreePointerVector(CHotspot::hotspotCandidates);

	/************ ע�⣺����ִ��HAR, IHAR, merge-HAR����������һ���ȵ�ѡȡ�Ľ����
				  HAR�в���ʹ�õ���IHAR�н����ڱȽ�ǰ������ѡȡ���ȵ�����ƶȣ�
				  merge-HAR�н������ȵ�鲢��                            ********************/

	//����һ��ѡ�е��ȵ㼯�ϱ��浽CHotspot::oldSelectedHotspots
	//��ʱ���ͷžɵ�CHotspot::oldSelectedHotspots
	//if( !CHotspot::oldSelectedHotspots.empty() )
	//	FreePointerVector(CHotspot::oldSelectedHotspots);
	CHotspot::oldSelectedHotspots[now - getConfig<int>("hs", "slot_hotspot_update")] = CHotspot::selectedHotspots;
	//�����g_selectedHotspot�����ͷ��ڴ�
	CHotspot::selectedHotspots.clear();
}

void CHotspotSelect::BuildCandidateHotspots(int now)
{
	CPrintHelper::PrintDoing("CANDIDATE BUILDING");

	//������position��x���������Ա�򻯱�������
	CPosition::positions = CSortHelper::mergeSort(CPosition::positions);

	//��ÿ��position��������һ����ѡhotspot
	for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); ++ipos)
		CHotspot::hotspotCandidates.push_back(new CHotspot(*ipos, now));

	////�����к�ѡhotspot��x��������
	//CHotspot::hotspotCandidates = mergeSort(CHotspot::hotspotCandidates, ascendByLocationX);

	//�����к�ѡhotspot��ratio������С����
	CHotspot::hotspotCandidates = CSortHelper::mergeSort(CHotspot::hotspotCandidates, CSortHelper::ascendByRatio);

	updateHotspotCandidates();
	CPrintHelper::PrintDone();
}

void CHotspotSelect::GreedySelect(int now)
{
	CPrintHelper::PrintDoing("GREEDY SELECT");

	do
	{
		updateStatus();

		int index_best_hotspot = -1;
		double best_ratio = 0;
		//��������ratio����GAMMAˮƽ(1/2)��hotspot������������
		for(int i = 0; i < hotspotsAboveAverage.size(); ++i)
		{
			bool modified = false;
			vector<CPosition *> temp_positions = hotspotsAboveAverage[i]->getCoveredPositions();

			//����flag
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
		}  //�������Ľ���

		CHotspot *best_hotspot;
		if( index_best_hotspot == -1 || index_best_hotspot == unselectedHotspots.size() )
		{
			//throw string("CHotspotSelect::GreedySelection() : index_max_hotspot == -1");
			
			//��merge-HAR�п��ܳ��ִ������ʣ���δѡ���ȵ�����һ���������Ǿ��ȵ㣬ϵ���õ��ۻ�֮��ﲻ��GAMMAָʾ��ˮƽ
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
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ++ihotspot)
		{
			if(*ihotspot == best_hotspot)
			{
				unselectedHotspots.erase(ihotspot);
				break;
			}
		}
		
		//�����hotspot�����б��е�����position������hotspot���б��С���uncoveredPositions���Ƴ�
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

void CHotspotSelect::MergeHotspots(int now)
{
	CPrintHelper::PrintDoing("HOTSPOT MERGE");

	vector<CHotspot *> mergeResult;
	int mergeCount = 0;
	int oldCount = 0;
	//stringstream temp;

	//sort new hotspots by x coordinates
	CHotspot::hotspotCandidates = CSortHelper::mergeSort(CHotspot::hotspotCandidates, CSortHelper::ascendByLocationX);

	vector<CHotspot*> lastSelectedHotspots = CHotspot::oldSelectedHotspots[now - getConfig<int>("hs", "slot_hotspot_update")];
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

	////�ֶ��ͷ�
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
		//FIXME:������ / ƽ����
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

void CHotspotSelect::HotspotSelect(int now)
{
	if( ! ( now % getConfig<int>("hs", "slot_hotspot_update") == 0 
		&& now >= getConfig<int>("hs", "starttime_hospot_select") ) )
		return;

//	if( TEST_LEARN )
//		DecayPositionsWithoutDeliveryCount(now);

	CPrintHelper::PrintHeading(now, "HOTSPOT SELECT");

	SaveOldSelectedHotspots(now);

	BuildCandidateHotspots(now);

	/**************************** �ȵ�鲢����(merge-HAR) *****************************/
	if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
		MergeHotspots(now);  //���� CHotspot ���ڱ���

	// ���º����������ڱ���

	/********************************** ̰��ѡȡ *************************************/
	GreedySelect(now);

	/********************************* ����ѡȡ���� ***********************************/
	CPostSelect postSelector(selectedHotspots, unselectedHotspots);
	selectedHotspots = postSelector.PostSelect(now);


	/***************************** ��©�ڵ��޸�����(IHAR) ******************************/
	if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved )
	{
		CNodeRepair repair(selectedHotspots, unselectedHotspots);  //��������
		selectedHotspots = repair.RepairPoorNodes(now);
	}

	//����ÿ��position��Ψһһ���ȵ㣬����������ѡȡ����hotspot��cover��node���Ա�ʹ��
	selectedHotspots = CHotspotSelect::assignPositionsToHotspots(selectedHotspots);

	//��ѡȡ�������CHotspot
	//ע�⣺δ��ѡ�е��ȵ������� CHotspot::hotspotCandidates ����֮��ͳһ�ͷţ����ֶ��ͷ�
	CHotspot::hotspotCandidates = unselectedHotspots;
	CHotspot::selectedHotspots = selectedHotspots;

	//�������ڱ���
	selectedHotspots.clear();
	unselectedHotspots.clear();
	//FreePointerVector( copy_hotspotCandidates );
	hotspotsAboveAverage.clear();
	uncoveredPositions.clear();

	CPrintHelper::PrintAttribute("Hotspot", CHotspot::selectedHotspots.size());

	//�Ƚ����������ȵ�ѡȡ�����ƶ�
	if( getConfig<bool>("hs", "test_hotspot_similarity") )
	{
		CompareWithOldHotspots(now);
	}

}

void CHotspotSelect::CompareWithOldHotspots(int now)
{
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots[now - getConfig<int>("hs", "slot_hotspot_update")], CHotspot::selectedHotspots);
	double oldArea = CHotspot::oldSelectedHotspots[now - getConfig<int>("hs", "slot_hotspot_update")].size() * AreaCircle( getConfig<int>("trans", "range_trans")) - CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots[now - getConfig<int>("hs", "slot_hotspot_update")]);
	double newArea = CHotspot::selectedHotspots.size() * AreaCircle( getConfig<int>("trans", "range_trans")) - CHotspot::getOverlapArea(CHotspot::selectedHotspots);

	ofstream similarity( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_similarity"), ios::app);
	if( now == getConfig<int>("hs", "starttime_hospot_select") + getConfig<int>("hs", "slot_hotspot_update") )
	{
		similarity << endl << endl << getConfig<string>("log", "info_log") << endl ;
		similarity << getConfig<string>("log", "info_hotspot_similarity") << endl;
	}
	similarity << now << TAB << ( overlapArea / oldArea ) << TAB << ( overlapArea / newArea ) << TAB
			   << overlapArea << TAB << oldArea << TAB << newArea << endl;
	similarity.close();

	//���ڼ�������ѡȡ�����ȵ��ǰ�����ƶȵ���ʷƽ��ֵ��Ϣ
	SUM_SIMILARITY_RATIO += overlapArea / oldArea;
	++COUNT_SIMILARITY_RATIO;
}

void CHotspotSelect::PrintInfo(int now)
{
	if( ! ( now % getConfig<int>("hs", "slot_hotspot_update")  == 0
		    && now >= getConfig<int>("hs", "starttime_hospot_select") ) )
		return;
	
	//�ȵ����
	ofstream hotspot( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot"), ios::app);
	if(now == getConfig<int>("hs", "starttime_hospot_select"))
	{
		hotspot << endl << getConfig<string>("log", "info_log") << endl ;
		hotspot << getConfig<string>("log", "info_hotspot") << endl;
	}
	hotspot << now << TAB << CHotspot::selectedHotspots.size() << endl; 
	hotspot.close();

	
	//�ȵ�λ��
	ofstream hotspot_details(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_details"), ios::app);
	if( now == getConfig<int>("hs", "starttime_hospot_select") )
	{
		hotspot_details << endl << getConfig<string>("log", "info_log") << endl;
		hotspot_details << getConfig<string>("log", "info_hotspot_details") << endl;
	}
	for(vector<CHotspot *>::iterator ihotspot = CHotspot::selectedHotspots.begin(); ihotspot != CHotspot::selectedHotspots.end(); ++ihotspot)
		hotspot_details << now << TAB << (*ihotspot)->getID() << TAB << (*ihotspot)->getX() << TAB << (*ihotspot)->getY() << endl;
	hotspot_details.close();


	//�ڵ����ȵ��ڵİٷֱȣ����ȵ�ѡȡ��ʼʱ��ʼͳ�ƣ�
	ofstream at_hotspot( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_visit"), ios::app);
	if(now == getConfig<int>("hs", "starttime_hospot_select"))
	{
		at_hotspot << endl << getConfig<string>("log", "info_log") << endl ; 
		at_hotspot << getConfig<string>("log", "info_visit") << endl;
	}
	at_hotspot << now << TAB << CHotspot::getPercentVisiterAtHotspot() << TAB << CHotspot::getVisiterAtHotspot() << TAB << CNode::getVisiter() << endl;
	at_hotspot.close();


	//���ڼ����ȵ������ʷƽ��ֵ
	SUM_HOTSPOT_COST += CHotspot::selectedHotspots.size();
	++COUNT_HOTSPOT_COST;

	if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
	{
		//�ȵ�鲢����ͳ����Ϣ��������ѡȡ�����ȵ㼯���У�
		if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
		{
			int mergeCount = 0;
			int oldCount = 0;
			int newCount = 0;
			ofstream merge( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_merge"), ios::app);
			ofstream merge_details( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_merge_details"), ios::app);

			if(now == getConfig<int>("hs", "starttime_hospot_select"))
			{
				merge << endl << getConfig<string>("log", "info_log") << endl ;
				merge << getConfig<string>("log", "info_merge") << endl;
				merge_details << endl << getConfig<string>("log", "info_log") << endl ;
				merge_details << getConfig<string>("log", "info_merge_details") << endl;
			}
			merge_details << now << TAB;

			//�ȵ����ͼ�����ͳ����Ϣ
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

			//�����ȵ���ռ�ı���
			int total = CHotspot::selectedHotspots.size();
			merge << now << TAB << mergeCount << TAB << double( mergeCount ) / double( total ) << TAB << oldCount << TAB 
				<< double( oldCount ) / double( total ) << TAB << newCount << TAB << double( newCount ) / double( total ) << endl;

			//���ڼ���鲢�ȵ�;��ȵ���ռ��������ʷƽ��ֵ��Ϣ
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
	//����final��������䣩
	ofstream final( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_final"), ios::app);
	//final << CNode::getPercentEncounterActiveAtHotspot() << TAB ;
	//final << CData::getPercentDeliveryAtHotspot() << TAB ;
	if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
		final << getAveragePercentMerge() << TAB << getAveragePercentOld() << TAB ;
	if( getConfig<bool>("hs", "test_hotspot_similarity") )
		final << getAverageSimilarityRatio() << TAB ;
	final.close();
	
}
