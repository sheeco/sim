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
	////��¼waiting time��Ϣ
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
	////���ڴ洢hotspot����Ͷ�ݼ����ľ�̬����
	////������������Ϣʱ��900sʱ���ȵ㽫��2700sʱ����������ȵ��ȫ��Ψһָ���Ѿ����ͷţ����Դ洢���Ǹ��ȵ��ǳ��������������������;������Ӱ���ȵ��¼��Ψһ��
	//static vector<CHotspot> deliveryCounts;  
	//ofstream delivery_statistics("delivery-statistics.txt", ios::app);
	//if( currentTime == startTimeForHotspotSelection )
	//{
	//	delivery_statistics << endl << logInfo;
	//	delivery_statistics << "#Time" << TAB << "#DeliveryAtHotspotCount" << TAB << "#DeliveryTotalCount" << TAB << "#DeliveryAtHotspotPercent" << endl;
	//}
	////���ڲ���Ͷ�ݼ���Ϊ0���ȵ���Ϣ������Ͷ�ݼ���������������ȵ�ĸ��ǵ�position����node����ratio��Ͷ�ݼ���
	//ofstream hotspot_rank("hotspot-rank.txt", ios::app);
	//if( currentTime == startTimeForHotspotSelection )
	//{
	//	hotspot_rank << endl << logInfo;
	//	hotspot_rank << "#WorkTime" << TAB << "#ID" << TAB << "#Location" << TAB << "#nPosition, nNode" << TAB << "#Ratio" << TAB << "#Tw" << TAB << "#DeliveryCount" << endl;
	//}
	////����ͳ�ƹ����ȵ��Ͷ�ݼ���ʱ�ж��Ƿ�Ӧ�����ʱ��
	//static bool hasMoreNewRoutes = false;

	////��������MA��λ��
	//for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); )
	//{

	//	//����flagΪtrue
	//	iMANode->setFlag(true);
	//	do
	//	{
	//		iMANode->updateLocation(currentTime);
	//		//�������sink��Ͷ��MA����������
	//		if( CBasicEntity::getDistance( *iMANode, *m_sink) <= TRANS_RANGE )
	//		{
	//			if(iMANode->getBufferSize() > 0)
	//			{
	//				cout << "####  [ MA " << iMANode->getID() << " Sends " << iMANode->getBufferSize() << " Data ]" << endl; 
	//				m_sink->receiveData(iMANode->sendAllData(), currentTime);
	//			}
	//			if( iMANode->routeIsOverdue() )
	//			{
	//				//��������·���������Ժ�ͳ�ƾ��ȵ��Ͷ�ݼ�����Ϣ
	//				vector<CBasicEntity *> overdueHotspots = iMANode->getRoute()->getWayPoints();

	//				//����·�ߣ�ȡ���µ��ȵ㼯��
	//				if( m_sink->hasMoreNewRoutes() )
	//				{
	//					//�ȵ㼯�Ϸ������£���������ͳ�Ƶ��ȵ�Ͷ�ݼ������ϣ�����һ���ȵ�ѡȡ��ʱ��
	//					if( ! hasMoreNewRoutes )
	//					{
	//						if( ! deliveryCounts.empty() )
	//						{
	//							//����Ͷ�ݼ����������У�2700sʱ���900sѡ�����ȵ���(900, 1800)�ڼ��Ͷ�ݼ���������Ĳ���ӦΪ1800
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
	//				//FIXME: ��·����Ŀ���٣�ɾ�������MA
	//				else
	//				{
	//					iMANode->setFlag(false);
	//					cout << "####  [ MA " << iMANode->getID() << " Is Removed ]" << endl;
	//				}

	//				//ͳ�ƾ��ȵ��Ͷ�ݼ�����Ϣ
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
	//				//�ȵ㼯�ϸ��½���������flag
	//				hasMoreNewRoutes = false;
	//			}
	//		}
	//		//�������hotspot��waitingTime��δ��ȡ
	//		if(   iMANode->isAtHotspot() 
	//				&& iMANode->getWaitingTime() < 0 )
	//		{
	//			ofstream waiting_time("waiting-time.txt", ios::app);

	//			CHotspot *atHotspot = iMANode->getAtHotspot();
	//			int tmp = ROUND( getWaitingTime(atHotspot) );
	//			//FIXME: ���������Buffer�����Buffer����ʱ��ֱ������waiting
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

	//			//��¼waiting time��Ϣ
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
	//	//ɾ�������MA
	//	if( ! iMANode->getFlag() )
	//	{
	//		iMANode = m_MANodes.erase(iMANode);
	//		continue;
	//	}
	//	else
	//		iMANode++;	
	//}

	////Ͷ������
	//for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); iMANode++)
	//{
	//	for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
	//	{
	//		if( (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
	//			break;
	//		if(CBasicEntity::getDistance( (CBasicEntity)*iMANode, (CBasicEntity)*inode ) > TRANS_RANGE)
	//			continue;

	//		//�����ȵ��Ϻ�·���Ϸֱ�ͳ����������
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

	//		//�����ȵ��Ϻ�·���Ϸֱ�ͳ������Ͷ�ݼ���
	//		if( iMANode->isAtHotspot() )
	//		{
	//			CData::deliverAtHotspot( data.size() );
	//			iMANode->getAtHotspot()->addDeliveryCount( data.size() );
	//		}
	//		else
	//			CData::deliverOnRoute( data.size() );

	//	}
	//}

	////�������нڵ��buffer״̬��¼
	//for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
	//	inode->updateBufferStatus();

	//cout << "####  [ Delivery Ratio ]  " << CData::getDataArrivalCount() / (double)CData::getDataCount() << endl;
	//delivery_hotspot.close();
}

