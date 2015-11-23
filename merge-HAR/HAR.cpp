#include "HAR.h"

int HAR::HOTSPOT_COST_SUM = 0;
int HAR::HOTSPOT_COST_COUNT = 0;
int HAR::MA_COST_SUM = 0;
int HAR::MA_COST_COUNT = 0;
double HAR::MA_WAYPOINT_SUM = 0;
int HAR::MA_WAYPOINT_COUNT = 0;
double HAR::MERGE_PERCENT_SUM = 0;
int HAR::MERGE_PERCENT_COUNT = 0;
double HAR::OLD_PERCENT_SUM = 0;
int HAR::OLD_PERCENT_COUNT = 0;
double HAR::SIMILARITY_RATIO_SUM = 0;
int HAR::SIMILARITY_RATIO_COUNT = 0;

double HAR::getHotspotHeat(CHotspot *hotspot)
{
	int nCoveredNodes = hotspot->getNCoveredNodes();
	double sum_generationRate = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	for(int i = 0; i < coveredNodes.size(); i++)
	{
		if( ! CNode::ifNodeExists( coveredNodes[i] ) )
		{
			nCoveredNodes--;
			continue;
		}
		sum_generationRate += CNode::getNodeByID( coveredNodes[i] ).getGenerationRate();
	}

	double ratio = 1;

	if( HEAT_RATIO_EXP )
	{
		//merge-HAR: exp ratio
		ratio *= pow( hotspot->getCoByCandidateType(), hotspot->getAge() );
	}
	else if( HEAT_RATIO_LN )
	{
		//merge-HAR: ln ratio
		ratio *= hotspot->getCoByCandidateType() * ( 1 + log( 1 + hotspot->getAge() ) );
	}

	return ratio * ( CO_HOTSPOT_HEAT_A1 * nCoveredNodes + CO_HOTSPOT_HEAT_A2 * sum_generationRate ) ;
}

double HAR::getWaitingTime(CHotspot *hotspot)
{
	long double result = 1;
	int count_trueHotspot = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	//DBG:
	vector<int> nCoveredPositionsForNode;

	for(int i = 0; i < coveredNodes.size(); i++)
	{
		int tmp_time = currentTime;
		double tmp;

		//IHAR: Reduce Memory currentTime
		if(DO_IHAR)
		{
			tmp_time = min(currentTime, MAX_MEMORY_TIME);
		}

		//DBG:
		nCoveredPositionsForNode.push_back( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) );
		tmp = (double) hotspot->getNCoveredPositionsForNode(coveredNodes[i]) / (double) (tmp_time + SLOT_HOTSPOT_UPDATE);

		//merge-HAR: ratio
		tmp *= pow( hotspot->getCoByCandidateType(), hotspot->getAge() );

		//double tmp = (double) hotspot->getNCoveredPositionsForNode(coveredNodes[i]) / (double) (currentTime + SLOT_HOTSPOT_UPDATE);
		if(tmp >= BETA)
		{
			result *= tmp;
			count_trueHotspot++;
		}
	}
	//FIXME: �������true hotspot��waiting timeΪ0
	if(count_trueHotspot == 0)
		return MIN_WAITING_TIME;
	double prob = exp( -1 / hotspot->getHeat() );
	result = prob / result;
	result = pow(result, ( 1 / (double) count_trueHotspot ) );

	return result + MIN_WAITING_TIME;
}

double HAR::getSumGenerationRate(vector<int> nodes)
{
	double sum = 0;
	for(int i = 0; i < nodes.size(); i++)
	{
		if( ! CNode::ifNodeExists( nodes[i] ) )
			continue;
		sum += CNode::getNodeByID( nodes[i] ).getGenerationRate();
	}
	return sum;
}

double HAR::getSumGenerationRate(vector<int> nodes_a, vector<int> nodes_b)
{
	double sum = 0;
	addToListUniquely(nodes_a, nodes_b);
	for(int i = 0; i < nodes_a.size(); i++)
	{
		if( ! CNode::ifNodeExists( nodes_a[i] ) )
			continue;
		sum += CNode::getNodeByID( nodes_a[i] ).getGenerationRate();
	}
	return sum;
}

double HAR::getTimeIncrementForInsertion(CRoute route, int front, CHotspot *hotspot)
{
	double result = getWaitingTime(hotspot) + ( route.getAddingDistance(front, hotspot) / SPEED_MANODE );
	return result;
}

