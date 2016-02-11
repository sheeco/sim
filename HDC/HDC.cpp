#include "HDC.h"

int CHDC::HOTSPOT_COST_SUM = 0;
int CHDC::HOTSPOT_COST_COUNT = 0;
double CHDC::MERGE_PERCENT_SUM = 0;
int CHDC::MERGE_PERCENT_COUNT = 0;
double CHDC::OLD_PERCENT_SUM = 0;
int CHDC::OLD_PERCENT_COUNT = 0;
double CHDC::SIMILARITY_RATIO_SUM = 0;
int CHDC::SIMILARITY_RATIO_COUNT = 0;

CHDC::CHDC(void)
{
}


CHDC::~CHDC(void)
{
}

//void CHDC::UpdateNodeLocations()
//{
//	//node
//	for(int i = 0; i < CNode::nodes.size(); i++)
//	{
//		double x = 0, y = 0;
//		CFileParser::getPositionFromFile(i, currentTime, x, y);
//		CNode::nodes[i].moveTo(x, y, currentTime);
//	}
//}

void CHDC::HotspotSelection(int currentTime)
{
	/**************************** �ȵ�鲢����(merge-HAR) *****************************/
	CPreprocessor::BuildCandidateHotspots(currentTime);

	CGreedySelection greedySelection;

	/**************************** �ȵ�鲢����(merge-HAR) *****************************/
	//merge-HAR: 
	if( DO_MERGE_HAR )
		greedySelection.mergeHotspots(currentTime);

	/********************************** ̰��ѡȡ *************************************/
	greedySelection.GreedySelect(currentTime);


	/********************************* ����ѡȡ���� ***********************************/
	CPostSelector postSelector(CHotspot::selectedHotspots);
	CHotspot::selectedHotspots = postSelector.PostSelect(currentTime);

	
	/***************************** ��©�ڵ��޸�����(IHAR) ******************************/
	//IHAR: POOR NODE REPAIR
	if(DO_IHAR)
	{
		CNodeRepair repair(CHotspot::selectedHotspots, CHotspot::hotspotCandidates, currentTime);
		CHotspot::selectedHotspots = repair.RepairPoorNodes();
		CHotspot::selectedHotspots = postSelector.assignPositionsToHotspots(CHotspot::selectedHotspots);
	}

	cout << CR ;
	cout << "####  [ Hotspots ]  " << CHotspot::selectedHotspots.size() ;

	//�Ƚ����������ȵ�ѡȡ�����ƶ�
	if(TEST_HOTSPOT_SIMILARITY)
	{
		CompareWithOldHotspots(currentTime);
	}
}

void CHDC::CompareWithOldHotspots(int currentTime)
{
	static double sumSimilarityRatio = 0;
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots, CHotspot::selectedHotspots);
	double oldArea = CHotspot::oldSelectedHotspots.size() * AREA_SINGLE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots);
	double newArea = CHotspot::selectedHotspots.size() * AREA_SINGLE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::selectedHotspots);

	ofstream similarity("similarity.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection + SLOT_HOTSPOT_UPDATE )
	{
		similarity << logInfo;
		similarity << "#Time" << TAB << "#Overlap/Old" << TAB << "#Overlap/New" << TAB
				   << "#OverlapArea" << TAB << "#OldArea" << TAB << "#NewArea" << endl;
	}
	similarity << currentTime << TAB << ( overlapArea / oldArea ) << TAB << ( overlapArea / newArea ) << TAB
			   << overlapArea << TAB << oldArea << TAB << newArea << endl;
	similarity.close();

	//���ڼ�������ѡȡ�����ȵ��ǰ�����ƶȵ���ʷƽ��ֵ��Ϣ
	SIMILARITY_RATIO_SUM += overlapArea / oldArea;
	SIMILARITY_RATIO_COUNT++;
}

