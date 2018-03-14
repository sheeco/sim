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
int CHotspotSelect::LIFETIME_POSITION = INVALID;
bool CHotspotSelect::TEST_HOTSPOT_SIMILARITY = false;

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

void CHotspotSelect::SaveOldSelectedHotspots(int now)
{
	if( now <= SLOT_HOTSPOT_UPDATE )
		return;

	//�ͷ���һ��ѡȡ��δ��ѡ�еķ����ȵ�
	if( !unselectedHotspots.empty() )
		FreePointerVector(unselectedHotspots);

	/************ ע�⣺����ִ��HAR, IHAR, merge-HAR����������һ���ȵ�ѡȡ�Ľ����
				  HAR��IHAR�н����ڱȽ�ǰ������ѡȡ���ȵ�����ƶȣ�
				  merge-HAR�н������ȵ�鲢��                            ********************/

	//����һ��ѡ�е��ȵ㼯�ϱ��浽oldSelectedHotspots
	//��ʱ���ͷžɵ�oldSelectedHotspots
	//if( !oldSelectedHotspots.empty() )
	//	FreePointerVector(oldSelectedHotspots);
	oldSelectedHotspots[now - SLOT_HOTSPOT_UPDATE] = selectedHotspots;
	//�����selectedHotspot�����ͷ��ڴ�
	selectedHotspots.clear();
}

void CHotspotSelect::BuildCandidateHotspots(int now)
{
	CPrintHelper::PrintDoing("CANDIDATE BUILDING");

	//������position��x���������Ա�򻯱�������
	positions = CSortHelper::mergeSort(positions);

	//��ÿ��position��������һ����ѡhotspot
	for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
		hotspotCandidates.push_back(CHotspot::generateHotspot((*ipos)->getLocation(), positions, now));

	////�����к�ѡhotspot��x��������
	//hotspotCandidates = mergeSort(hotspotCandidates, ascendByLocationX);

	//�����к�ѡhotspot��ratio������С����
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

	//���ڼ�������ѡȡ�����ȵ��ǰ�����ƶȵ���ʷƽ��ֵ��Ϣ
	SUM_SIMILARITY_RATIO += overlapArea / oldArea;
	++COUNT_SIMILARITY_RATIO;
}

void CHotspotSelect::Init()
{
	STARTTIME_HOTSPOT_SELECT = getConfig<int>("hs", "starttime_hospot_select");
	SLOT_POSITION_UPDATE = getConfig<int>("hs", "slot_position_update");
	SLOT_HOTSPOT_UPDATE = getConfig<int>("hs", "slot_hotspot_update");
	TEST_HOTSPOT_SIMILARITY = getConfig<int>("hs", "test_hotspot_similarity");
	LIFETIME_POSITION = getConfig<int>("ihs", "lifetime_position");
}


//��ȡ���нڵ�ĵ�ǰλ�ã�����position�б�append������ÿ������λ����Ϣ�ռ�ʱ϶�ϵ���
//ע�⣺�����ڵ��� UpdateNodeStatus() ֮�����

void CHotspotSelect::CollectNewPositions(int now, vector<CNode*> nodes)
{
	if(!( now % SLOT_POSITION_UPDATE == 0 ))
		return;

	CPosition* temp_pos = nullptr;

	//�������нڵ㣬��ȡ��ǰλ�ã�������Ӧ��CPosition�࣬��ӵ�positions��
	for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		temp_pos = new CPosition();
		CCoordinate location = ( *inode )->getLocation();
		temp_pos->setLocation(location);
		temp_pos->setTime(now);
		temp_pos->setNode(( *inode )->getID());
		temp_pos->generateID();
		positions.push_back(temp_pos);
	}

	//IHAR: ɾ�����ڵ�position��¼
	if(getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved)
	{
		int threshold = now - LIFETIME_POSITION;
		if(threshold > 0)
		{
			for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); )
			{
				if(( *ipos )->getTime() < threshold)
				{
					delete *ipos;
					ipos = positions.erase(ipos);
				}
				else
					++ipos;
			}
		}
	}
}

