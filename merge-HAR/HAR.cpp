#include "HAR.h"

//void HAR::HotspotSelection(int time)
//{
//
//}

double HAR::getHotspotHeat(CHotspot *hotspot)
{
	int nCoveredNodes = hotspot->getNCoveredNodes();
	double sum_generationRate = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	for(int i = 0; i < coveredNodes.size(); i++)
	{
		if(coveredNodes[i] >= m_nodes.size())
		{
			nCoveredNodes--;
			continue;
		}
		sum_generationRate += m_nodes.at(coveredNodes[i]).getGenerationRate();
	}

	////merge-HAR: ratio
	//double ratio = pow( hotspot->getRatioByCandidateType(), hotspot->getAge() );

	//merge-HAR: ln ratio
	//double ratio = hotspot->getRatioByCandidateType() * log( 1 + hotspot->getAge() );
	//if(ratio == 0)
	//	ratio = 1;

	double ratio = 1;

	return ratio * ( CO_HOTSPOT_HEAT_A1 * nCoveredNodes + CO_HOTSPOT_HEAT_A2 * sum_generationRate ) ;
}

double HAR::getWaitingTime(CHotspot *hotspot, int time)
{
	long double result = 1;
	int count_trueHotspot = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	for(int i = 0; i < coveredNodes.size(); i++)
	{
		int tmp_time = time;
		double tmp;

		//IHAR: Reduce Memory Time
		if(DO_IHAR)
		{
			time = min(time, MAX_MEMORY_TIME);
		}

		tmp = (double) hotspot->getNCoveredPositionsForNode(coveredNodes[i]) / (double) (time + SLOT_HOTSPOT_UPDATE);

		//double tmp = (double) hotspot->getNCoveredPositionsForNode(coveredNodes[i]) / (double) (time + SLOT_HOTSPOT_UPDATE);
		if(tmp >= BETA)
		{
			result *= tmp;
			count_trueHotspot++;
		}
	}
	//FIXME: �������true hotspot��waiting timeΪ0
	if(count_trueHotspot == 0)
		return 0;
	double prob = exp( -1 / hotspot->getHeat() );
	result = prob / result;
	result = pow(result, ( 1 / (double) count_trueHotspot ) );

	return result;
}

double HAR::getSumGenerationRate(vector<int> nodes)
{
	double sum = 0;
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i] >= m_nodes.size())
			continue;
		sum += m_nodes.at(nodes[i]).getGenerationRate();
	}
	return sum;
}

double HAR::getSumGenerationRate(vector<int> nodes_a, vector<int> nodes_b)
{
	double sum = 0;
	addToListUniquely(nodes_a, nodes_b);
	for(int i = 0; i < nodes_a.size(); i++)
	{
		if(nodes_a[i] >= m_nodes.size())
			continue;
		sum += m_nodes.at(nodes_a[i]).getGenerationRate();
	}
	return sum;
}

double HAR::getTimeIncrementForInsertion(CRoute route, int front, CHotspot *hotspot, int time)
{
	double result = getWaitingTime(hotspot, time) + ( route.getAddingDistance(front, hotspot) / SPEED_MANODE );
	return result;
}

double HAR::calculateRatioForInsertion(CRoute route, int front, CHotspot *hotspot, int time)
{
	double time_incr = getTimeIncrementForInsertion(route, front, hotspot, time);
	double sumGenerationRate = getSumGenerationRate(route.getCoveredNodes(), hotspot->getCoveredNodes());
	return ( time_incr * sumGenerationRate );
}

double HAR::calculateEDTime(int time)
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
		sum_waitingTime += getWaitingTime(*ihotspot, time);
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

void HAR::ChangeNodeNumber(int time)
{
	float bet = RandomFloat(-1, 1);
	if(bet > 0)
		bet = 0.2 + bet / 2;
	else
		bet = -0.2 + bet / 2;
	int delta = bet * (NUM_NODE_MAX - NUM_NODE_MIN);
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
			node.setID(i);
			m_nodes.push_back(node);
		}	
	}
	else
	{
		vector<CNode>::iterator start = m_nodes.begin();
		vector<CNode>::iterator end = m_nodes.end();
		vector<CNode> new_nodes(start, start + NUM_NODE + delta);
		m_nodes = new_nodes;
	}
	NUM_NODE += delta;
}

