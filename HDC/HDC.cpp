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

void CHDC::UpdateNodeLocations()
{
	//node
	for(int i = 0; i < CNode::nodes.size(); i++)
	{
		double x = 0, y = 0;
		CFileParser::getPositionFromFile(i, currentTime, x, y);
		CNode::nodes[i].setLocation(x, y, currentTime);
	}
}

void CHDC::HotspotSelection()
{
	CGreedySelection greedySelection;

	//TEST�� 
	if( TEST_LEARN )
		CPreprocessor::DecayPositionsWithoutDeliveryCount();

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

	//�Ƚ����������ȵ�ѡȡ�����ƶ�
	if(TEST_HOTSPOT_SIMILARITY)
	{
		CompareWithOldHotspots();
	}
}

void CHDC::CompareWithOldHotspots()
{
	static double sumSimilarityRatio = 0;
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots, CHotspot::selectedHotspots);
	double oldArea = CHotspot::oldSelectedHotspots.size() * AREA_SINGE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots);
	double newArea = CHotspot::selectedHotspots.size() * AREA_SINGE_HOTSPOT - CHotspot::getOverlapArea(CHotspot::selectedHotspots);

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

void CHDC::PrintInfo()
{
	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption���ڵ�buffer״̬ ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0 
		&& currentTime >= startTimeForHotspotSelection )
	{
		//�ȵ����
		ofstream hotspot("hotspot.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot << logInfo;
			hotspot << "#Time" << TAB << "#HotspotCount" << endl;
		}
		hotspot << currentTime << TAB << m_hotspots.size() << endl; 
		hotspot.close();

		//���ڼ����ȵ������ʷƽ��ֵ
		HOTSPOT_COST_SUM += m_hotspots.size();
		HOTSPOT_COST_COUNT++;

		//�ȵ�����ͳ����Ϣ
		ofstream hotspot_statistics("hotspot-statistics.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot_statistics << logInfo;
			hotspot_statistics << "#Time" << TAB << "#CoverSum" << TAB << "#HotspotCount" << TAB << "#AvgCover" << endl;
		}
		int sumCover = 0;
		for(vector<CHotspot *>::iterator it = m_hotspots.begin(); it != m_hotspots.end(); it++)
			sumCover += (*it)->getNCoveredPosition();
		hotspot_statistics << currentTime << TAB << sumCover << TAB << m_hotspots.size() << TAB << (double)sumCover / (double)m_hotspots.size() << endl;
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
			for(vector<CHotspot *>::iterator ihotspot = m_hotspots.begin(); ihotspot != m_hotspots.end(); ihotspot++)
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
			int total = m_hotspots.size();
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

		////MA�ڵ����
		//ofstream ma("ma.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	ma << logInfo;
		//	ma << "#Time" << TAB << "#MACount" << TAB << "#AvgMAWayPointCount" << endl;
		//}
		//ma << currentTime << TAB << m_routes.size() << TAB << ( (double)m_hotspots.size() / (double)m_routes.size() ) << endl;
		//ma.close();

		////���ڼ���MA�ڵ��������ʷƽ��ֵ��Ϣ
		//MA_COST_SUM += m_routes.size();
		//MA_COST_COUNT++;
		////���ڼ���MA·�㣨�ȵ㣩ƽ����������ʷƽ��ֵ��Ϣ
		//MA_WAYPOINT_SUM += (double)m_hotspots.size() / (double)m_routes.size();
		//MA_WAYPOINT_COUNT++;

		////ED��ƽ��Ͷ���ӳٵ�����ֵ
		//ofstream ed("ed.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	ed << logInfo;
		//	ed << "#Time" << TAB << "#EstimatedDelay" << endl;
		//}
		//ed << currentTime << TAB << calculateEDTime() << endl;
		//ed.close();

		////ƽ���ܺ�
		//ofstream energy_consumption("energy-consumption.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	energy_consumption << logInfo;
		//	energy_consumption << "#Time" << TAB << "#AvgEnergyConsumption" << endl;
		//}
		//energy_consumption << currentTime << TAB << ( CData::getAverageEnergyConsumption() * 100 ) << endl;
		//energy_consumption.close();

		////ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		//ofstream buffer("buffer-node-statistics.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	buffer << logInfo;
		//	buffer << "#Time" << TAB << "#AvgBufferStateInHistoryOfEachNode" << endl;
		//}
		//buffer << currentTime << TAB;
		//for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
		//	 buffer << inode->getAverageBufferSize() << TAB;
		//buffer << endl;
		//buffer.close();

		////����Ͷ����-900������debug��
		//ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	delivery_ratio << logInfo;
		//	delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		//}
		//delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		//delivery_ratio.close();
		
	}

	////����Ͷ���ʡ�����Ͷ��ʱ��
	//if(currentTime % SLOT_RECORD_INFO == 0
	//	|| currentTime == RUNTIME)
	//{
	//	//����Ͷ����-100�����ڻ������ߣ�
	//	ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
	//	if(currentTime == startTimeForHotspotSelection)
	//	{
	//		delivery_ratio << logInfo;
	//		delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
	//	}
	//	delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
	//	delivery_ratio.close();

	//	//����Ͷ���ӳ�
	//	ofstream delay("delay.txt", ios::app);
	//	if(currentTime == startTimeForHotspotSelection)
	//	{
	//		delay << logInfo;
	//		delay << "#Time" << TAB << "#AvgDelay" << endl;
	//	}
	//	delay << currentTime << TAB << CData::getAverageDelay() << endl;
	//	delay.close();

		////MA�ͽڵ����������ͳ����Ϣ
		//ofstream encounter("encounter.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	encounter << logInfo;
		//	encounter << "#Time" << TAB << "#EncounterAtHotspot" << TAB << "#Encounter" << TAB << "#EncounterPercentAtHotspot" << endl;
		//}
		//encounter << currentTime << TAB << CMANode::getEncounterAtHotspot() << TAB << CMANode::getEncounter() << TAB << CMANode::getEncounterPercentAtHotspot() << endl;
		//encounter.close();

		////log���
		//cout << "####  [ MA State ]  ";
		//for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
		//	cout << iMA->getBufferSize() << "  " ;
		//cout << endl;

		////ÿ��MA�ĵ�ǰbuffer״̬
		//ofstream ma("buffer-ma.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	ma << logInfo;
		//	ma << "#Time" << TAB << "#BufferStateOfEachMA" << endl;
		//}
		//ma << currentTime << TAB;
		//for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
		//	ma << iMA->getBufferSize() << TAB ;
		//ma << endl;
		//ma.close();

		////ÿ���ڵ�ĵ�ǰbuffer״̬
		//ofstream node("buffer-node.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	node << logInfo;
		//	node << "#Time" << TAB << "#BufferStateOfEachNode" << endl;
		//}
		//node << currentTime << TAB;
		//for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
		//	node << inode->getBufferSize() << "  " ;
		//node << endl;
		//node.close();

		////buffer�������
		//ofstream overflow("overflow.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	overflow << logInfo;
		//	overflow << "#Time" << TAB << "#OverflowCount" << endl;
		//}
		//overflow << currentTime << TAB << CData::getOverflowCount() << endl;
		//overflow.close();
	//}

	////����debug���
	//if( currentTime == RUNTIME )
	//{
	//	debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << getAverageHotspotCost() << TAB ;
	//	if(DO_MERGE_HAR)
	//		debugInfo << getAverageMergePercent() << TAB << getAverageOldPercent() << TAB ;
	//	debugInfo << getAverageMACost() << TAB ;
	//	if(TEST_HOTSPOT_SIMILARITY)
	//		debugInfo << getAverageSimilarityRatio() << TAB ;
	//	debugInfo << CData::getDeliveryAtHotspotPercent() << TAB << logInfo.replace(0, 1, "");
	//}

}