double HAR::calculateRatioForInsertion(CRoute route, int front, CHotspot *hotspot)
{
	double time_incr = getTimeIncrementForInsertion(route, front, hotspot);
	double sumGenerationRate = getSumGenerationRate(route.getCoveredNodes(), hotspot->getCoveredNodes());
	return ( time_incr * sumGenerationRate );
}

double HAR::calculateEDTime()
{
	double sum_length = 0;
	double avg_length = 0;
	double sum_waitingTime = 0;
	double avg_waitingTime = 0;
	double avg_u = 0;
	double pmh = 0;
	double sum_pm = 0;
	double avg_pw = 0;
	//double pw = 0.6;  //??
	double EM = 0;
	double EIM = 0;
	double ED = 0;

	for(vector<CRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); iroute++)
		sum_length += iroute->getLength();
	avg_length = sum_length / m_hotspots.size() + 1;
	for(vector<CHotspot *>::iterator ihotspot = m_hotspots.begin(); ihotspot != m_hotspots.end(); ihotspot++)
	{
		sum_waitingTime += getWaitingTime(*ihotspot);
		sum_pm += exp( -1 / (*ihotspot)->getHeat() );
	}
	avg_waitingTime = sum_waitingTime / m_hotspots.size();
	avg_u = avg_length / SPEED_MANODE + avg_waitingTime;
	avg_pw = sum_pm / m_hotspots.size();
	pmh = sum_waitingTime / (sum_length / SPEED_MANODE + sum_waitingTime);
	EM = (1 - pmh) * ( ( (1 - avg_pw) / avg_pw) * avg_u + (avg_length / (2 * SPEED_MANODE) + avg_waitingTime) ) + pmh * ( ( (1 - avg_pw) / avg_pw) * avg_u + avg_waitingTime);
	EIM = avg_u / avg_pw;
	ED = EM + ( (1 - PROB_DATA_FORWARD) / PROB_DATA_FORWARD ) * EIM + ( (double)m_hotspots.size() / (2 * m_routes.size()) ) * avg_u;

	return ED;
}

void HAR::ChangeNodeNumber()
{
	float bet = RandomFloat(-1, 1);
	if(bet > 0)
		bet = 0.2 + bet / 2;
	else
		bet = -0.2 + bet / 2;
	int delta = ROUND( bet * (NUM_NODE_MAX - NUM_NODE_MIN) );
	if(delta == 0)
		return;
	if(delta < NUM_NODE_MIN - NUM_NODE)
		delta = NUM_NODE_MIN - NUM_NODE;
	else if(delta > NUM_NODE_MAX - NUM_NODE)
		delta = NUM_NODE_MAX - NUM_NODE;

	if(delta > 0)
	{
		for(int i = NUM_NODE; i < NUM_NODE + delta; i++)
		{
			double generationRate = RATE_DATA_GENERATE;
			if(i % 5 == 0)
				generationRate *= 5;
			CNode node(generationRate, BUFFER_CAPACITY_NODE);
			node.generateID();
			CNode::nodes.push_back(node);
			CNode::idNodes.push_back( node.getID() );
		}	
	}
	else
	{
		//FIXME: Random selected ?
		vector<CNode>::iterator start = CNode::nodes.begin();
		vector<CNode>::iterator end = CNode::nodes.end();
		vector<CNode> new_nodes(start, start + NUM_NODE + delta);

		//Remove invalid positoins belonging to the deleted nodes
		vector<int> deletedNodes;
		for(vector<CNode>::iterator inode = start + NUM_NODE + delta; inode != end; inode++)
			deletedNodes.push_back( inode->getID() );
		for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
		{
			if( ifExists(deletedNodes, (*ipos)->getNode()) )
				ipos = CPosition::positions.erase(ipos);
			else
				ipos++;
		}

		CNode::nodes = new_nodes;
		CNode::idNodes.erase( CNode::idNodes.begin(), CNode::idNodes.end() );
		for(vector<CNode>::iterator inode = new_nodes.begin(); inode != new_nodes.end(); inode++)
			CNode::idNodes.push_back( inode->getID() );
	}
	NUM_NODE += delta;
}