void HAR::UpdateNodeLocations(int time)
{
	//node
	for(int i = 0; i < NUM_NODE; i++)
	{
		double x = 0, y = 0;
		CFileParser::getPositionFromFile(i, time, x, y);
		m_nodes[i].setLocation(x, y, time);
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

void HAR::HotspotClassification(int time, vector<CHotspot *> hotspots)
{
	vector<CHotspot *> tmp_hotspots = hotspots;
	this->m_hotspots = hotspots;
	for(vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin(); ihotspot != tmp_hotspots.end(); ihotspot++)
	{
		(*ihotspot)->setHeat( getHotspotHeat(*ihotspot) );
	}

	//merge-HAR
	//FIXME:
	
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

				time_increment = getTimeIncrementForInsertion(route, best_front, tmp_hotspots[ihotspot], time);
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


void HAR::MANodeRouteDesign(int time)
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
			CMANode ma(m_sink->popRoute(), time);
			m_MANodes.push_back(ma);
		}
	}

	for(vector<CRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); iroute++)
		cout << "##  " << iroute->toString();
}


void HAR::GenerateData(int time)
{
	if(time > DATATIME)
		return;

	for(int i = 0; i < NUM_NODE; i++)
	{
		m_nodes.at(i).generateData(time);
	}
}