vector<CHotspot *> CHotspotSelect::HotspotSelect(vector<int> idNodes, int now)
{
	if(!( now % SLOT_HOTSPOT_UPDATE == 0
		 && now >= STARTTIME_HOTSPOT_SELECT ))
		return vector<CHotspot*>();

	CPrintHelper::PrintHeading(now, "HOTSPOT SELECT");

	SaveOldSelectedHotspots(now);

	BuildCandidateHotspots(now);

	/**************************** �ȵ�鲢����(merge-HAR) *****************************/
	if(getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge)
		MergeHotspots(now);  //���� CHotspot ���ڱ���

							 /********************************** ̰��ѡȡ *************************************/
	GreedySelect(now);

	/********************************* ����ѡȡ���� ***********************************/
	CPostSelect::PostSelect(selectedHotspots, unselectedHotspots, idNodes);


	/***************************** ��©�ڵ��޸�����(IHAR) ******************************/
	if(getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved)
	{
		CNodeRepair::Repair(selectedHotspots, unselectedHotspots, idNodes, now);  //��������
	}

	//����ÿ��position��Ψһһ���ȵ㣬����������ѡȡ����hotspot��cover��node���Ա�ʹ��
	CHotspotSelect::assignPositionsToHotspots(selectedHotspots);

	CPrintHelper::PrintAttribute("Hotspot", selectedHotspots.size());

	//�Ƚ����������ȵ�ѡȡ�����ƶ�
	if(TEST_HOTSPOT_SIMILARITY)
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
	
	//�ȵ����
	ofstream hotspot( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot"), ios::app);
	if(now == STARTTIME_HOTSPOT_SELECT)
	{
		hotspot << endl << getConfig<string>("log", "info_log") << endl ;
		hotspot << getConfig<string>("log", "info_hotspot") << endl;
	}
	hotspot << now << TAB << selectedHotspots.size() << endl; 
	hotspot.close();

	
	//�ȵ�λ��
	ofstream hotspot_details(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_details"), ios::app);
	if( now == STARTTIME_HOTSPOT_SELECT )
	{
		hotspot_details << endl << getConfig<string>("log", "info_log") << endl;
		hotspot_details << getConfig<string>("log", "info_hotspot_details") << endl;
	}
	for(vector<CHotspot *>::iterator ihotspot = selectedHotspots.begin(); ihotspot != selectedHotspots.end(); ++ihotspot)
		hotspot_details << now << TAB << (*ihotspot)->getID() << TAB << (*ihotspot)->getX() << TAB << (*ihotspot)->getY() << endl;
	hotspot_details.close();


	//�ڵ����ȵ��ڵİٷֱȣ����ȵ�ѡȡ��ʼʱ��ʼͳ�ƣ�
	ofstream at_hotspot( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_visit"), ios::app);
	if(now == STARTTIME_HOTSPOT_SELECT)
	{
		at_hotspot << endl << getConfig<string>("log", "info_log") << endl ; 
		at_hotspot << getConfig<string>("log", "info_visit") << endl;
	}
	at_hotspot << now << TAB << CHotspot::getPercentVisiterAtHotspot() << TAB << CHotspot::getVisiterAtHotspot() << TAB << CNode::getVisiter() << endl;
	at_hotspot.close();


	//���ڼ����ȵ������ʷƽ��ֵ
	SUM_HOTSPOT_COST += selectedHotspots.size();
	++COUNT_HOTSPOT_COST;

	if( getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
	{
		//�ȵ�鲢����ͳ����Ϣ��������ѡȡ�����ȵ㼯���У�
		if( getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_merge )
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

			//�ȵ����ͼ�����ͳ����Ϣ
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

			//�����ȵ���ռ�ı���
			int total = selectedHotspots.size();
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
}