void HAR::UpdateNodeLocations()
{
	//node
	for(int i = 0; i < CNode::nodes.size(); i++)
	{
		double x = 0, y = 0;
		CFileParser::getPositionFromFile(i, currentTime, x, y);
		CNode::nodes[i].setLocation(x, y, currentTime);
	}
}

void HAR::OptimizeRoute(CRoute &route)
{
	vector<CBase *> waypoints = route.getWayPoints();
	CBase *current = route.getSink();
	CRoute result(current);
	waypoints.erase(waypoints.begin());
	while(! waypoints.empty())
	{
		double min_distance = -1;
		vector<CBase *>::iterator min_point;
		for(vector<CBase *>::iterator ipoint = waypoints.begin(); ipoint != waypoints.end(); ipoint++)
		{
			double distance = CBase::getDistance(**ipoint, *current);
			if( min_distance < 0 
				|| distance < min_distance)
			{
				min_distance = distance;
				min_point = ipoint;
			}
		}
		result.AddPoint(*min_point);
		current = *min_point;
		waypoints.erase(min_point);
	}
	result.updateLength();
	route = result;
}

void HAR::HotspotSelection()
{
	CGreedySelection greedySelection;

	/**************************** �ȵ�鲢����(merge-HAR) *****************************/
	//merge-HAR: 
	if(DO_MERGE_HAR)
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

void HAR::HotspotClassification()
{
	vector<CHotspot *> tmp_hotspots = CHotspot::selectedHotspots;
	this->m_hotspots = CHotspot::selectedHotspots;
	for(vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin(); ihotspot != tmp_hotspots.end(); ihotspot++)
	{
		(*ihotspot)->setHeat( getHotspotHeat(*ihotspot) );
	}
	
	vector<CRoute> newRoutes;
	
	while(! tmp_hotspots.empty())
	{
		//����һ��hotspot class
		CRoute route(m_sink);
		double current_time_cost = 0;
		double current_buffer = 0;
		while(true)
		{
			if( tmp_hotspots.empty() )
				break;
			//ѭ����class�����hotspot��ֱ��buffer����
			double max_ratio = 0;
			int max_front = -1;
			int max_hotspot = -1;
			double time_increment = 0;
			double max_time_increment = 0;
			double sum_generationRate = 0;
			double max_sum_ge = 0;
			double ratio = 0;
			int size_waypoints = route.getNWayPoints();
			//��������ʣ��hotspot��ѡ��ratio����hotspot���
			for(int ihotspot = 0; ihotspot < tmp_hotspots.size(); ihotspot++)
			{
				double min_length_increment = -1;
				double best_front = -1;
				for(int i = 0; i < size_waypoints; i++)  //��Ѱ����С·����������
				{
					double length_increment = route.getAddingDistance(i, tmp_hotspots[ihotspot]);
					if(length_increment < min_length_increment
						|| min_length_increment < 0)
					{
						min_length_increment = length_increment;
						best_front = i;
					}
				}

				time_increment = getTimeIncrementForInsertion(route, best_front, tmp_hotspots[ihotspot]);
				sum_generationRate = getSumGenerationRate(route.getCoveredNodes(), tmp_hotspots[ihotspot]->getCoveredNodes());
				ratio = time_increment * sum_generationRate;  //sum_ge�ظ����㣿��
				if(ratio > max_ratio)
				{
					max_ratio = ratio;
					max_front = best_front;
					max_hotspot = ihotspot;
					max_time_increment = time_increment;
					max_sum_ge = sum_generationRate;
				}
			}
			//���buffer��С
			double new_buffer = ( current_time_cost + max_time_increment ) * max_sum_ge;
			//��������ȵ�Buffer�������󣬾ͽ��䵥�������һ��MA
			//FIXME: ���������
			if( new_buffer > BUFFER_CAPACITY_MA
				&& route.getNWayPoints() == 1)
			{
				//cout<<"Error: HAR::HotspotClassification() a singe hotspot's buffer expection > BUFFER_CAPACITY_MA"<<endl;
				//_PAUSE;
				route.AddPoint(max_front, tmp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin() + max_hotspot;
				tmp_hotspots.erase(ihotspot);
				break;
			}
			if( new_buffer > BUFFER_CAPACITY_MA)
				break;
			else
			{
				current_time_cost += max_time_increment;
				current_buffer = new_buffer;
				route.AddPoint(max_front, tmp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin() + max_hotspot;
				tmp_hotspots.erase(ihotspot);
			}
		}
		newRoutes.push_back(route);
	}
	//���õ����µ�hotspot class����sink��route�б�
	m_sink->setNewRoutes(newRoutes);
}


void HAR::MANodeRouteDesign()
{
	vector<CRoute> routes = m_sink->getNewRoutes();
	//��ÿ�������·��������ھ��㷨�����Ż�
	for(vector<CRoute>::iterator iroute = routes.begin(); iroute != routes.end(); iroute++)
	{
		OptimizeRoute( *iroute );
	}
	m_routes = routes;
	m_sink->setNewRoutes(routes);

	//֪ͨ��ǰ������MA·���ѹ��ڣ���������
	for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); iMANode++)
	{
		iMANode->setRouteOverdue(true);
	}

	//��������·�߷�����µ�MA
	if(m_MANodes.size() < routes.size())
	{
		int num_newMANodes = routes.size() - m_MANodes.size();
		for(int i = 0; i < num_newMANodes; i++)
		{
			CMANode ma(m_sink->popRoute(), currentTime);
			m_MANodes.push_back(ma);
		}
	}

	for(vector<CRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); iroute++)
		cout << "##  " << iroute->toString();
}


