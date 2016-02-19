#include "HAR.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "Sink.h"
#include "MANode.h"

vector<CHotspot *> HAR::m_hotspots;
vector<CRoute> HAR::m_routes;
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

extern int currentTime;
extern _HotspotSelect HOTSPOT_SELECT;

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
		sum_generationRate += CNode::getNodeByID( coveredNodes[i] )->getGenerationRate();
	}

	double ratio = 1;

	return ratio * ( CO_HOTSPOT_HEAT_A1 * nCoveredNodes + CO_HOTSPOT_HEAT_A2 * sum_generationRate ) ;
}

double HAR::getWaitingTime(CHotspot *hotspot)
{
	double result = 1;
	int count_trueHotspot = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	vector<int> nCoveredPositionsForNode;

	for(int i = 0; i < coveredNodes.size(); i++)
	{
		int tmp_time = currentTime;
		double tmp;

		//IHAR: Reduce Memory currentTime
		if( HOTSPOT_SELECT == _improved )
		{
			tmp_time = min(currentTime, MAX_MEMORY_TIME);
		}

		nCoveredPositionsForNode.push_back( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) );
		tmp = double( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) ) / double( tmp_time + SLOT_HOTSPOT_UPDATE );

		//merge-HAR: ratio
		tmp *= pow( hotspot->getCoByCandidateType(), hotspot->getAge() );

		if(tmp >= BETA)
		{
			result *= tmp;
			count_trueHotspot++;
		}
	}
	//FIXME: 如果不是true hotspot，waiting time为0
	if(count_trueHotspot == 0)
		return MIN_WAITING_TIME;
	double prob = exp( -1 / hotspot->getHeat() );
	result = prob / result;
	result = pow(result, ( 1 / double( count_trueHotspot ) ) );

	return result + MIN_WAITING_TIME;
}

double HAR::getSumGenerationRate(vector<int> nodes)
{
	double sum = 0;
	for(int i = 0; i < nodes.size(); i++)
	{
		if( ! CNode::ifNodeExists( nodes[i] ) )
			continue;
		sum += CNode::getNodeByID( nodes[i] )->getGenerationRate();
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
		sum += CNode::getNodeByID( nodes_a[i] )->getGenerationRate();
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

	for(vector<CRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); ++iroute)
		sum_length += iroute->getLength();
	avg_length = sum_length / m_hotspots.size() + 1;
	for(vector<CHotspot *>::iterator ihotspot = m_hotspots.begin(); ihotspot != m_hotspots.end(); ++ihotspot)
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
	ED = EM + ( (1 - PROB_DATA_FORWARD) / PROB_DATA_FORWARD ) * EIM + ( double( m_hotspots.size() ) / (2 * m_routes.size()) ) * avg_u;

	return ED;
}