void CHDC::PrintInfo(int currentTime)
{
	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption���ڵ�buffer״̬ ...
	if( ( currentTime % SLOT_HOTSPOT_UPDATE  == 0 
		&& currentTime >= startTimeForHotspotSelection )
		|| currentTime == RUNTIME )
	{
		//�ȵ����
		ofstream hotspot("hotspot.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot << logInfo;
			hotspot << "#Time" << TAB << "#HotspotCount" << endl;
		}
		hotspot << currentTime << TAB << CHotspot::selectedHotspots.size() << endl; 
		hotspot.close();

		//���ڼ����ȵ������ʷƽ��ֵ
		HOTSPOT_COST_SUM += CHotspot::selectedHotspots.size();
		HOTSPOT_COST_COUNT++;

		//�ȵ�����ͳ����Ϣ
		ofstream hotspot_statistics("hotspot-statistics.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot_statistics << logInfo;
			hotspot_statistics << "#Time" << TAB << "#CoverSum" << TAB << "#HotspotCount" << TAB << "#AvgCover" << endl;
		}
		int sumCover = 0;
		for(vector<CHotspot *>::iterator it = CHotspot::selectedHotspots.begin(); it != CHotspot::selectedHotspots.end(); it++)
			sumCover += (*it)->getNCoveredPosition();
		hotspot_statistics << currentTime << TAB << sumCover << TAB << CHotspot::selectedHotspots.size() << TAB << (double)sumCover / (double)CHotspot::selectedHotspots.size() << endl;
		hotspot_statistics.close();

		//�ȵ�鲢����ͳ����Ϣ��������ѡȡ�����ȵ㼯���У�
		if( DO_MERGE_HAR )
		{
			int mergeCount = 0;
			int oldCount = 0;
			int newCount = 0;
			ofstream merge("merge.txt", ios::app);
			ofstream merge_details("merge-details.txt", ios::app);

			if(currentTime == startTimeForHotspotSelection)
			{
				merge << logInfo;
				merge << "#Time" << TAB << "#MergeHotspotCount" << TAB << "#MergeHotspotPercent" << TAB << "#OldHotspotCount" << TAB 
					  << "#OldHotspotPercent" << TAB << "#NewHotspotCount" << TAB << "#NewHotspotPercent" << endl;
				merge_details << logInfo;
				merge_details << "#Time" << TAB << "#HotspotType/#MergeAge ..." << endl;
			}
			merge_details << currentTime << TAB;

			//�ȵ����ͼ�����ͳ����Ϣ
			for(vector<CHotspot *>::iterator ihotspot = CHotspot::selectedHotspots.begin(); ihotspot != CHotspot::selectedHotspots.end(); ihotspot++)
			{
				if( (*ihotspot)->getCandidateType() == TYPE_MERGE_HOTSPOT )
				{
					merge_details << "M/" << (*ihotspot)->getAge() << TAB;
					mergeCount++;
				}
				else if( (*ihotspot)->getCandidateType() == TYPE_OLD_HOTSPOT )
				{
					merge_details << "O/" << (*ihotspot)->getAge() << TAB;
					oldCount++;
				}
				else
				{
					merge_details << "N/" << (*ihotspot)->getAge() << TAB;
					newCount++;
				}
			}

			//�����ȵ���ռ�ı���
			int total = CHotspot::selectedHotspots.size();
			merge << currentTime << TAB << mergeCount << TAB << (double)mergeCount / (double)total << TAB << oldCount << TAB 
				  << (double)oldCount / (double)total << TAB << newCount << TAB << (double)newCount / (double)total << endl;

			//���ڼ���鲢�ȵ�;��ȵ���ռ��������ʷƽ��ֵ��Ϣ
			MERGE_PERCENT_SUM += (double)mergeCount / (double)total;
			MERGE_PERCENT_COUNT++;
			OLD_PERCENT_SUM += (double)oldCount / (double)total;
			OLD_PERCENT_COUNT++;

			merge.close();
			merge_details.close();
		}
	}

}

void CHDC::UpdateDutyCycleForNodes(int currentTime)
{
	vector<CHotspot *> hotspots = CHotspot::selectedHotspots;
	if( hotspots.empty() )
		return;

	vector<CNode *> nodes = CNode::getNodes();
	nodes = CPreprocessor::mergeSort( nodes );
	hotspots = CPreprocessor::mergeSort( hotspots, CPreprocessor::ascendByLocationX );
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	{
		CHotspot *atHotspot = NULL;

		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
		{
			if( (*ihotspot)->getX() + TRANS_RANGE < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + TRANS_RANGE < (*ihotspot)->getX() )
				break;
			if( CBasicEntity::getDistance( **inode, **ihotspot ) <= TRANS_RANGE )		
			{
				atHotspot = *ihotspot;
				break;
			}
		}

		//update duty cycle
		if( (*inode)->useHotspotDutyCycle() && atHotspot == NULL )
		{
			cout << CR ;
			cout << "####  ( Node " << (*inode)->getID() << " leaves Hotspot " << (*inode)->getAtHotspot()->getID() << " )     " ;			
			(*inode)->setAtHotspot(NULL);
			(*inode)->resetDutyCycle();
		}
		else if( ! (*inode)->useHotspotDutyCycle() && atHotspot != NULL )
		{
			cout << CR ;
			cout << "####  ( Node " << (*inode)->getID() << " enters Hotspot " << atHotspot->getID() << " )     " ;
			(*inode)->setAtHotspot(atHotspot);
			(*inode)->raiseDutyCycle();
		}
	}
}