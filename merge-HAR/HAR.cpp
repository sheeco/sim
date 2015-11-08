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
		if(coveredNodes[i] >= m_nodes.size())
		{
			nCoveredNodes--;
			continue;
		}
		sum_generationRate += m_nodes.at(coveredNodes[i]).getGenerationRate();
	}

	double ratio = 1;

	if( HEAT_RATIO_EXP )
	{
		//merge-HAR: exp ratio
		ratio *= pow( hotspot->getRatioByCandidateType(), hotspot->getAge() );
	}
	else if( HEAT_RATIO_LN )
	{
		//merge-HAR: ln ratio
		ratio *= hotspot->getRatioByCandidateType() * ( 1 + log( 1 + hotspot->getAge() ) );
	}

	return ratio * ( CO_HOTSPOT_HEAT_A1 * nCoveredNodes + CO_HOTSPOT_HEAT_A2 * sum_generationRate ) ;
}

double HAR::getWaitingTime(CHotspot *hotspot)
{
	long double result = 1;
	int count_trueHotspot = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	for(int i = 0; i < coveredNodes.size(); i++)
	{
		int tmp_time = currentTime;
		double tmp;

		//IHAR: Reduce Memory currentTime
		if(DO_IHAR)
		{
			tmp_time = min(currentTime, MAX_MEMORY_TIME);
		}

		tmp = (double) hotspot->getNCoveredPositionsForNode(coveredNodes[i]) / (double) (tmp_time + SLOT_HOTSPOT_UPDATE);

		//merge-HAR: ratio
		tmp *= pow( hotspot->getRatioByCandidateType(), hotspot->getAge() );

		//double tmp = (double) hotspot->getNCoveredPositionsForNode(coveredNodes[i]) / (double) (currentTime + SLOT_HOTSPOT_UPDATE);
		if(tmp >= BETA)
		{
			result *= tmp;
			count_trueHotspot++;
		}
	}
	//FIXME: 如果不是true hotspot，waiting time为0
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

void HAR::UpdateNodeLocations()
{
	//node
	for(int i = 0; i < NUM_NODE; i++)
	{
		double x = 0, y = 0;
		CFileParser::getPositionFromFile(i, currentTime, x, y);
		m_nodes[i].setLocation(x, y, currentTime);
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

	/**************************** 热点归并过程(merge-HAR) *****************************/
	//merge-HAR: 
	if(DO_MERGE_HAR)
		greedySelection.mergeHotspots(currentTime);

	/********************************** 贪婪选取 *************************************/
	greedySelection.GreedySelect(currentTime);


	/********************************* 后续选取过程 ***********************************/
	CPostSelector postSelector(g_selectedHotspots);
	g_selectedHotspots = postSelector.PostSelect(currentTime);

	
	/***************************** 疏漏节点修复过程(IHAR) ******************************/
	//IHAR: POOR NODE REPAIR
	if(DO_IHAR)
	{
		CNodeRepair repair(g_selectedHotspots, g_hotspotCandidates, currentTime);
		g_selectedHotspots = repair.RepairPoorNodes();
		g_selectedHotspots = postSelector.assignPositionsToHotspots(g_selectedHotspots);
	}

	//比较相邻两次热点选取的相似度
	if(DO_COMP)
	{
		CompareWithOldHotspots();
	}
}

void HAR::HotspotClassification()
{
	vector<CHotspot *> tmp_hotspots = g_selectedHotspots;
	this->m_hotspots = g_selectedHotspots;
	for(vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin(); ihotspot != tmp_hotspots.end(); ihotspot++)
	{
		(*ihotspot)->setHeat( getHotspotHeat(*ihotspot) );
	}
	
	vector<CRoute> newRoutes;
	
	while(! tmp_hotspots.empty())
	{
		//构造一个hotspot class
		CRoute route(m_sink);
		double current_time_cost = 0;
		double current_buffer = 0;
		while(true)
		{
			if( tmp_hotspots.empty() )
				break;
			//循环向class中添加hotspot，直到buffer已满
			double max_ratio = 0;
			int max_front = -1;
			int max_hotspot = -1;
			double time_increment = 0;
			double max_time_increment = 0;
			double sum_generationRate = 0;
			double max_sum_ge = 0;
			double ratio = 0;
			int size_waypoints = route.getNWayPoints();
			//遍历所有剩余hotspot，选择ratio最大的hotspot添加
			for(int ihotspot = 0; ihotspot < tmp_hotspots.size(); ihotspot++)
			{
				double min_length_increment = -1;
				double best_front = -1;
				for(int i = 0; i < size_waypoints; i++)  //先寻找最小路径增量？？
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
				ratio = time_increment * sum_generationRate;  //sum_ge重复计算？？
				if(ratio > max_ratio)
				{
					max_ratio = ratio;
					max_front = best_front;
					max_hotspot = ihotspot;
					max_time_increment = time_increment;
					max_sum_ge = sum_generationRate;
				}
			}
			//检查buffer大小
			double new_buffer = ( current_time_cost + max_time_increment ) * max_sum_ge;
			//如果单个热点Buffer期望过大，就将其单独分配给一个MA
			//FIXME: 或分配多个？
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
	//将得到的新的hotspot class放入sink的route列表
	m_sink->setNewRoutes(newRoutes);
}


void HAR::MANodeRouteDesign()
{
	vector<CRoute> routes = m_sink->getNewRoutes();
	//对每个分类的路线用最近邻居算法进行优化
	for(vector<CRoute>::iterator iroute = routes.begin(); iroute != routes.end(); iroute++)
	{
		OptimizeRoute( *iroute );
	}
	m_routes = routes;
	m_sink->setNewRoutes(routes);

	//通知当前的所有MA路线已过期，立即返回
	for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); iMANode++)
	{
		iMANode->setRouteOverdue(true);
	}

	//将新增的路线分配给新的MA
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

	for(int i = 0; i < NUM_NODE; i++)
	{
		m_nodes.at(i).generateData(currentTime);
	}
}


void HAR::SendData()
{
	//记录waiting time信息
	ofstream waiting_time("waiting-time.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		waiting_time << logInfo;
		waiting_time << "#Time" << TAB << "#HotspotID" << TAB << "#HotspotType/HotspotAge" << TAB 
					 << "#Cover" << TAB << "#Heat" << TAB << "#WaitingTime" << endl;
	}
	waiting_time.close();	

	//更新所有MA的位置
	for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); )
	{

		//重置flag为true
		iMANode->setFlag(true);
		do
		{
			iMANode->updateLocation(currentTime);
			//如果到达sink，投递MA的所有数据
			if( CBase::getDistance( *iMANode, *m_sink) <= TRANS_RANGE )
			{
				if(iMANode->getBufferSize() > 0)
				{
					cout << "####  [ MA " << iMANode->getID() << " Sends " << iMANode->getBufferSize() << " Data ]" << endl; 
					m_sink->receiveData(iMANode->sendAllData(), currentTime);
				}
				if( iMANode->routeIsOverdue() )
				{
					//取得新路线
					if( m_sink->hasMoreNewRoutes() )
					{
						iMANode->setRoute(m_sink->popRoute());
						cout << "####  [ MA " << iMANode->getID() << " Updates Its Route ]" << endl;
					}
					//FIXME: 若路线数目变少，删除多余的MA
					else
					{
						iMANode->setFlag(false);
						cout << "####  [ MA " << iMANode->getID() << " Is Removed ]" << endl;
						break;
					}
				}
			}
			//如果到达hotspot，waitingTime尚未获取
			if(   iMANode->isAtHotspot() 
					&& iMANode->getWaitingTime() < 0 )
			{
				ofstream waiting_time("waiting-time.txt", ios::app);

				CHotspot *atHotspot = iMANode->getAtHotspot();
				int tmp = ROUND( getWaitingTime(atHotspot) );
				//FIXME: 如果不允许Buffer溢出，Buffer已满时即直接跳过waiting
				if( (! INFINITE_BUFFER) && (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
					iMANode->setWaitingTime( 0 );
				else
				{
					iMANode->setWaitingTime( tmp );
					cout << "####  [ MA " << iMANode->getID() << " Waits For " << tmp << " ]" << endl;
					if(tmp == 0)
						iMANode->setAtHotspot(NULL);
				}

				//记录waiting time信息
				if( tmp != 0)
				{
					waiting_time << atHotspot->getTime() << TAB << atHotspot->getID() << TAB ;
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
		
		//删除多余的MA
		if( ! iMANode->getFlag() )
		{
			iMANode = m_MANodes.erase(iMANode);
			continue;
		}
		else
			iMANode++;	
	}

	//投递数据
	for(vector<CMANode>::iterator iMANode = m_MANodes.begin(); iMANode != m_MANodes.end(); iMANode++)
	{
		for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
		{
			if( (! BUFFER_OVERFLOW_ALLOWED) && iMANode->isFull() )
				break;
			if(CBase::getDistance( (CBase)*iMANode, (CBase)*inode ) > TRANS_RANGE)
				continue;

			if( iMANode->isAtHotspot() )
				CMANode::encountAtHotspots();
			else
				CMANode::encountOnTheWay();

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
				iMANode->receiveData(currentTime, inode->sendAllData());
			}
			else
			{
				cout << "####  [ MA " << iMANode->getID() << " Receives " << inode->getBufferSize() << " Data ]" << endl;
				iMANode->receiveData(currentTime, inode->sendData( iMANode->getBufferCapacity() ) );
			}
		}
	}

	//更新所有节点的buffer状态记录
	for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
		inode->updateBufferStatus();

	cout << "####  [ Delivery Ratio ]  " << CData::getDeliveryRatio() << endl;
}

void HAR::PrintInfo()
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
			hotspot << "#time" << TAB << "#HotspotCount" << endl;
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
			hotspot_statistics << "#time" << TAB << "#CoverSum" << TAB << "#HotspotCount" << TAB << "#AvgCover" << endl;
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

		//MA节点个数
		ofstream ma("ma.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ma << logInfo;
			ma << "#time" << TAB << "#MACount" << TAB << "#AvgMAWayPointCount" << endl;
		}
		ma << currentTime << TAB << m_routes.size() << TAB << ( (double)m_hotspots.size() / (double)m_routes.size() ) << endl;
		ma.close();

		//用于计算MA节点个数的历史平均值信息
		MA_COST_SUM += m_routes.size();
		MA_COST_COUNT++;
		//用于计算MA路点（热点）平均个数的历史平均值信息
		MA_WAYPOINT_SUM += (double)m_hotspots.size() / (double)m_routes.size();
		MA_WAYPOINT_COUNT++;

		//ED即平均投递延迟的理论值
		ofstream ed("ed.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ed << logInfo;
			ed << "#time" << TAB << "#EstimatedDelay" << endl;
		}
		ed << currentTime << TAB << calculateEDTime() << endl;
		ed.close();

		//平均能耗
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			energy_consumption << logInfo;
			energy_consumption << "#time" << TAB << "#AvgEnergyConsumption" << endl;
		}
		energy_consumption << currentTime << TAB << ( CData::getAverageEnergyConsumption() * 100 ) << endl;
		energy_consumption.close();

		//每个节点buffer状态的历史平均值
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			buffer << logInfo;
			buffer << "#time" << TAB << "#AvgBufferStateInHistoryOfEachNode" << endl;
		}
		buffer << currentTime << TAB;
		for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
			 buffer << inode->getAverageBufferSize() << TAB;
		buffer << endl;
		buffer.close();

		//数据投递率-900（用于debug）
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
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
			delivery_ratio << "#time" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//数据投递延迟
		ofstream delay("delay.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delay << logInfo;
			delay << "#time" << TAB << "#AvgDelay" << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//MA和节点的相遇次数统计信息
		ofstream encounter("encounter.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			encounter << logInfo;
			encounter << "#time" << TAB << "#Encounter" << TAB << "#EncounterAtHotspots" << TAB << "#EncounterOnTheWay" << TAB 
					  << "#EncounterPercentAtHotspots" << TAB << "#EncounterPercentOnTheWay" << endl;
		}
		encounter << currentTime << TAB << CMANode::getEncounter() << TAB << CMANode::getEncounterAtHotspots() << TAB << CMANode::getEncounterOnTheWay() << TAB 
				  << CMANode::getEncounterPercentAtHotspots() << TAB << CMANode::getEncounterPercentOnTheWay() << endl;
		encounter.close();

		//log输出
		cout << "####  [ MA State ]  ";
		for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
			cout << iMA->getBufferSize() << "  " ;
		cout << endl;

		//每个MA的当前buffer状态
		ofstream ma("buffer-ma.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ma << logInfo;
			ma << "#time" << TAB << "#BufferStateOfEachMA" << endl;
		}
		ma << currentTime << TAB;
		for(vector<CMANode>::iterator iMA = m_MANodes.begin(); iMA != m_MANodes.end(); iMA++)
			ma << iMA->getBufferSize() << TAB ;
		ma << endl;
		ma.close();

		//每个节点的当前buffer状态
		ofstream node("buffer-node.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			node << logInfo;
			node << "#time" << TAB << "#BufferStateOfEachNode" << endl;
		}
		node << currentTime << TAB;
		for(vector<CNode>::iterator inode = m_nodes.begin(); inode != m_nodes.end(); inode++)
			node << inode->getBufferSize() << "  " ;
		node << endl;
		node.close();

		//buffer溢出计数
		ofstream overflow("overflow.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			overflow << logInfo;
			overflow << "#time" << TAB << "#OverflowCount" << endl;
		}
		overflow << currentTime << TAB << CData::getOverflowCount() << endl;
		overflow.close();
	}

	//最终debug输出
	if( currentTime == RUNTIME )
	{
		debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << getAverageHotspotCost() << TAB ;
		if(DO_MERGE_HAR)
			debugInfo << getAverageMergePercent() << TAB << getAverageOldPercent() << TAB ;
		debugInfo << getAverageMACost() << TAB << getAverageMAWaypoint() << TAB ;
		if(DO_COMP)
			debugInfo << getAverageSimilarityRatio() << TAB ;
		debugInfo << logInfo.replace(0, 1, "");
	}

}

void HAR::CompareWithOldHotspots()
{
	static double sumSimilarityRatio = 0;
	if( g_oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(g_oldSelectedHotspots, g_selectedHotspots);
	double oldArea = g_oldSelectedHotspots.size() * AREA_SINGE_HOTSPOT - CHotspot::getOverlapArea(g_oldSelectedHotspots);
	double newArea = g_selectedHotspots.size() * AREA_SINGE_HOTSPOT - CHotspot::getOverlapArea(g_selectedHotspots);

	ofstream similarity("similarity.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection + SLOT_HOTSPOT_UPDATE )
	{
		similarity << logInfo;
		similarity << "#time" << TAB << "#Overlap/Old" << TAB << "#Overlap/New" << TAB
				   << "#OverlapArea" << TAB << "#OldArea" << TAB << "#NewArea" << endl;
	}
	similarity << currentTime << TAB << ( overlapArea / oldArea ) << TAB << ( overlapArea / newArea ) << TAB
			   << overlapArea << TAB << oldArea << TAB << newArea << endl;
	similarity.close();

	//用于计算最终选取出的热点的前后相似度的历史平均值信息
	SIMILARITY_RATIO_SUM += overlapArea / oldArea;
	SIMILARITY_RATIO_COUNT++;
}
