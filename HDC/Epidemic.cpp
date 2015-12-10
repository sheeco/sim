#include "Epidemic.h"


Epidemic::Epidemic(void)
{
}


Epidemic::~Epidemic(void)
{
}

void Epidemic::GenerateData()
{
	if(currentTime > DATATIME)
		return;

	for(int i = 0; i < CNode::nodes.size(); i++)
	{
		CNode::nodes.at(i).generateData(currentTime);
	}
}


void Epidemic::SendData()
{
	////记录waiting time信息
	//ofstream waiting_time("waiting-time.txt", ios::app);
	//if( currentTime == startTimeForHotspotSelection )
	//{
	//	waiting_time << logInfo;
	//	waiting_time << "#Time" << TAB << "#MANodeID" << TAB << "#HotspotID" << TAB << "#HotspotType/HotspotAge" << TAB 
	//				 << "#Cover" << TAB << "#Heat" << TAB << "#WaitingTime" << endl;
	//}
	//waiting_time.close();
	//ofstream delivery_hotspot("delivery-hotspot.txt", ios::app);
	//if( currentTime == startTimeForHotspotSelection )
	//{
	//	delivery_hotspot << endl << logInfo;
	//	delivery_hotspot << "#Time" << TAB << "#DeliveryCountForSingleHotspotInThisSlot ..." << endl;
	//}
	////用于存储hotspot及其投递计数的静态拷贝
	////由于输出相关信息时（900s时的热点将在2700s时被输出）该热点的全局唯一指针已经被释放，所以存储的是该热点的浅拷贝，不能用于其他用途，否则将影响热点记录的唯一性
	//static vector<CHotspot> deliveryCounts;  
	//ofstream delivery_statistics("delivery-statistics.txt", ios::app);
	//if( currentTime == startTimeForHotspotSelection )
	//{
	//	delivery_statistics << endl << logInfo;
	//	delivery_statistics << "#Time" << TAB << "#DeliveryAtHotspotCount" << TAB << "#DeliveryTotalCount" << TAB << "#DeliveryAtHotspotPercent" << endl;
	//}
	////用于测试投递计数为0的热点信息，按照投递计数降序输出所有热点的覆盖的position数、node数、ratio、投递计数
	//ofstream hotspot_rank("hotspot-rank.txt", ios::app);
	//if( currentTime == startTimeForHotspotSelection )
	//{
	//	hotspot_rank << endl << logInfo;
	//	hotspot_rank << "#WorkTime" << TAB << "#ID" << TAB << "#Location" << TAB << "#nPosition, nNode" << TAB << "#Ratio" << TAB << "#Tw" << TAB << "#DeliveryCount" << endl;
	//}
	////用于统计过期热点的投递计数时判断是否应当输出时间
	//static bool hasMoreNewRoutes = false;

	////更新所有MA的位置
	//for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); )
	//{

	//	//重置flag为true
	//	iMANode->setFlag(true);
	//	do
	//	{
	//		iMANode->updateLocation(currentTime);
	//		//如果到达sink，投递MA的所有数据
	//		if( CBasicEntity::getDistance( *iMANode, *m_sink) <= TRANS_RANGE )
	//		{
	//			if(iMANode->getBufferSize() > 0)
	//			{
	//				cout << "####  [ MA " << iMANode->getID() << " Sends " << iMANode->getBufferSize() << " Data ]" << endl; 
	//				m_sink->receiveData(iMANode->sendAllData(), currentTime);
	//			}
	//			if( iMANode->routeIsOverdue() )
	//			{
	//				//保留过期路径，用于稍后统计旧热点的投递计数信息
	//				vector<CBasicEntity *> overdueHotspots = iMANode->getRoute()->getWayPoints();

	//				//更新路线，取得新的热点集合
	//				if( m_sink->hasMoreNewRoutes() )
	//				{
	//					//热点集合发生更新，输出已完成统计的热点投递计数集合，和上一轮热点选取的时间
	//					if( ! hasMoreNewRoutes )
	//					{
	//						if( ! deliveryCounts.empty() )
	//						{
	//							//按照投递计数降序排列，2700s时输出900s选出的热点在(900, 1800)期间的投递计数，传入的参数应为1800
	//							int endTime =  currentTime - SLOT_HOTSPOT_UPDATE;
	//							deliveryCounts = CPreprocessor::mergeSortByDeliveryCount(deliveryCounts, (endTime) );
	//							for(vector<CHotspot>::iterator ihotspot = deliveryCounts.begin(); ihotspot != deliveryCounts.end(); ihotspot++)
	//							{
	//								delivery_hotspot << ihotspot->getDeliveryCount( currentTime - SLOT_HOTSPOT_UPDATE ) << TAB ;
	//								hotspot_rank << ihotspot->getTime() << "-" << currentTime - SLOT_HOTSPOT_UPDATE << TAB << ihotspot->getID() << TAB << ihotspot->getX() << "," << ihotspot->getY() << TAB << ihotspot->getNCoveredPosition() << "," << ihotspot->getNCoveredNodes() << TAB 
	//									<< ihotspot->getRatio() << TAB << ihotspot->getWaitingTime(endTime) << TAB << ihotspot->getDeliveryCount(endTime) << endl;

	//							}
	//							delivery_hotspot << endl;
	//							deliveryCounts.clear();
	//						}
	//						delivery_hotspot << currentTime - SLOT_HOTSPOT_UPDATE << TAB ;
	//						delivery_statistics << currentTime - SLOT_HOTSPOT_UPDATE << TAB << CData::getDeliveryAtHotspotCount() << TAB 
	//							<< CData::getDeliveryTotalCount() << TAB << CData::getDeliveryAtHotspotPercent() << endl;
	//						delivery_statistics.close();
	//						hasMoreNewRoutes = true;
	//					}

	//					iMANode->setRoute(m_sink->popRoute());
	//					cout << "####  [ MA " << iMANode->getID() << " Updates Its Route ]" << endl;
	//				}
	//				//FIXME: 若路线数目变少，删除多余的MA
	//				else
	//				{
	//					iMANode->setFlag(false);
	//					cout << "####  [ MA " << iMANode->getID() << " Is Removed ]" << endl;
	//				}

	//				//统计旧热点的投递计数信息
	//				for(vector<CBasicEntity *>::iterator iHotspot = overdueHotspots.begin(); iHotspot != overdueHotspots.end(); iHotspot++)
	//				{
	//					if( (*iHotspot)->getID() == SINK_ID )
	//						continue;
	//					CHotspot *hotspot = (CHotspot *)(*iHotspot);
	//					int count = hotspot->getDeliveryCount();
	//					deliveryCounts.push_back( *hotspot );
	//				}
	//				if( ! iMANode->getFlag() )
	//					break;
	//			}
	//			else if( ! m_sink->hasMoreNewRoutes() )
	//			{
	//				//热点集合更新结束，重置flag
	//				hasMoreNewRoutes = false;
	//			}
	//		}
	//		//如果到达hotspot，waitingTime尚未获取
	//		if(   iMANode->isAtHotspot() 
	//				&& iMANode->getWaitingTime() < 0 )
	//		{
	//			ofstream waiting_time("waiting-time.txt", ios::app);

	//			CHotspot *atHotspot = iMANode->getAtHotspot();
	//			int tmp = ROUND( getWaitingTime(atHotspot) );
	//			//FIXME: 如果不允许Buffer溢出，Buffer已满时即直接跳过waiting
	//			if( (! INFINITE_BUFFER) && (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
	//				iMANode->setWaitingTime( 0 );
	//			else
	//			{
	//				iMANode->setWaitingTime( tmp );
	//				atHotspot->addWaitingTime( tmp );
	//				cout << "####  [ MA " << iMANode->getID() << " Waits For " << tmp << " ]" << endl;
	//				//if(tmp == 0)
	//				//	iMANode->setAtHotspot(NULL);
	//			}

	//			//记录waiting time信息
	//			if( tmp > 0)
	//			{
	//				waiting_time << atHotspot->getTime() << TAB << iMANode->getID() << TAB << atHotspot->getID() << TAB ;
	//				switch( atHotspot->getCandidateType() )
	//				{
	//					case TYPE_OLD_HOTSPOT: 
	//						waiting_time << "O/" ;
	//						break;
	//					case TYPE_MERGE_HOTSPOT: 
	//						waiting_time << "M/" ;
	//						break;
	//					case TYPE_NEW_HOTSPOT: 
	//						waiting_time << "N/" ;
	//						break;
	//					default:
	//						break;
	//				}
	//				waiting_time << atHotspot->getAge() << TAB << atHotspot->getNCoveredPosition() << TAB 
	//							 << atHotspot->getHeat() << TAB << tmp << endl;
	//				waiting_time.close();	
	//			}

	//		}
	//	}while(iMANode->getTime() < currentTime);
	//	
	//	//删除多余的MA
	//	if( ! iMANode->getFlag() )
	//	{
	//		iMANode = m_MANodes.erase(iMANode);
	//		continue;
	//	}
	//	else
	//		iMANode++;	
	//}

	////投递数据
	//for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); iMANode++)
	//{
	//	for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
	//	{
	//		if( (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
	//			break;
	//		if(CBasicEntity::getDistance( (CBasicEntity)*iMANode, (CBasicEntity)*inode ) > TRANS_RANGE)
	//			continue;

	//		//对于热点上和路径上分别统计相遇次数
	//		if( iMANode->isAtHotspot() )
	//			CMANode::encountAtHotspot();
	//		else
	//			CMANode::encountOnRoute();

	//		if( ! inode->hasData() )
	//			continue;

	//		double bet = RandomFloat(0, 1);
	//		if(bet > PROB_DATA_FORWARD)
	//		{
	//			inode->failSendData();
	//			continue;
	//		}

	//		vector<CData> data;
	//		if( INFINITE_BUFFER || BUFFER_OVERFLOW_ALLOWED )
	//		{
	//			cout << "####  [ MA " << iMANode->getID() << " Receives " << inode->getBufferSize() << " Data ]" << endl;
	//			data = inode->sendAllData();
	//			iMANode->receiveData(currentTime, data);
	//		}
	//		else
	//		{
	//			cout << "####  [ MA " << iMANode->getID() << " Receives " << inode->getBufferSize() << " Data ]" << endl;
	//			data = inode->sendData( iMANode->getBufferCapacity() );
	//			iMANode->receiveData(currentTime, data);
	//		}

	//		//对于热点上和路径上分别统计数据投递计数
	//		if( iMANode->isAtHotspot() )
	//		{
	//			CData::deliverAtHotspot( data.size() );
	//			iMANode->getAtHotspot()->addDeliveryCount( data.size() );
	//		}
	//		else
	//			CData::deliverOnRoute( data.size() );

	//	}
	//}

	////更新所有节点的buffer状态记录
	//for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
	//	inode->updateBufferStatus();

	//cout << "####  [ Delivery Ratio ]  " << CData::getDataArrivalCount() / (double)CData::getDataCount() << endl;
	//delivery_hotspot.close();
}

