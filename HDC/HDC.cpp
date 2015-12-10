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

	//TEST： 
	if( TEST_LEARN )
		CPreprocessor::DecayPositionsWithoutDeliveryCount();

	/**************************** 热点归并过程(merge-HAR) *****************************/
	//merge-HAR: 
	if( DO_MERGE_HAR )
		greedySelection.mergeHotspots(currentTime);

	/********************************** 贪婪选取 *************************************/
	greedySelection.GreedySelect(currentTime);


	/********************************* 后续选取过程 ***********************************/
	CPostSelector postSelector(CHotspot::selectedHotspots);
	CHotspot::selectedHotspots = postSelector.PostSelect(currentTime);

	
	/***************************** 疏漏节点修复过程(IHAR) ******************************/
	//IHAR: POOR NODE REPAIR
	if(DO_IHAR)
	{
		CNodeRepair repair(CHotspot::selectedHotspots, CHotspot::hotspotCandidates, currentTime);
		CHotspot::selectedHotspots = repair.RepairPoorNodes();
		CHotspot::selectedHotspots = postSelector.assignPositionsToHotspots(CHotspot::selectedHotspots);
	}

	//比较相邻两次热点选取的相似度
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

	//用于计算最终选取出的热点的前后相似度的历史平均值信息
	SIMILARITY_RATIO_SUM += overlapArea / oldArea;
	SIMILARITY_RATIO_COUNT++;
}

void CHDC::PrintInfo()
{
	//hotspot选取结果、hotspot class数目、ED、Energy Consumption、节点buffer状态 ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0 
		&& currentTime >= startTimeForHotspotSelection )
	{
		//热点个数
		ofstream hotspot("hotspot.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot << logInfo;
			hotspot << "#Time" << TAB << "#HotspotCount" << endl;
		}
		hotspot << currentTime << TAB << m_hotspots.size() << endl; 
		hotspot.close();

		//用于计算热点个数历史平均值
		HOTSPOT_COST_SUM += m_hotspots.size();
		HOTSPOT_COST_COUNT++;

		//热点质量统计信息
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

		//热点归并过程统计信息（在最终选取出的热点集合中）
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

			//热点类型及年龄统计信息
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

			//三种热点所占的比例
			int total = m_hotspots.size();
			merge << currentTime << TAB << mergeCount << TAB << (double)mergeCount / (double)total << TAB << oldCount << TAB 
				  << (double)oldCount / (double)total << TAB << newCount << TAB << (double)newCount / (double)total << endl;

			//用于计算归并热点和旧热点所占比例的历史平均值信息
			MERGE_PERCENT_SUM += (double)mergeCount / (double)total;
			MERGE_PERCENT_COUNT++;
			OLD_PERCENT_SUM += (double)oldCount / (double)total;
			OLD_PERCENT_COUNT++;

			merge.close();
			merge_details.close();
		}

		////MA节点个数
		//ofstream ma("ma.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	ma << logInfo;
		//	ma << "#Time" << TAB << "#MACount" << TAB << "#AvgMAWayPointCount" << endl;
		//}
		//ma << currentTime << TAB << m_routes.size() << TAB << ( (double)m_hotspots.size() / (double)m_routes.size() ) << endl;
		//ma.close();

		////用于计算MA节点个数的历史平均值信息
		//MA_COST_SUM += m_routes.size();
		//MA_COST_COUNT++;
		////用于计算MA路点（热点）平均个数的历史平均值信息
		//MA_WAYPOINT_SUM += (double)m_hotspots.size() / (double)m_routes.size();
		//MA_WAYPOINT_COUNT++;

		////ED即平均投递延迟的理论值
		//ofstream ed("ed.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	ed << logInfo;
		//	ed << "#Time" << TAB << "#EstimatedDelay" << endl;
		//}
		//ed << currentTime << TAB << calculateEDTime() << endl;
		//ed.close();

		////平均能耗
		//ofstream energy_consumption("energy-consumption.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	energy_consumption << logInfo;
		//	energy_consumption << "#Time" << TAB << "#AvgEnergyConsumption" << endl;
		//}
		//energy_consumption << currentTime << TAB << ( CData::getAverageEnergyConsumption() * 100 ) << endl;
		//energy_consumption.close();

		////每个节点buffer状态的历史平均值
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

		////数据投递率-900（用于debug）
		//ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	delivery_ratio << logInfo;
		//	delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		//}
		//delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		//delivery_ratio.close();
		
	}

	////数据投递率、数据投递时延
	//if(currentTime % SLOT_RECORD_INFO == 0
	//	|| currentTime == RUNTIME)
	//{
	//	//数据投递率-100（用于绘制曲线）
	//	ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
	//	if(currentTime == startTimeForHotspotSelection)
	//	{
	//		delivery_ratio << logInfo;
	//		delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
	//	}
	//	delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
	//	delivery_ratio.close();

	//	//数据投递延迟
	//	ofstream delay("delay.txt", ios::app);
	//	if(currentTime == startTimeForHotspotSelection)
	//	{
	//		delay << logInfo;
	//		delay << "#Time" << TAB << "#AvgDelay" << endl;
	//	}
	//	delay << currentTime << TAB << CData::getAverageDelay() << endl;
	//	delay.close();

		////MA和节点的相遇次数统计信息
		//ofstream encounter("encounter.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	encounter << logInfo;
		//	encounter << "#Time" << TAB << "#EncounterAtHotspot" << TAB << "#Encounter" << TAB << "#EncounterPercentAtHotspot" << endl;
		//}
		//encounter << currentTime << TAB << CMANode::getEncounterAtHotspot() << TAB << CMANode::getEncounter() << TAB << CMANode::getEncounterPercentAtHotspot() << endl;
		//encounter.close();

		////log输出
		//cout << "####  [ MA State ]  ";
		//for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
		//	cout << iMA->getBufferSize() << "  " ;
		//cout << endl;

		////每个MA的当前buffer状态
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

		////每个节点的当前buffer状态
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

		////buffer溢出计数
		//ofstream overflow("overflow.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	overflow << logInfo;
		//	overflow << "#Time" << TAB << "#OverflowCount" << endl;
		//}
		//overflow << currentTime << TAB << CData::getOverflowCount() << endl;
		//overflow.close();
	//}

	////最终debug输出
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