void HAR::GenerateData()
{
	if(currentTime > DATATIME)
		return;

	for(int i = 0; i < CNode::nodes.size(); i++)
	{
		CNode::nodes.at(i).generateData(currentTime);
	}
}


void HAR::SendData()
{
	//��¼waiting time��Ϣ
	ofstream waiting_time("waiting-time.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		waiting_time << logInfo;
		waiting_time << "#Time" << TAB << "#MANodeID" << TAB << "#HotspotID" << TAB << "#HotspotType/HotspotAge" << TAB 
					 << "#Cover" << TAB << "#Heat" << TAB << "#WaitingTime" << endl;
	}
	waiting_time.close();
	ofstream delivery_hotspot("delivery-hotspot.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		delivery_hotspot << endl << logInfo;
		delivery_hotspot << "#Time" << TAB << "#DeliveryCountForSingleHotspotInThisSlot ..." << endl;
	}
	static vector<int> deliveryCounts;  //���ڴ洢hotspotͶ�ݼ���
	ofstream delivery_statistics("delivery-statistics.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		delivery_statistics << endl << logInfo;
		delivery_statistics << "#Time" << TAB << "#DeliveryAtHotspotCount" << TAB << "#DeliveryTotalCount" << TAB << "#DeliveryAtHotspotPercent" << endl;
	}
	//����ͳ�ƹ����ȵ��Ͷ�ݼ���ʱ�ж��Ƿ�Ӧ�����ʱ��
	static bool hasMoreNewRoutes = false;

	//��������MA��λ��
	for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); )
	{

		//����flagΪtrue
		iMANode->setFlag(true);
		do
		{
			iMANode->updateLocation(currentTime);
			//�������sink��Ͷ��MA����������
			if( CBase::getDistance( *iMANode, *m_sink) <= TRANS_RANGE )
			{
				if(iMANode->getBufferSize() > 0)
				{
					cout << "####  [ MA " << iMANode->getID() << " Sends " << iMANode->getBufferSize() << " Data ]" << endl; 
					m_sink->receiveData(iMANode->sendAllData(), currentTime);
				}
				if( iMANode->routeIsOverdue() )
				{
					//��������·���������Ժ�ͳ�ƾ��ȵ��Ͷ�ݼ�����Ϣ
					vector<CBase *> overdueHotspots = iMANode->getRoute()->getWayPoints();

					//����·�ߣ�ȡ���µ��ȵ㼯��
					if( m_sink->hasMoreNewRoutes() )
					{
						//�ȵ㼯�Ϸ������£���������ͳ�Ƶ��ȵ�Ͷ�ݼ������ϣ�����һ���ȵ�ѡȡ��ʱ��
						if( ! hasMoreNewRoutes )
						{
							if( ! deliveryCounts.empty() )
							{
								//�Ӵ�С�������
								deliveryCounts = CPreprocessor::mergeSort(deliveryCounts, CPreprocessor::smaller);
								for(vector<int>::iterator icount = deliveryCounts.begin(); icount != deliveryCounts.end(); icount++)
									delivery_hotspot << *icount << TAB ;
								delivery_hotspot << endl;
								deliveryCounts.erase( deliveryCounts.begin(), deliveryCounts.end() );
							}
							delivery_hotspot << currentTime - SLOT_HOTSPOT_UPDATE << TAB ;
							delivery_statistics << currentTime - SLOT_HOTSPOT_UPDATE << TAB << CData::getDeliveryAtHotspotCount() << TAB 
								<< CData::getDeliveryTotalCount() << TAB << CData::getDeliveryAtHotspotPercent() << endl;
							delivery_statistics.close();
							hasMoreNewRoutes = true;
						}

						iMANode->setRoute(m_sink->popRoute());
						cout << "####  [ MA " << iMANode->getID() << " Updates Its Route ]" << endl;
					}
					//FIXME: ��·����Ŀ���٣�ɾ�������MA
					else
					{
						iMANode->setFlag(false);
						cout << "####  [ MA " << iMANode->getID() << " Is Removed ]" << endl;
					}

					//ͳ�ƾ��ȵ��Ͷ�ݼ�����Ϣ
					for(vector<CBase *>::iterator iHotspot = overdueHotspots.begin(); iHotspot != overdueHotspots.end(); iHotspot++)
					{
						if( (*iHotspot)->getID() == SINK_ID )
							continue;
						CHotspot *hotspot = (CHotspot *)(*iHotspot);
						deliveryCounts.push_back( hotspot->getDeliveryCount() );
					}
					if( ! iMANode->getFlag() )
						break;
				}
				else if( ! m_sink->hasMoreNewRoutes() )
				{
					//�ȵ㼯�ϸ��½���������flag
					hasMoreNewRoutes = false;
				}
			}
			//�������hotspot��waitingTime��δ��ȡ
			if(   iMANode->isAtHotspot() 
					&& iMANode->getWaitingTime() < 0 )
			{
				ofstream waiting_time("waiting-time.txt", ios::app);

				CHotspot *atHotspot = iMANode->getAtHotspot();
				int tmp = ROUND( getWaitingTime(atHotspot) );
				//FIXME: ���������Buffer�����Buffer����ʱ��ֱ������waiting
				if( (! INFINITE_BUFFER) && (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
					iMANode->setWaitingTime( 0 );
				else
				{
					iMANode->setWaitingTime( tmp );
					cout << "####  [ MA " << iMANode->getID() << " Waits For " << tmp << " ]" << endl;
					//if(tmp == 0)
					//	iMANode->setAtHotspot(NULL);
				}

				//��¼waiting time��Ϣ
				if( tmp > 0)
				{
					waiting_time << atHotspot->getTime() << TAB << iMANode->getID() << TAB << atHotspot->getID() << TAB ;
					switch( atHotspot->getCandidateType() )
					{
						case TYPE_OLD_HOTSPOT: 
							waiting_time << "O/" ;
							break;
						case TYPE_MERGE_HOTSPOT: 
							waiting_time << "M/" ;
							break;
						case TYPE_NEW_HOTSPOT: 
							waiting_time << "N/" ;
							break;
						default:
							break;
					}
					waiting_time << atHotspot->getAge() << TAB << atHotspot->getNCoveredPosition() << TAB 
								 << atHotspot->getHeat() << TAB << tmp << endl;
					waiting_time.close();	
				}

			}
		}while(iMANode->getTime() < currentTime);
		
		//ɾ�������MA
		if( ! iMANode->getFlag() )
		{
			iMANode = m_MANodes.erase(iMANode);
			continue;
		}
		else
			iMANode++;	
	}

	//Ͷ������
	for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); iMANode++)
	{
		for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
		{
			if( (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
				break;
			if(CBase::getDistance( (CBase)*iMANode, (CBase)*inode ) > TRANS_RANGE)
				continue;

			//�����ȵ��Ϻ�·���Ϸֱ�ͳ����������
			if( iMANode->isAtHotspot() )
				CMANode::encountAtHotspot();
			else
				CMANode::encountOnRoute();

			if( ! inode->hasData() )
				continue;

			double bet = RandomFloat(0, 1);
			if(bet > PROB_DATA_FORWARD)
			{
				inode->failSendData();
				continue;
			}

			vector<CData> data;
			if( INFINITE_BUFFER || BUFFER_OVERFLOW_ALLOWED )
			{
				cout << "####  [ MA " << iMANode->getID() << " Receives " << inode->getBufferSize() << " Data ]" << endl;
				data = inode->sendAllData();
				iMANode->receiveData(currentTime, data);
			}
			else
			{
				cout << "####  [ MA " << iMANode->getID() << " Receives " << inode->getBufferSize() << " Data ]" << endl;
				data = inode->sendData( iMANode->getBufferCapacity() );
				iMANode->receiveData(currentTime, data);
			}

			//�����ȵ��Ϻ�·���Ϸֱ�ͳ������Ͷ�ݼ���
			if( iMANode->isAtHotspot() )
			{
				CData::deliverAtHotspot( data.size() );
				iMANode->getAtHotspot()->addDeliveryCount( data.size() );
			}
			else
				CData::deliverOnRoute( data.size() );

		}
	}

	//�������нڵ��buffer״̬��¼
	for(vector<CNode>::iterator inode = CNode::nodes.begin(); inode != CNode::nodes.end(); inode++)
		inode->updateBufferStatus();

	cout << "####  [ Delivery Ratio ]  " << CData::getDataArrivalCount() / (double)CData::getDataCount() << endl;
	delivery_hotspot.close();
}

void HAR::PrintInfo()
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

		//MA�ڵ����
		ofstream ma("ma.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ma << logInfo;
			ma << "#Time" << TAB << "#MACount" << TAB << "#AvgMAWayPointCount" << endl;
		}
		ma << currentTime << TAB << m_routes.size() << TAB << ( (double)m_hotspots.size() / (double)m_routes.size() ) << endl;
		ma.close();

		//���ڼ���MA�ڵ��������ʷƽ��ֵ��Ϣ
		MA_COST_SUM += m_routes.size();
		MA_COST_COUNT++;
		//���ڼ���MA·�㣨�ȵ㣩ƽ����������ʷƽ��ֵ��Ϣ
		MA_WAYPOINT_SUM += (double)m_hotspots.size() / (double)m_routes.size();
		MA_WAYPOINT_COUNT++;

		//ED��ƽ��Ͷ���ӳٵ�����ֵ
		ofstream ed("ed.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ed << logInfo;
			ed << "#Time" << TAB << "#EstimatedDelay" << endl;
		}
		ed << currentTime << TAB << calculateEDTime() << endl;
		ed.close();

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

		//MA�ͽڵ����������ͳ����Ϣ
		ofstream encounter("encounter.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			encounter << logInfo;
			encounter << "#Time" << TAB << "#EncounterAtHotspot" << TAB << "#Encounter" << TAB << "#EncounterPercentAtHotspot" << endl;
		}
		encounter << currentTime << TAB << CMANode::getEncounterAtHotspot() << TAB << CMANode::getEncounter() << TAB << CMANode::getEncounterPercentAtHotspot() << endl;
		encounter.close();

		//log���
		cout << "####  [ MA State ]  ";
		for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
			cout << iMA->getBufferSize() << "  " ;
		cout << endl;

		//ÿ��MA�ĵ�ǰbuffer״̬
		ofstream ma("buffer-ma.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ma << logInfo;
			ma << "#Time" << TAB << "#BufferStateOfEachMA" << endl;
		}
		ma << currentTime << TAB;
		for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
			ma << iMA->getBufferSize() << TAB ;
		ma << endl;
		ma.close();

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

		//buffer�������
		ofstream overflow("overflow.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			overflow << logInfo;
			overflow << "#Time" << TAB << "#OverflowCount" << endl;
		}
		overflow << currentTime << TAB << CData::getOverflowCount() << endl;
		overflow.close();
	}

	//����debug���
	if( currentTime == RUNTIME )
	{
		debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << getAverageHotspotCost() << TAB ;
		if(DO_MERGE_HAR)
			debugInfo << getAverageMergePercent() << TAB << getAverageOldPercent() << TAB ;
		debugInfo << getAverageMACost() << TAB ;
		if(TEST_HOTSPOT_SIMILARITY)
			debugInfo << getAverageSimilarityRatio() << TAB ;
		debugInfo << CData::getDeliveryAtHotspotPercent() << TAB << logInfo.replace(0, 1, "");
	}

}

void HAR::CompareWithOldHotspots()
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
