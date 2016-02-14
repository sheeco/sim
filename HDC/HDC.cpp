#include "HDC.h"
#include "PostSelector.h"
#include "NodeRepair.h"

int CHDC::HOTSPOT_COST_SUM = 0;
int CHDC::HOTSPOT_COST_COUNT = 0;
double CHDC::MERGE_PERCENT_SUM = 0;
int CHDC::MERGE_PERCENT_COUNT = 0;
double CHDC::OLD_PERCENT_SUM = 0;
int CHDC::OLD_PERCENT_COUNT = 0;
double CHDC::SIMILARITY_RATIO_SUM = 0;
int CHDC::SIMILARITY_RATIO_COUNT = 0;

extern HotspotSelect HOTSPOT_SELECT;

CHDC::CHDC(void)
{
}

void CHDC::CompareWithOldHotspots(int currentTime)
{
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots, CHotspot::selectedHotspots);
	double oldArea = CHotspot::oldSelectedHotspots.size() * AREA_SINGLE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots);
	double newArea = CHotspot::selectedHotspots.size() * AREA_SINGLE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::selectedHotspots);

	ofstream similarity("similarity.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection + SLOT_HOTSPOT_UPDATE )
	{
		similarity << INFO_LOG;
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
	if( ! currentTime % SLOT_RECORD_INFO == 0 )
		return ;

	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption���ڵ�buffer״̬ ...
	if( ( currentTime % SLOT_HOTSPOT_UPDATE  == 0 
		  && currentTime >= startTimeForHotspotSelection )
		  || currentTime == RUNTIME )
	{
		//�ȵ����
		ofstream hotspot("hotspot.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot << INFO_LOG;
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
			hotspot_statistics << INFO_LOG;
			hotspot_statistics << "#Time" << TAB << "#CoverSum" << TAB << "#HotspotCount" << TAB << "#AvgCover" << endl;
		}
		int sumCover = 0;
		for(vector<CHotspot *>::iterator it = CHotspot::selectedHotspots.begin(); it != CHotspot::selectedHotspots.end(); ++it)
			sumCover += (*it)->getNCoveredPosition();
		hotspot_statistics << currentTime << TAB << sumCover << TAB << CHotspot::selectedHotspots.size() << TAB << static_cast<double>( sumCover ) / static_cast<double>( CHotspot::selectedHotspots.size() ) << endl;
		hotspot_statistics.close();

		//�ȵ�鲢����ͳ����Ϣ��������ѡȡ�����ȵ㼯���У�
		if( HOTSPOT_SELECT == _merge )
		{
			int mergeCount = 0;
			int oldCount = 0;
			int newCount = 0;
			ofstream merge("merge.txt", ios::app);
			ofstream merge_details("merge-details.txt", ios::app);

			if(currentTime == startTimeForHotspotSelection)
			{
				merge << INFO_LOG;
				merge << "#Time" << TAB << "#MergeHotspotCount" << TAB << "#MergeHotspotPercent" << TAB << "#OldHotspotCount" << TAB 
					  << "#OldHotspotPercent" << TAB << "#NewHotspotCount" << TAB << "#NewHotspotPercent" << endl;
				merge_details << INFO_LOG;
				merge_details << "#Time" << TAB << "#HotspotType/#MergeAge ..." << endl;
			}
			merge_details << currentTime << TAB;

			//�ȵ����ͼ�����ͳ����Ϣ
			for(vector<CHotspot *>::iterator ihotspot = CHotspot::selectedHotspots.begin(); ihotspot != CHotspot::selectedHotspots.end(); ++ihotspot)
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
			merge << currentTime << TAB << mergeCount << TAB << static_cast<double>( mergeCount ) / static_cast<double>( total ) << TAB << oldCount << TAB 
				  << static_cast<double>( oldCount ) / static_cast<double>( total ) << TAB << newCount << TAB << static_cast<double>( newCount ) / static_cast<double>( total ) << endl;

			//���ڼ���鲢�ȵ�;��ȵ���ռ��������ʷƽ��ֵ��Ϣ
			MERGE_PERCENT_SUM += static_cast<double>( mergeCount ) / static_cast<double>( total );
			MERGE_PERCENT_COUNT++;
			OLD_PERCENT_SUM += static_cast<double>( oldCount ) / static_cast<double>( total );
			OLD_PERCENT_COUNT++;

			merge.close();
			merge_details.close();
		}
	}

}

void CHDC::UpdateDutyCycleForNodes(int currentTime)
{
	if( ! currentTime % SLOT_MOBILITYMODEL == 0 )
		return;

	vector<CHotspot *> hotspots = CHotspot::selectedHotspots;
	if( hotspots.empty() )
		return;

	vector<CNode *> nodes = CNode::getNodes();
	nodes = CPreprocessor::mergeSort( nodes );
	hotspots = CPreprocessor::mergeSort( hotspots, CPreprocessor::ascendByLocationX );
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		CHotspot *atHotspot = nullptr;

		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
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
		if( (*inode)->useHotspotDutyCycle() && atHotspot == nullptr )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " leaves Hotspot " << (*inode)->getAtHotspot()->getID() << " )     " ;			
			(*inode)->setAtHotspot(nullptr);
			(*inode)->resetDutyCycle();
		}
		else if( ! (*inode)->useHotspotDutyCycle() && atHotspot != nullptr )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " enters Hotspot " << atHotspot->getID() << " )     " ;
			(*inode)->setAtHotspot(atHotspot);
			(*inode)->raiseDutyCycle();
		}
	}
}