void Epidemic::PrintInfo()
{
	//hotspot选取结果、hotspot class数目、ED、Energy Consumption、节点buffer状态 ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0 
		&& currentTime >= startTimeForHotspotSelection )
	{
	//	//热点个数
	//	ofstream hotspot("hotspot.txt", ios::app);
	//	if(currentTime == startTimeForHotspotSelection)
	//	{
	//		hotspot << logInfo;
	//		hotspot << "#Time" << TAB << "#HotspotCount" << endl;
	//	}
	//	hotspot << currentTime << TAB << m_hotspots.size() << endl; 
	//	hotspot.close();

	//	//用于计算热点个数历史平均值
	//	HOTSPOT_COST_SUM += m_hotspots.size();
	//	HOTSPOT_COST_COUNT++;

	//	//热点质量统计信息
	//	ofstream hotspot_statistics("hotspot-statistics.txt", ios::app);
	//	if(currentTime == startTimeForHotspotSelection)
	//	{
	//		hotspot_statistics << logInfo;
	//		hotspot_statistics << "#Time" << TAB << "#CoverSum" << TAB << "#HotspotCount" << TAB << "#AvgCover" << endl;
	//	}
	//	int sumCover = 0;
	//	for(vector<CHotspot *>::iterator it = m_hotspots.begin(); it != m_hotspots.end(); it++)
	//		sumCover += (*it)->getNCoveredPosition();
	//	hotspot_statistics << currentTime << TAB << sumCover << TAB << m_hotspots.size() << TAB << (double)sumCover / (double)m_hotspots.size() << endl;
	//	hotspot_statistics.close();

	//	//热点归并过程统计信息（在最终选取出的热点集合中）
	//	if( DO_MERGE_HAR )
	//	{
	//		int mergeCount = 0;
	//		int oldCount = 0;
	//		int newCount = 0;
	//		ofstream merge("merge.txt", ios::app);
	//		ofstream merge_details("merge-details.txt", ios::app);

	//		if(currentTime == startTimeForHotspotSelection)
	//		{
	//			merge << logInfo;
	//			merge << "#Time" << TAB << "#MergeHotspotCount" << TAB << "#MergeHotspotPercent" << TAB << "#OldHotspotCount" << TAB 
	//				  << "#OldHotspotPercent" << TAB << "#NewHotspotCount" << TAB << "#NewHotspotPercent" << endl;
	//			merge_details << logInfo;
	//			merge_details << "#Time" << TAB << "#HotspotType/#MergeAge ..." << endl;
	//		}
	//		merge_details << currentTime << TAB;

	//		//热点类型及年龄统计信息
	//		for(vector<CHotspot *>::iterator ihotspot = m_hotspots.begin(); ihotspot != m_hotspots.end(); ihotspot++)
	//		{
	//			if( (*ihotspot)->getCandidateType() == TYPE_MERGE_HOTSPOT )
	//			{
	//				merge_details << "M/" << (*ihotspot)->getAge() << TAB;
	//				mergeCount++;
	//			}
	//			else if( (*ihotspot)->getCandidateType() == TYPE_OLD_HOTSPOT )
	//			{
	//				merge_details << "O/" << (*ihotspot)->getAge() << TAB;
	//				oldCount++;
	//			}
	//			else
	//			{
	//				merge_details << "N/" << (*ihotspot)->getAge() << TAB;
	//				newCount++;
	//			}
	//		}

	//		//三种热点所占的比例
	//		int total = m_hotspots.size();
	//		merge << currentTime << TAB << mergeCount << TAB << (double)mergeCount / (double)total << TAB << oldCount << TAB 
	//			  << (double)oldCount / (double)total << TAB << newCount << TAB << (double)newCount / (double)total << endl;

	//		//用于计算归并热点和旧热点所占比例的历史平均值信息
	//		MERGE_PERCENT_SUM += (double)mergeCount / (double)total;
	//		MERGE_PERCENT_COUNT++;
	//		OLD_PERCENT_SUM += (double)oldCount / (double)total;
	//		OLD_PERCENT_COUNT++;

	//		merge.close();
	//		merge_details.close();
	//	}

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

		//平均能耗
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			energy_consumption << logInfo;
			energy_consumption << "#Time" << TAB << "#AvgEnergyConsumption" << endl;
		}
		energy_consumption << currentTime << TAB << ( CData::getAverageEnergyConsumption() * 100 ) << endl;
		energy_consumption.close();

		//每个节点buffer状态的历史平均值
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			buffer << logInfo;
			buffer << "#Time" << TAB << "#AvgBufferStateInHistoryOfEachNode" << endl;
		}
		buffer << currentTime << TAB;
		for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
			 buffer << inode->getAverageBufferSize() << TAB;
		buffer << endl;
		buffer.close();

		//数据投递率-900（用于debug）
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();
		
	}

	//数据投递率、数据投递时延
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//数据投递率-100（用于绘制曲线）
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//数据投递延迟
		ofstream delay("delay.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delay << logInfo;
			delay << "#Time" << TAB << "#AvgDelay" << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

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

		//每个节点的当前buffer状态
		ofstream node("buffer-node.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			node << logInfo;
			node << "#Time" << TAB << "#BufferStateOfEachNode" << endl;
		}
		node << currentTime << TAB;
		for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
			node << inode->getBufferSize() << "  " ;
		node << endl;
		node.close();

		////buffer溢出计数
		//ofstream overflow("overflow.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	overflow << logInfo;
		//	overflow << "#Time" << TAB << "#OverflowCount" << endl;
		//}
		//overflow << currentTime << TAB << CData::getOverflowCount() << endl;
		//overflow.close();
	}

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