void Epidemic::PrintInfo()
{
	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption���ڵ�buffer״̬ ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0 
		&& currentTime >= startTimeForHotspotSelection )
	{
	//	//�ȵ����
	//	ofstream hotspot("hotspot.txt", ios::app);
	//	if(currentTime == startTimeForHotspotSelection)
	//	{
	//		hotspot << logInfo;
	//		hotspot << "#Time" << TAB << "#HotspotCount" << endl;
	//	}
	//	hotspot << currentTime << TAB << m_hotspots.size() << endl; 
	//	hotspot.close();

	//	//���ڼ����ȵ������ʷƽ��ֵ
	//	HOTSPOT_COST_SUM += m_hotspots.size();
	//	HOTSPOT_COST_COUNT++;

	//	//�ȵ�����ͳ����Ϣ
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

	//	//�ȵ�鲢����ͳ����Ϣ��������ѡȡ�����ȵ㼯���У�
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

	//		//�ȵ����ͼ�����ͳ����Ϣ
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

	//		//�����ȵ���ռ�ı���
	//		int total = m_hotspots.size();
	//		merge << currentTime << TAB << mergeCount << TAB << (double)mergeCount / (double)total << TAB << oldCount << TAB 
	//			  << (double)oldCount / (double)total << TAB << newCount << TAB << (double)newCount / (double)total << endl;

	//		//���ڼ���鲢�ȵ�;��ȵ���ռ��������ʷƽ��ֵ��Ϣ
	//		MERGE_PERCENT_SUM += (double)mergeCount / (double)total;
	//		MERGE_PERCENT_COUNT++;
	//		OLD_PERCENT_SUM += (double)oldCount / (double)total;
	//		OLD_PERCENT_COUNT++;

	//		merge.close();
	//		merge_details.close();
	//	}

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

		//ƽ���ܺ�
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			energy_consumption << logInfo;
			energy_consumption << "#Time" << TAB << "#AvgEnergyConsumption" << endl;
		}
		energy_consumption << currentTime << TAB << ( CData::getAverageEnergyConsumption() * 100 ) << endl;
		energy_consumption.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
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

		//����Ͷ����-900������debug��
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();
		
	}

	//����Ͷ���ʡ�����Ͷ��ʱ��
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay("delay.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delay << logInfo;
			delay << "#Time" << TAB << "#AvgDelay" << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

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

		//ÿ���ڵ�ĵ�ǰbuffer״̬
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

		////buffer�������
		//ofstream overflow("overflow.txt", ios::app);
		//if(currentTime == startTimeForHotspotSelection)
		//{
		//	overflow << logInfo;
		//	overflow << "#Time" << TAB << "#OverflowCount" << endl;
		//}
		//overflow << currentTime << TAB << CData::getOverflowCount() << endl;
		//overflow.close();
	}

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