void HAR::SendData(int time)
{
	//��������MA��λ��
	for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); )
	{
		//����flagΪtrue
		iMANode->setFlag(true);
		do
		{
			iMANode->updateLocation(time);
			//�������sink��Ͷ��MA����������
			if( CBase::getDistance( *iMANode, *m_sink) <= TRANS_RANGE )
			{
				if(iMANode->getBufferSize() > 0)
				{
					cout << "####  [ MA " << iMANode->getID() << " Sends " << iMANode->getBufferSize() << " Data ]" << endl; 
					m_sink->receiveData(iMANode->sendAllData(), time);
				}
				if( iMANode->routeIsOverdue() )
				{
					//ȡ����·��
					if( m_sink->hasMoreNewRoutes() )
					{
						iMANode->setRoute(m_sink->popRoute());
						cout << "####  [ MA " << iMANode->getID() << " Updates Its Route ]" << endl;
					}
					//FIXME: ��·����Ŀ���٣�ɾ�������MA
					else
					{
						iMANode->setFlag(false);
						cout << "####  [ MA " << iMANode->getID() << " Is Removed ]" << endl;
						break;
					}
				}
			}
			//�������hotspot��waitingTime��δ��ȡ
			if(   iMANode->isAtHotspot() 
					&& iMANode->getWaitingTime() < 0 )
			{
				//FIXME: ���������Buffer�����Buffer����ʱ��ֱ������waiting
				if( (! INFINITE_BUFFER) && (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
					;
				else
				{
					int tmp = getWaitingTime(iMANode->getAtHotspot(), time);
					iMANode->setWaitingTime( tmp );
					cout << "####  [ MA " << iMANode->getID() << " Waits For " << tmp << " ]" << endl;
				}
				iMANode->setAtHotspot(NULL);
			}
		}while(iMANode->getTime() < time);
		
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
		for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
		{
			if( (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
				break;
			if(CBase::getDistance( (CBase)*iMANode, (CBase)*inode ) > TRANS_RANGE)
				continue;

			if( ! inode->hasData() )
				continue;

			double bet = RandomFloat(0, 1);
			if(bet > PROB_DATA_FORWARD)
			{
				inode->failSendData();
				continue;
			}

			if( INFINITE_BUFFER || BUFFER_OVERFLOW_ALLOWED )
			{
				cout << "####  [ MA " << iMANode->getID() << " Receives " << inode->getBufferSize() << " Data ]" << endl;
				iMANode->receiveData(time, inode->sendAllData());
			}
			else
			{
				cout << "####  [ MA " << iMANode->getID() << " Receives " << inode->getBufferSize() << " Data ]" << endl;
				iMANode->receiveData(time, inode->sendData( iMANode->getBufferCapacity() ) );
			}
		}
	}

	//�������нڵ��buffer״̬��¼
	for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
		inode->updateBufferStatus();

	cout << "####  [ Delivery Ratio ]  " << CData::getDataArrivalCount() / (double)CData::getDataCount() << endl;
}

void HAR::PrintInfo(int time)
{
	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption���ڵ�buffer״̬
	if(time % SLOT_HOTSPOT_UPDATE == 0)
	{
		ofstream hotspot("hotspot.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			hotspot << logInfo;
			hotspot << "#Time" << TAB << "#HotspotCount" << endl;
		}
		hotspot << time << TAB << g_selectedHotspots.size() << endl; 
		//CFileParser::writeHotspotToFile("hotspot.txt", m_hotspots, false);

		//CFileParser::writeHotspotToFile("hotspot-details.txt", m_hotspots, true);
		ofstream hotspot_statistics("hotspot-statistics.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			hotspot_statistics << logInfo;
			hotspot_statistics << "#Time" << TAB << "#CoverSum" << TAB << "#HotspotCount" << TAB << "#CoverAvg" << endl;
		}
		CFileParser::writeHotspotStatisics(time, "hotspot-statistics.txt", m_hotspots);

		//ofstream hotspot_class("class.txt", ios::app);
		//hotspot_class << time << TAB << m_routes.size() << endl;
		//hotspot_class.close();

		//FIXME: ����ED
		ofstream ed("ed.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			ed << logInfo;
			ed << "#Time" << TAB << "#EstimatedDelay" << endl;
		}
		ed << time << TAB << calculateEDTime(time) << endl;
		ed.close();

		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			energy_consumption << logInfo;
			energy_consumption << "#Time" << TAB << "#EnergyConsumptionAvg" << endl;
		}
		energy_consumption << time << TAB << ( getAverageEnergyConsumption() * 100 ) << endl;
		energy_consumption.close();

		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			buffer << logInfo;
			buffer << "#Time" << TAB << "#AvgBufferStateInHistoryOfEachNode" << endl;
		}
		buffer << time << TAB;
		for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
			 buffer << inode->getAverageBufferSize() << TAB;
		buffer << endl;
		buffer.close();
	}

	//����Ͷ���ʡ�����Ͷ��ʱ��
	if(time % SLOT_RECORD_INFO == 0)
	{
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << time << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		ofstream delay("delay.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			delay << logInfo;
			delay << "#Time" << TAB << "#Delay" << endl;
		}
		delay << time << TAB << CData::getAverageDelay() << endl;
		delay.close();

	}

	static int tmp_nDelivery;
	static int tmp_nData;
	if( time % ( SLOT_HOTSPOT_UPDATE ) == 0 )
	{
		tmp_nDelivery = CData::getDataArrivalCount() - tmp_nDelivery;
		tmp_nData = CData::getDataCount() - tmp_nData;

		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << time << TAB << tmp_nDelivery << TAB << tmp_nData << TAB << (double)tmp_nDelivery / tmp_nData << endl;
		delivery_ratio.close();
		
		ofstream ma("ma.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			ma << logInfo;
			ma << "#Time" << TAB << "#MACount" << endl;
		}
		ma << time << TAB << m_routes.size() << endl;
		ma.close();
	}

	if(time % SLOT_RECORD_INFO == 0
		|| time == RUNTIME)
	{
		cout << "####  [ MA State ]  ";
		for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
			cout << iMA->getBufferSize() << "  " ;
		cout << endl;

		ofstream ma("buffer-ma.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			ma << logInfo;
			ma << "#Time" << TAB << "#BufferStateOfEachMA" << endl;
		}
		ma << time << TAB;
		for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
			ma << iMA->getBufferSize() << TAB ;
		ma << endl;
		ma.close();

		ofstream node("buffer-node.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			node << logInfo;
			node << "#Time" << TAB << "#BufferStateOfEachNode" << endl;
		}
		node << time << TAB;
		for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
			node << inode->getBufferSize() << "  " ;
		node << endl;
		node.close();

		ofstream overflow("overflow.txt", ios::app);
		if(time == startTimeForHotspotSelection)
		{
			overflow << logInfo;
			overflow << "#Time" << TAB << "#OverflowCount" << endl;
		}
		overflow << time << TAB << CData::getOverflowCount() << endl;
		overflow.close();
	}

}