void HAR::OptimizeRoute(CRoute &route)
{
	vector<CBasicEntity *> waypoints = route.getWayPoints();
	CBasicEntity *current = route.getSink();
	CRoute result(current);
	waypoints.erase(waypoints.begin());
	while(! waypoints.empty())
	{
		double min_distance = -1;
		vector<CBasicEntity *>::iterator min_point;
		for(vector<CBasicEntity *>::iterator ipoint = waypoints.begin(); ipoint != waypoints.end(); ++ipoint)
		{
			double distance = CBasicEntity::getDistance(**ipoint, *current);
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

void HAR::HotspotSelection(int currentTime)
{
	if( ! ( currentTime % SLOT_HOTSPOT_UPDATE == 0 
		&& currentTime >= startTimeForHotspotSelection ) )
		return;

	if( TEST_LEARN )
		DecayPositionsWithoutDeliveryCount(currentTime);

	CGreedySelection::CollectNewPositions(currentTime);

	if( currentTime >= startTimeForHotspotSelection )
	{
		cout << "########  < " << currentTime << " >  HOTSPOT SELECTTION" << endl ;

		CGreedySelection::BuildCandidateHotspots(currentTime);

		/**************************** 热点归并过程(merge-HAR) *****************************/
		if( HOTSPOT_SELECT == _merge )
			CGreedySelection::MergeHotspots(currentTime);

		/********************************** 贪婪选取 *************************************/
		CGreedySelection::GreedySelect(currentTime);


		/********************************* 后续选取过程 ***********************************/
		CPostSelector postSelector(CHotspot::selectedHotspots);
		CHotspot::selectedHotspots = postSelector.PostSelect(currentTime);

	
		/***************************** 疏漏节点修复过程(IHAR) ******************************/
		if( HOTSPOT_SELECT == _improved )
		{
			CNodeRepair repair(CHotspot::selectedHotspots, CHotspot::hotspotCandidates, currentTime);
			CHotspot::selectedHotspots = repair.RepairPoorNodes();
			CHotspot::selectedHotspots = postSelector.assignPositionsToHotspots(CHotspot::selectedHotspots);
		}

		flash_cout << "####  [ Hotspot ] " << CHotspot::selectedHotspots.size() << "                           " << endl;

		//比较相邻两次热点选取的相似度
		if(TEST_HOTSPOT_SIMILARITY)
		{
			CompareWithOldHotspots(currentTime);
		}
	}
}

void HAR::HotspotClassification(int currentTime)
{
	vector<CHotspot *> tmp_hotspots = CHotspot::selectedHotspots;
	m_hotspots = CHotspot::selectedHotspots;
	for(vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin(); ihotspot != tmp_hotspots.end(); ++ihotspot)
	{
		(*ihotspot)->setHeat( getHotspotHeat(*ihotspot) );
	}
	
	vector<CRoute> newRoutes;
	
	while(! tmp_hotspots.empty())
	{
		//构造一个hotspot class
		CRoute route(CSink::getSink());
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
				//cout << endl << "Error @ HAR::HotspotClassification() : A single hotspot's buffer expection > BUFFER_CAPACITY_MA"<<endl;
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
				//current_buffer = new_buffer;
				route.AddPoint(max_front, tmp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin() + max_hotspot;
				tmp_hotspots.erase(ihotspot);
			}
		}
		newRoutes.push_back(route);
	}
	//将得到的新的hotspot class放入sink的route列表
	CSink::getSink()->setNewRoutes(newRoutes);
}


void HAR::MANodeRouteDesign(int currentTime)
{
	vector<CRoute> routes = CSink::getSink()->getNewRoutes();
	//对每个分类的路线用最近邻居算法进行优化
	for(vector<CRoute>::iterator iroute = routes.begin(); iroute != routes.end(); ++iroute)
	{
		OptimizeRoute( *iroute );
	}
	m_routes = routes;
	CSink::getSink()->setNewRoutes(routes);

	//通知当前的所有MA路线已过期，立即返回
	for(auto iMANode = CMANode::getMANodes().begin(); iMANode != CMANode::getMANodes().end(); ++iMANode)
	{
		(*iMANode)->setRouteOverdue(true);
	}

	//将新增的路线分配给新的MA
	if(CMANode::getMANodes().size() < routes.size())
	{
		int num_newMANodes = routes.size() - CMANode::getMANodes().size();
		for(int i = 0; i < num_newMANodes; i++)
		{
			CMANode::newMANode(CSink::getSink()->popRoute(), currentTime);
		}
	}

	for(vector<CRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); ++iroute)
		cout << "####  [ MA ]  " << routes.size() << endl;
}


void HAR::SendData(int currentTime)
{
	if( ! ( currentTime % SLOT_DATA_SEND == 0 ) )
		return;
	cout << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;

	//记录waiting time信息
	ofstream waiting_time("waiting-time.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		waiting_time << INFO_LOG;
		waiting_time << "#Time" << TAB << "#MANodeID" << TAB << "#HotspotID" << TAB << "#HotspotType/HotspotAge" << TAB 
					 << "#Cover" << TAB << "#Heat" << TAB << "#WaitingTime" << endl;
	}
	waiting_time.close();
	ofstream delivery_hotspot("delivery-hotspot.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		delivery_hotspot << endl << INFO_LOG;
		delivery_hotspot << "#Time" << TAB << "#DeliveryCountForSingleHotspotInThisSlot ..." << endl;
	}
	//用于存储hotspot及其投递计数的静态拷贝
	//由于输出相关信息时（900s时的热点将在2700s时被输出）该热点的全局唯一指针已经被释放，所以存储的是该热点的浅拷贝，不能用于其他用途，否则将影响热点记录的唯一性
	static vector<CHotspot> deliveryCounts;  
	ofstream delivery_statistics("delivery-statistics.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		delivery_statistics << endl << INFO_LOG;
		delivery_statistics << "#Time" << TAB << "#DeliveryAtHotspotCount" << TAB << "#DeliveryTotalCount" << TAB << "#DeliveryAtHotspotPercent" << endl;
	}
	//用于测试投递计数为0的热点信息，按照投递计数降序输出所有热点的覆盖的position数、node数、ratio、投递计数
	ofstream hotspot_rank("hotspot-rank.txt", ios::app);
	if( currentTime == startTimeForHotspotSelection )
	{
		hotspot_rank << endl << INFO_LOG;
		hotspot_rank << "#WorkTime" << TAB << "#ID" << TAB << "#Location" << TAB << "#nPosition, nNode" << TAB << "#Ratio" << TAB << "#Tw" << TAB << "#DeliveryCount" << endl;
	}
	//用于统计过期热点的投递计数时判断是否应当输出时间
	static bool hasMoreNewRoutes = false;

	//更新所有MA的位置
	for(auto iMANode = CMANode::getMANodes().begin(); iMANode != CMANode::getMANodes().end(); )
	{

		//重置flag为true
		(*iMANode)->setFlag(true);
		do
		{
			(*iMANode)->updateLocation(currentTime);
			//如果到达sink，投递MA的所有数据
			if( CBasicEntity::getDistance( **iMANode, *CSink::getSink()) <= TRANS_RANGE )
			{
				if((*iMANode)->getBufferSize() > 0)
				{
					flash_cout << "####  ( MA " << (*iMANode)->getID() << " deliver " << (*iMANode)->getBufferSize() << " data to Sink )               " ; 
					CSink::getSink()->receiveData(currentTime, (*iMANode)->sendAllData(SEND::DUMP));
				}
				if( (*iMANode)->routeIsOverdue() )
				{
					//保留过期路径，用于稍后统计旧热点的投递计数信息
					vector<CBasicEntity *> overdueHotspots = (*iMANode)->getRoute()->getWayPoints();

					//更新路线，取得新的热点集合
					if( CSink::getSink()->hasMoreNewRoutes() )
					{
						//热点集合发生更新，输出已完成统计的热点投递计数集合，和上一轮热点选取的时间
						if( ! hasMoreNewRoutes )
						{
							if( ! deliveryCounts.empty() )
							{
								//按照投递计数降序排列，2700s时输出900s选出的热点在(900, 1800)期间的投递计数，传入的参数应为1800
								int endTime =  currentTime - SLOT_HOTSPOT_UPDATE;
								deliveryCounts = CPreprocessor::mergeSortByDeliveryCount(deliveryCounts, (endTime) );
								for(vector<CHotspot>::iterator ihotspot = deliveryCounts.begin(); ihotspot != deliveryCounts.end(); ++ihotspot)
								{
									delivery_hotspot << ihotspot->getDeliveryCount( currentTime - SLOT_HOTSPOT_UPDATE ) << TAB ;
									hotspot_rank << ihotspot->getTime() << "-" << currentTime - SLOT_HOTSPOT_UPDATE << TAB << ihotspot->getID() << TAB << ihotspot->getX() << "," << ihotspot->getY() << TAB << ihotspot->getNCoveredPosition() << "," << ihotspot->getNCoveredNodes() << TAB 
										<< ihotspot->getRatio() << TAB << ihotspot->getWaitingTime(endTime) << TAB << ihotspot->getDeliveryCount(endTime) << endl;

								}
								delivery_hotspot << endl;
								deliveryCounts.clear();
							}
							delivery_hotspot << currentTime - SLOT_HOTSPOT_UPDATE << TAB ;
							delivery_statistics << currentTime - SLOT_HOTSPOT_UPDATE << TAB << CData::getDeliveryAtHotspotCount() << TAB 
								<< CData::getDeliveryTotalCount() << TAB << CData::getDeliveryAtHotspotPercent() << endl;
							delivery_statistics.close();
							hasMoreNewRoutes = true;
						}

						(*iMANode)->updateRoute(CSink::getSink()->popRoute());
						flash_cout << "####  ( MA " << (*iMANode)->getID() << " update route )               " ;
					}
					//FIXME: 若路线数目变少，删除多余的MA
					else
					{
						(*iMANode)->setFlag(false);
					}

					//统计旧热点的投递计数信息
					for(vector<CBasicEntity *>::iterator iHotspot = overdueHotspots.begin(); iHotspot != overdueHotspots.end(); ++iHotspot)
					{
						if( (*iHotspot)->getID() == SINK_ID )
							continue;
						CHotspot *hotspot = static_cast<CHotspot *>(*iHotspot);
						deliveryCounts.push_back( *hotspot );
					}
					if( ! (*iMANode)->getFlag() )
						break;
				}
				else if( ! CSink::getSink()->hasMoreNewRoutes() )
				{
					//热点集合更新结束，重置flag
					hasMoreNewRoutes = false;
				}
			}
			//如果到达hotspot，waitingTime尚未获取
			if(   (*iMANode)->isAtHotspot() 
					&& (*iMANode)->getWaitingTime() < 0 )
			{
				ofstream waiting_time("waiting-time.txt", ios::app);

				CHotspot *atHotspot = (*iMANode)->getAtHotspot();
				int tmp = ROUND( getWaitingTime(atHotspot) );
				//FIXME: 如果不允许Buffer溢出，Buffer已满时即直接跳过waiting
				if( ( CMANode::BUFFER_MODE == BUFFER::SELFISH ) && (*iMANode)->isFull() )
					(*iMANode)->setWaitingTime( 0 );
				else
				{
					(*iMANode)->setWaitingTime( tmp );
					atHotspot->addWaitingTime( tmp );
					flash_cout << "####  ( MA " << (*iMANode)->getID() << " wait for " << tmp << " )                               " ;
					//if(tmp == 0)
					//	(*iMANode)->setAtHotspot(nullptr);
				}

				//记录waiting time信息
				if( tmp > 0)
				{
					waiting_time << atHotspot->getTime() << TAB << (*iMANode)->getID() << TAB << atHotspot->getID() << TAB ;
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
		}while((*iMANode)->getTime() < currentTime);
		
		//删除多余的MA
		if( ! (*iMANode)->getFlag() )
		{
			(*iMANode)->turnFree();
			continue;
		}
		else
			++iMANode;	
	}

	//投递数据
	for(auto iMANode = CMANode::getMANodes().begin(); iMANode != CMANode::getMANodes().end(); ++iMANode)
	{
		for(auto inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		{
			if( ( CMANode::BUFFER_MODE == BUFFER::SELFISH ) && (*iMANode)->isFull() )
				break;
			if(CBasicEntity::getDistance( static_cast<CBasicEntity>(**iMANode), static_cast<CBasicEntity>(**inode) ) > TRANS_RANGE)
				continue;

			//对于热点上和路径上分别统计相遇次数
			if( (*iMANode)->isAtHotspot() )
				CNode::encountAtHotspot();
			else
				CNode::encountOnRoute();

			if( ! (*inode)->hasData() )
				continue;

			vector<CData> data;
			int capacity = (*iMANode)->getBufferCapacity();
			if( capacity > 0 )
			{
				data = (*inode)->sendData(capacity);
				(*iMANode)->receiveData(currentTime, data);			
			}
			flash_cout << "####  ( Node " << (*inode)->getID() << " send " << data.size() << " data to MA " << (*iMANode)->getID() << " )                    " ;

			//对于热点上和路径上分别统计数据投递计数
			if( (*iMANode)->isAtHotspot() )
			{
				CData::deliverAtHotspot( data.size() );
				(*iMANode)->getAtHotspot()->addDeliveryCount( data.size() );
			}
			else
				CData::deliverOnRoute( data.size() );

		}
	}

	//更新所有节点的buffer状态记录
	for(auto inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		(*inode)->recordBufferStatus();

	//控制台输出时保留一位小数
	double deliveryRatio = 0;
	if( CData::getDataArrivalCount() > 0 )
		deliveryRatio = CData::getDataArrivalCount() / double(CData::getDataCount()) * 1000;
	deliveryRatio = ROUND( deliveryRatio );
	deliveryRatio = deliveryRatio / double( 10 );
	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl << endl;
	delivery_hotspot.close();
}

string INFO_HOTSPOT = "#Time	#HotspotCount \n" ;
string INFO_AT_HOTSPOT = "#Time	#VisitAtHotspot	#VisitSum	#VisitAtHotspotPercent \n" ;
string INFO_HOTSPOT_STATISTICS = "#Time	#CoverSum	#HotspotCount	#AvgCover \n" ;
string INFO_MERGE = "#Time	#MergeHotspotCount	#MergeHotspotPercent	#OldHotspotCount	#OldHotspotPercent	#NewHotspotCount	#NewHotspotPercent \n" ;
string INFO_MERGE_DETAILS = "#Time	#HotspotType/#MergeAge ... \n" ;
string INFO_MA = "#Time	#MACount	#AvgMAWayPointCount \n" ;
string INFO_ED = "#Time	#EstimatedDelay \n" ;
string INFO_BUFFER_MA = "#Time	#BufferStateOfEachMA \n" ;

void HAR::PrintInfo(int currentTime)
{
	if( MAC_PROTOCOL != _hdc && ROUTING_PROTOCOL != _har )
		return ;

	if( ! ( ( currentTime % SLOT_RECORD_INFO == 0 
		      && currentTime >= startTimeForHotspotSelection )
			|| currentTime == RUNTIME  ) )
		return;

	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//热点个数
		ofstream hotspot("hotspot.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot << INFO_LOG ;
			hotspot << INFO_HOTSPOT ;
		}
		hotspot << currentTime << TAB << CHotspot::selectedHotspots.size() << endl; 
		hotspot.close();

		//节点在热点内的百分比（从热点选取开始时开始统计）
		ofstream at_hotspot("at-hotspot.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			at_hotspot << INFO_LOG ; 
			at_hotspot << INFO_AT_HOTSPOT ;
		}
		at_hotspot << currentTime << TAB << CNode::getVisiterAtHotspot() << TAB << CNode::getVisiter() << TAB << CNode::getVisiterAtHotspotPercent() << endl;
		at_hotspot.close();
	}

	if( ROUTING_PROTOCOL != _har )
		return ;

	CRoutingProtocol::PrintInfo(currentTime);


	//hotspot选取结果、hotspot class数目、ED、Energy Consumption、MA节点buffer状态 ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//用于计算热点个数历史平均值
		HOTSPOT_COST_SUM += m_hotspots.size();
		HOTSPOT_COST_COUNT++;

		//热点质量统计信息
		ofstream hotspot_statistics("hotspot-statistics.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			hotspot_statistics << INFO_LOG ;
			hotspot_statistics << INFO_HOTSPOT_STATISTICS ;
		}
		int sumCover = 0;
		for(vector<CHotspot *>::iterator it = m_hotspots.begin(); it != m_hotspots.end(); ++it)
			sumCover += (*it)->getNCoveredPosition();
		hotspot_statistics << currentTime << TAB << sumCover << TAB << m_hotspots.size() << TAB << double( sumCover ) / double( m_hotspots.size() ) << endl;
		hotspot_statistics.close();

		if( HOTSPOT_SELECT == _merge )
		{
			//热点归并过程统计信息（在最终选取出的热点集合中）
			if( HOTSPOT_SELECT == _merge )
			{
				int mergeCount = 0;
				int oldCount = 0;
				int newCount = 0;
				ofstream merge("merge.txt", ios::app);
				ofstream merge_details("merge-details.txt", ios::app);

				if(currentTime == startTimeForHotspotSelection)
				{
					merge << INFO_LOG ;
					merge << INFO_MERGE ;
					merge_details << INFO_LOG ;
					merge_details << INFO_MERGE_DETAILS ;
				}
				merge_details << currentTime << TAB;

				//热点类型及年龄统计信息
				for(vector<CHotspot *>::iterator ihotspot = m_hotspots.begin(); ihotspot != m_hotspots.end(); ++ihotspot)
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
				merge << currentTime << TAB << mergeCount << TAB << double( mergeCount ) / double( total ) << TAB << oldCount << TAB 
					  << double( oldCount ) / double( total ) << TAB << newCount << TAB << double( newCount ) / double( total ) << endl;

				//用于计算归并热点和旧热点所占比例的历史平均值信息
				MERGE_PERCENT_SUM += double( mergeCount ) / double( total );
				MERGE_PERCENT_COUNT++;
				OLD_PERCENT_SUM += double( oldCount ) / double( total );
				OLD_PERCENT_COUNT++;

				merge.close();
				merge_details.close();
			}
		}

		//MA节点个数
		ofstream ma("ma.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ma << INFO_LOG ;
			ma << INFO_MA ;
		}
		ma << currentTime << TAB << m_routes.size() << TAB << ( double( m_hotspots.size() ) / double( m_routes.size() ) ) << endl;
		ma.close();

		//用于计算MA节点个数的历史平均值信息
		MA_COST_SUM += m_routes.size();
		MA_COST_COUNT++;
		//用于计算MA路点（热点）平均个数的历史平均值信息
		MA_WAYPOINT_SUM += double( m_hotspots.size() ) / double( m_routes.size() );
		MA_WAYPOINT_COUNT++;

		//ED即平均投递延迟的理论值
		ofstream ed("ed.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			ed << INFO_LOG ;
			ed << INFO_ED ;
		}
		ed << currentTime << TAB << calculateEDTime() << endl;
		ed.close();

	}

	//Buffer
	if( currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME )
	{
		//每个MA的当前buffer状态
		ofstream buffer_ma("buffer-ma.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			buffer_ma << INFO_LOG ;
			buffer_ma << INFO_BUFFER_MA ;
		}
		buffer_ma << currentTime << TAB;
		for(auto iMA = CMANode::getMANodes().begin(); iMA != CMANode::getMANodes().end(); ++iMA)
			buffer_ma << (*iMA)->getBufferSize() << TAB ;
		buffer_ma << endl;
		buffer_ma.close();

	}

	//最终debug输出（补充）
	if( currentTime == RUNTIME )
	{
		if( HOTSPOT_SELECT == _merge )
			debugInfo << getAverageMergePercent() << TAB << getAverageOldPercent() << TAB ;
		debugInfo << getAverageMACost() << TAB ;
		if( TEST_HOTSPOT_SIMILARITY )
			debugInfo << getAverageSimilarityRatio() << TAB ;
		debugInfo << CData::getDeliveryAtHotspotPercent() << TAB ;
		debugInfo.flush();
	}
}

void HAR::CompareWithOldHotspots(int currentTime)
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

	//用于计算最终选取出的热点的前后相似度的历史平均值信息
	SIMILARITY_RATIO_SUM += overlapArea / oldArea;
	SIMILARITY_RATIO_COUNT++;
}

void HAR::DecayPositionsWithoutDeliveryCount(int currentTime)
{
	if(currentTime == 0)
		return ;
	vector<CPosition*> badPositions;
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	for(vector<CHotspot*>::iterator ihotspot = CHotspot::oldSelectedHotspots.begin(); ihotspot != CHotspot::oldSelectedHotspots.end(); )
	{
		if( (*ihotspot)->getDeliveryCount(currentTime) == 0 )
		{
			addToListUniquely( badPositions, (*ihotspot)->getCoveredPositions() );
			//free(*ihotspot);
			//在mHAR中，应该考虑是否将这些热点排除在归并之外
			//CHotspot::deletedHotspots.push_back(*ihotspot);
			//ihotspot = CHotspot::oldSelectedHotspots.erase(ihotspot);
			++ihotspot;
		}
		else
			++ihotspot;
	}
	for(vector<CPosition*>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
	{
		if( ifExists(badPositions, *ipos) )
		{
			(*ipos)->decayWeight();
			//Reduce complexity
			RemoveFromList(badPositions, *ipos);
			//如果权值低于最小值，直接删除，MIN_POSITION_WEIGHT默认值为1，即不会删除任何position
			if( (*ipos)->getWeight() < MIN_POSITION_WEIGHT )
			{
				CPosition::deletedPositions.push_back(*ipos);
				ipos = CPosition::positions.erase(ipos);
			}
			else
				++ipos;
		}
		else
			++ipos;
	}
}

