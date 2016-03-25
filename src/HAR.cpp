#include "Global.h"
#include "HAR.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "NodeRepair.h"
#include "Sink.h"
#include "MANode.h"
#include "Node.h"
#include "HDC.h"
#include "SMac.h"

vector<CHotspot *> HAR::m_hotspots;
vector<CRoute> HAR::m_routes;
int HAR::SUM_HOTSPOT_COST = 0;
int HAR::COUNT_HOTSPOT_COST = 0;
int HAR::SUM_MA_COST = 0;
int HAR::COUNT_MA_COST = 0;
double HAR::SUM_WAYPOINT_PER_MA = 0;
int HAR::COUNT_WAYPOINT_PER_MA = 0;
double HAR::SUM_PERCENT_MERGE = 0;
int HAR::COUNT_PERCENT_MERGE = 0;
double HAR::SUM_PERCENT_OLD = 0;
int HAR::COUNT_PERCENT_OLD = 0;
double HAR::SUM_SIMILARITY_RATIO = 0;
int HAR::COUNT_SIMILARITY_RATIO = 0;

double HAR::BETA = 0.0025;  //ratio for true hotspot
//double HAR::GAMMA = 0.5;  //ratio for HotspotsAboveAverage
double HAR::CO_HOTSPOT_HEAT_A1 = 1;
double HAR::CO_HOTSPOT_HEAT_A2 = 30;

/************************************ IHAR ************************************/

double HAR::LAMBDA = 0;
int HAR::LIFETIME_POSITION = 3600;

/********************************* merge-HAR ***********************************/

int HAR::MIN_WAITING_TIME = 0;  //add minimum waiting time to each hotspot
bool HAR::TEST_BALANCED_RATIO = false;
//bool HAR::TEST_LEARN = false;
//double HAR::MIN_POSITION_WEIGHT = 0;


double HAR::getHotspotHeat(CHotspot *hotspot)
{
	int nCoveredNodes = hotspot->getNCoveredNodes();
	double sum_generationRate = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	for(int i = 0; i < coveredNodes.size(); ++i)
	{
		if( ! CNode::ifNodeExists( coveredNodes[i] ) )
		{
			nCoveredNodes--;
			continue;
		}
		sum_generationRate += CNode::getNodeByID( coveredNodes[i] )->getDataRate();
	}

	double ratio = 1;

	return ratio * ( CO_HOTSPOT_HEAT_A1 * nCoveredNodes + CO_HOTSPOT_HEAT_A2 * sum_generationRate ) ;
}

double HAR::getWaitingTime(int currentTime, CHotspot *hotspot)
{
	double result = 1;
	int count_trueHotspot = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	vector<int> nCoveredPositionsForNode;

	for(int i = 0; i < coveredNodes.size(); ++i)
	{
		int temp_time = currentTime;
		double temp;

		//IHAR: Reduce Memory currentTime
		if( HOTSPOT_SELECT == _improved )
		{
			temp_time = min(currentTime, LIFETIME_POSITION);
		}

		nCoveredPositionsForNode.push_back( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) );
		temp = double( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) ) / double( temp_time + CHotspot::SLOT_HOTSPOT_UPDATE );

		//merge-HAR: ratio
		temp *= pow( hotspot->getRatioByTypeHotspotCandidate(), hotspot->getAge() );

		if(temp >= BETA)
		{
			result *= temp;
			++count_trueHotspot;
		}
	}
	//FIXME: �������true hotspot��waiting timeΪ0
	if(count_trueHotspot == 0)
		return MIN_WAITING_TIME;
	double prob = exp( -1 / hotspot->getHeat() );
	result = prob / result;
	result = pow(result, ( 1 / double( count_trueHotspot ) ) );

	return result + MIN_WAITING_TIME;
}

double HAR::getSumDataRate(vector<int> nodes)
{
	double sum = 0;
	for(int i = 0; i < nodes.size(); ++i)
	{
		if( ! CNode::ifNodeExists( nodes[i] ) )
			continue;
		sum += CNode::getNodeByID( nodes[i] )->getDataRate();
	}
	return sum;
}

double HAR::getTimeIncreForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot)
{
	double result = getWaitingTime(currentTime, hotspot) + ( route.getIncreDistance(front, hotspot) / CMANode::getSpeed() );
	return result;
}

double HAR::calculateRatioForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot)
{
	double time_incr = getTimeIncreForInsertion(currentTime, route, front, hotspot);
	vector<int> temp_nodes = route.getCoveredNodes();
	AddToListUniquely(temp_nodes, hotspot->getCoveredNodes());

	double sumGenerationRate = getSumDataRate(temp_nodes);
	return ( time_incr * sumGenerationRate );
}

double HAR::calculateEDTime(int currentTime)
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
		sum_waitingTime += getWaitingTime(currentTime, *ihotspot);
		sum_pm += exp( -1 / (*ihotspot)->getHeat() );
	}
	avg_waitingTime = sum_waitingTime / m_hotspots.size();
	avg_u = avg_length / CMANode::getSpeed() + avg_waitingTime;
	avg_pw = sum_pm / m_hotspots.size();
	pmh = sum_waitingTime / (sum_length / CMANode::getSpeed() + sum_waitingTime);
	EM = (1 - pmh) * ( ( (1 - avg_pw) / avg_pw) * avg_u + (avg_length / (2 * CMANode::getSpeed()) + avg_waitingTime) ) + pmh * ( ( (1 - avg_pw) / avg_pw) * avg_u + avg_waitingTime);
	EIM = avg_u / avg_pw;
	ED = EM + ( (1 - CGeneralNode::PROB_TRANS) / CGeneralNode::PROB_TRANS ) * EIM + ( double( m_hotspots.size() ) / (2 * m_routes.size()) ) * avg_u;

	return ED;
}


void HAR::OptimizeRoute(CRoute &route)
{
	vector<CBasicEntity *> waypoints = route.getWayPoints();
	CBasicEntity *current = CSink::getSink();
	CRoute result( CSink::getSink() );
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
		result.AddHotspot(*min_point);
		current = *min_point;
		waypoints.erase(min_point);
	}
	result.updateLength();
	route = result;
}

void HAR::HotspotSelection(int currentTime)
{
	if( ! ( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE == 0 
		&& currentTime >= CHotspot::TIME_HOSPOT_SELECT_START ) )
		return;

//	if( TEST_LEARN )
//		DecayPositionsWithoutDeliveryCount(currentTime);

	CHotspotSelect::CollectNewPositions(currentTime);

	if( currentTime >= CHotspot::TIME_HOSPOT_SELECT_START )
	{
		flash_cout << "########  < " << currentTime << " >  HOTSPOT SELECT            " << endl ;

		CHotspotSelect::BuildCandidateHotspots(currentTime);

		/**************************** �ȵ�鲢����(merge-HAR) *****************************/
		if( HOTSPOT_SELECT == _merge )
			CHotspotSelect::MergeHotspots(currentTime);

		/********************************** ̰��ѡȡ *************************************/
		CHotspotSelect::GreedySelect(currentTime);


		/********************************* ����ѡȡ���� ***********************************/
		CPostSelect postSelector(CHotspot::selectedHotspots);
		CHotspot::selectedHotspots = postSelector.PostSelect(currentTime);

	
		/***************************** ��©�ڵ��޸�����(IHAR) ******************************/
		if( HOTSPOT_SELECT == _improved )
		{
			CNodeRepair repair(CHotspot::selectedHotspots, CHotspot::hotspotCandidates, currentTime);
			CHotspot::selectedHotspots = repair.RepairPoorNodes();
			CHotspot::selectedHotspots = postSelector.assignPositionsToHotspots(CHotspot::selectedHotspots);
		}

		flash_cout << "####  [ Hotspot ] " << CHotspot::selectedHotspots.size() << "                           " << endl;

		//�Ƚ����������ȵ�ѡȡ�����ƶ�
		if(TEST_HOTSPOT_SIMILARITY)
		{
			CompareWithOldHotspots(currentTime);
		}
	}
}

void HAR::HotspotClassification(int currentTime)
{
	vector<CHotspot *> temp_hotspots = CHotspot::selectedHotspots;
	m_hotspots = CHotspot::selectedHotspots;
	for(vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin(); ihotspot != temp_hotspots.end(); ++ihotspot)
	{
		(*ihotspot)->setHeat( getHotspotHeat(*ihotspot) );
	}
	
	vector<CRoute> newRoutes;
	
	while(! temp_hotspots.empty())
	{
		//����һ��hotspot class
		CRoute route(CSink::getSink());
		double current_time_cost = 0;
		while(true)
		{
			if( temp_hotspots.empty() )
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
			for(int ihotspot = 0; ihotspot < temp_hotspots.size(); ++ihotspot)
			{
				double min_length_increment = -1;
				double best_front = -1;
				for(int i = 0; i < size_waypoints; ++i)  //��Ѱ����С·����������
				{
					double length_increment = route.getIncreDistance(i, temp_hotspots[ihotspot]);
					if(length_increment < min_length_increment
						|| min_length_increment < 0)
					{
						min_length_increment = length_increment;
						best_front = i;
					}
				}

				time_increment = getTimeIncreForInsertion(currentTime, route, best_front, temp_hotspots[ihotspot]);
				vector<int> temp_nodes = route.getCoveredNodes();
				AddToListUniquely(temp_nodes, temp_hotspots[ihotspot]->getCoveredNodes());
				sum_generationRate = getSumDataRate(temp_nodes);
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
			if( new_buffer > CMANode::getCapacityBuffer()
				&& route.getNWayPoints() == 1)
			{
				//cout << endl << "Error @ HAR::HotspotClassification() : A single hotspot's buffer expection > BUFFER_CAPACITY_MA"<<endl;
				//_PAUSE_;
				route.AddHotspot(max_front, temp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin() + max_hotspot;
				temp_hotspots.erase(ihotspot);
				break;
			}
			if( new_buffer > CMANode::getCapacityBuffer() )
				break;
			else
			{
				current_time_cost += max_time_increment;
				//current_buffer = new_buffer;
				route.AddHotspot(max_front, temp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin() + max_hotspot;
				temp_hotspots.erase(ihotspot);
			}
		}
		newRoutes.push_back(route);
	}
	//���õ����µ�hotspot class����sink��route�б�
	CSink::getSink()->setNewRoutes(newRoutes);
}


void HAR::MANodeRouteDesign(int currentTime)
{
	vector<CRoute> routes = CSink::getSink()->getNewRoutes();
	//��ÿ�������·��������ھ��㷨�����Ż�
	for(vector<CRoute>::iterator iroute = routes.begin(); iroute != routes.end(); ++iroute)
	{
		OptimizeRoute( *iroute );
	}
	m_routes = routes;
	CSink::getSink()->setNewRoutes(routes);

	//֪ͨ��ǰ������MA·���ѹ��ڣ���������
	for(auto iMANode = CMANode::getMANodes().begin(); iMANode != CMANode::getMANodes().end(); ++iMANode)
	{
		(*iMANode)->setRouteOverdue(true);
	}

	//��������·�߷�����µ�MA
	if(CMANode::getMANodes().size() < routes.size())
	{
		int num_newMANodes = routes.size() - CMANode::getMANodes().size();
		for(int i = 0; i < num_newMANodes; ++i)
		{
			CMANode::newMANode(CSink::getSink()->popRoute(), currentTime);
		}
	}

	for(vector<CRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); ++iroute)
		cout << "####  [ MA ]  " << routes.size() << endl;
}


//void HAR::SendData(int currentTime)
//{
//	if( ! ( currentTime % SLOT_DATA_SEND == 0 ) )
//		return;
//	cout << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;
//
//	//��¼waiting time��Ϣ
//	ofstream waiting_time("waiting-time.log", ios::app);
//	if( currentTime == CHotspot::TIME_HOSPOT_SELECT_START )
//	{
//		waiting_time << endl << INFO_LOG << endl ;
//		waiting_time << "#Time" << TAB << "#MANodeID" << TAB << "#HotspotID" << TAB << "#HotspotType/HotspotAge" << TAB 
//					 << "#Cover" << TAB << "#Heat" << TAB << "#WaitingTime" << endl;
//	}
//	waiting_time.close();
//	ofstream delivery_hotspot( PATH_ROOT + PATH_LOG + FILE_DELIVERY_HOTSPOT, ios::app);
//	if( currentTime == CHotspot::TIME_HOSPOT_SELECT_START )
//	{
//		delivery_hotspot << endl << INFO_LOG << endl ;
//		delivery_hotspot << INFO_DELIVERY_HOTSPOT ;
//	}
//	//���ڴ洢hotspot����Ͷ�ݼ����ľ�̬����
//	//������������Ϣʱ��900sʱ���ȵ㽫��2700sʱ����������ȵ��ȫ��Ψһָ���Ѿ����ͷţ����Դ洢���Ǹ��ȵ��ǳ��������������������;������Ӱ���ȵ��¼��Ψһ��
//	static vector<CHotspot> countsDelivery;  
//	ofstream delivery_statistics( PATH_ROOT + PATH_LOG + FILE_DELIVERY_STATISTICS, ios::app);
//	if( currentTime == CHotspot::TIME_HOSPOT_SELECT_START )
//	{
//		delivery_statistics << endl << INFO_LOG << endl ;
//		delivery_statistics << INFO_DELIVERY_STATISTICS ;
//	}
//	//���ڲ���Ͷ�ݼ���Ϊ0���ȵ���Ϣ������Ͷ�ݼ���������������ȵ�ĸ��ǵ�position����node����ratio��Ͷ�ݼ���
//	ofstream hotspot_rank( PATH_ROOT + PATH_LOG + FILE_HOTSPOT_RANK, ios::app);
//	if( currentTime == CHotspot::TIME_HOSPOT_SELECT_START )
//	{
//		hotspot_rank << endl << INFO_LOG << endl ;
//		hotspot_rank << INFO_HOTSPOT_RANK ;
//	}
//	//����ͳ�ƹ����ȵ��Ͷ�ݼ���ʱ�ж��Ƿ�Ӧ�����ʱ��
//	static bool hasMoreNewRoutes = false;
//
//	//��������MA��λ��
//	for(auto iMANode = CMANode::getMANodes().begin(); iMANode != CMANode::getMANodes().end(); )
//	{
//
//		//����flagΪtrue
//		(*iMANode)->setFlag(true);
//		do
//		{
//			(*iMANode)->updateLocation(currentTime);
//			//�������sink��Ͷ��MA����������
//			if( CBasicEntity::withinRange( **iMANode, *CSink::getSink(), CGeneralNode::RANGE_TRANS ) )
//			{
//				if((*iMANode)->getSizeBuffer() > 0)
//				{
//					flash_cout << "####  ( MA " << (*iMANode)->getID() << " deliver " << (*iMANode)->getSizeBuffer() << " data to Sink )               " ; 
//					CSink::getSink()->receiveData(currentTime, (*iMANode)->sendAllData(CGeneralNode::_dump));
//				}
//				if( (*iMANode)->routeIsOverdue() )
//				{
//					//��������·���������Ժ�ͳ�ƾ��ȵ��Ͷ�ݼ�����Ϣ
//					vector<CBasicEntity *> overdueHotspots = (*iMANode)->getRoute()->getWayPoints();
//
//					//����·�ߣ�ȡ���µ��ȵ㼯��
//					if( CSink::getSink()->hasMoreNewRoutes() )
//					{
//						//�ȵ㼯�Ϸ������£���������ͳ�Ƶ��ȵ�Ͷ�ݼ������ϣ�����һ���ȵ�ѡȡ��ʱ��
//						if( ! hasMoreNewRoutes )
//						{
//							if( ! countsDelivery.empty() )
//							{
//								//����Ͷ�ݼ����������У�2700sʱ���900sѡ�����ȵ���(900, 1800)�ڼ��Ͷ�ݼ���������Ĳ���ӦΪ1800
//								int endTime =  currentTime - CHotspot::SLOT_HOTSPOT_UPDATE;
//								countsDelivery = CSortHelper::mergeSortByDeliveryCount(countsDelivery, (endTime) );
//								for(vector<CHotspot>::iterator ihotspot = countsDelivery.begin(); ihotspot != countsDelivery.end(); ++ihotspot)
//								{
//									delivery_hotspot << ihotspot->getCountDelivery( currentTime - CHotspot::SLOT_HOTSPOT_UPDATE ) << TAB ;
//									hotspot_rank << ihotspot->getTime() << "-" << currentTime - CHotspot::SLOT_HOTSPOT_UPDATE << TAB << ihotspot->getID() << TAB << ihotspot->getX() << "," << ihotspot->getY() << TAB << ihotspot->getNCoveredPosition() << "," << ihotspot->getNCoveredNodes() << TAB 
//										<< ihotspot->getRatio() << TAB << ihotspot->getWaitingTime(endTime) << TAB << ihotspot->getCountDelivery(endTime) << endl;
//
//								}
//								delivery_hotspot << endl;
//								countsDelivery.clear();
//							}
//							delivery_hotspot << currentTime - CHotspot::SLOT_HOTSPOT_UPDATE << TAB ;
//							delivery_statistics << currentTime - CHotspot::SLOT_HOTSPOT_UPDATE << TAB << CData::getCountDeliveryAtHotspot() << TAB 
//								<< CData::getCountDeliveryTotal() << TAB << CData::getPercentDeliveryAtHotspot() << endl;
//							delivery_statistics.close();
//							hasMoreNewRoutes = true;
//						}
//
//						(*iMANode)->updateRoute(CSink::getSink()->popRoute());
//						flash_cout << "####  ( MA " << (*iMANode)->getID() << " update route )               " ;
//					}
//					//FIXME: ��·����Ŀ���٣�ɾ�������MA
//					else
//					{
//						(*iMANode)->setFlag(false);
//					}
//
//					//ͳ�ƾ��ȵ��Ͷ�ݼ�����Ϣ
//					for(vector<CBasicEntity *>::iterator iHotspot = overdueHotspots.begin(); iHotspot != overdueHotspots.end(); ++iHotspot)
//					{
//						if( (*iHotspot)->getID() == CSink::getSink()->getID() )
//							continue;
//						CHotspot *hotspot = static_cast<CHotspot *>(*iHotspot);
//						countsDelivery.push_back( *hotspot );
//					}
//					if( ! (*iMANode)->getFlag() )
//						break;
//				}
//				else if( ! CSink::getSink()->hasMoreNewRoutes() )
//				{
//					//�ȵ㼯�ϸ��½���������flag
//					hasMoreNewRoutes = false;
//				}
//			}
//			//�������hotspot��waitingTime��δ��ȡ
//			if(   (*iMANode)->isAtHotspot() 
//					&& (*iMANode)->getWaitingTime() < 0 )
//			{
//				waiting_time.open("waiting-time.log", ios::app);
//
//				CHotspot *atHotspot = (*iMANode)->getAtHotspot();
//				int temp = ROUND( getWaitingTime(currentTime, atHotspot) );
//				//FIXME: ���������Buffer�����Buffer����ʱ��ֱ������waiting
//				if( ( CMANode::MODE_RECEIVE == CGeneralNode::_selfish ) && (*iMANode)->isFull() )
//					(*iMANode)->setWaitingTime( 0 );
//				else
//				{
//					(*iMANode)->setWaitingTime( temp );
//					atHotspot->addWaitingTime( temp );
//					flash_cout << "####  ( MA " << (*iMANode)->getID() << " wait for " << temp << " )                               " ;
//					//if(temp == 0)
//					//	(*iMANode)->setAtHotspot(nullptr);
//				}
//
//				//��¼waiting time��Ϣ
//				if( temp > 0)
//				{
//					waiting_time << atHotspot->getTime() << TAB << (*iMANode)->getID() << TAB << atHotspot->getID() << TAB ;
//					switch( atHotspot->getTypeHotspotCandidate() )
//					{
//						case CHotspot::_old_hotspot: 
//							waiting_time << "O/" ;
//							break;
//						case CHotspot::_merge_hotspot: 
//							waiting_time << "M/" ;
//							break;
//						case CHotspot::_new_hotspot: 
//							waiting_time << "N/" ;
//							break;
//						default:
//							break;
//					}
//					waiting_time << atHotspot->getAge() << TAB << atHotspot->getNCoveredPosition() << TAB 
//								 << atHotspot->getHeat() << TAB << temp << endl;
//				}
//				waiting_time.close();	
//
//			}
//		}while((*iMANode)->getTime() < currentTime);
//		
//		//ɾ�������MA
//		if( ! (*iMANode)->getFlag() )
//		{
//			(*iMANode)->turnFree();
//			continue;
//		}
//		else
//			++iMANode;	
//	}
//
//	//Ͷ������
//	for(auto iMANode = CMANode::getMANodes().begin(); iMANode != CMANode::getMANodes().end(); ++iMANode)
//	{
//		for(auto inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
//		{
//			if( ( CMANode::MODE_RECEIVE == CGeneralNode::_selfish ) && (*iMANode)->isFull() )
//				break;
//			if(CBasicEntity::getDistance( **iMANode, **inode ) > CGeneralNode::RANGE_TRANS)
//				continue;
//
//			//�����ȵ��Ϻ�·���Ϸֱ�ͳ����������
//			if( (*iMANode)->isAtHotspot() )
//				CNode::encountAtHotspot();
//			else
//				CNode::encountOnRoute();
//
//			if( ! (*inode)->hasData() )
//				continue;
//
//			vector<CData> data;
//			int capacity = (*iMANode)->getToleranceData();
//			if( capacity > 0 )
//			{
//				data = (*inode)->sendData(capacity);
//				(*iMANode)->receiveData(currentTime, data);			
//			}
//			flash_cout << "####  ( Node " << (*inode)->getID() << " send " << data.size() << " data to MA " << (*iMANode)->getID() << " )                    " ;
//
//			//�����ȵ��Ϻ�·���Ϸֱ�ͳ������Ͷ�ݼ���
//			if( (*iMANode)->isAtHotspot() )
//			{
//				CData::deliverAtHotspot( data.size() );
//				(*iMANode)->getAtHotspot()->addDeliveryCount( data.size() );
//			}
//			else
//				CData::deliverOnRoute( data.size() );
//
//		}
//	}
//
//	//�������нڵ��buffer״̬��¼
//	for(auto inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
//		(*inode)->recordBufferStatus();
//
//	//����̨���ʱ����һλС��
//	double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
//	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl << endl;
//	delivery_hotspot.close();
//}

void HAR::PrintHotspotInfo(int currentTime)
{
	if( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE  != 0 )
		return;
	
	//�ȵ����
	ofstream hotspot( PATH_ROOT + PATH_LOG + FILE_HOTSPOT, ios::app);
	if(currentTime == CHotspot::TIME_HOSPOT_SELECT_START)
	{
		hotspot << endl << INFO_LOG << endl ;
		hotspot << INFO_HOTSPOT ;
	}
	hotspot << currentTime << TAB << CHotspot::selectedHotspots.size() << endl; 
	hotspot.close();

	//�ڵ����ȵ��ڵİٷֱȣ����ȵ�ѡȡ��ʼʱ��ʼͳ�ƣ�
	ofstream at_hotspot( PATH_ROOT + PATH_LOG + FILE_AT_HOTSPOT, ios::app);
	if(currentTime == CHotspot::TIME_HOSPOT_SELECT_START)
	{
		at_hotspot << endl << INFO_LOG << endl ; 
		at_hotspot << INFO_AT_HOTSPOT ;
	}
	at_hotspot << currentTime << TAB << CNode::getVisiterAtHotspot() << TAB << CNode::getVisiter() << TAB << CNode::getPercentVisiterAtHotspot() << endl;
	at_hotspot.close();

}

void HAR::PrintInfo(int currentTime)
{
	if( ! ( ( currentTime % SLOT_LOG == 0 
		      && currentTime >= CHotspot::TIME_HOSPOT_SELECT_START )
			|| currentTime == RUNTIME  ) )
		return;

	/***************************************** �ȵ�ѡȡ�������� *********************************************/

	PrintHotspotInfo(currentTime);


	/***************************************** ·��Э���ͨ����� *********************************************/

	CRoutingProtocol::PrintInfo(currentTime);


	/****************************************** HAR·�ɵĲ������ *********************************************/

	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption��MA�ڵ�buffer״̬ ...

	//���ڼ����ȵ������ʷƽ��ֵ
	SUM_HOTSPOT_COST += m_hotspots.size();
	++COUNT_HOTSPOT_COST;

	//�ȵ�����ͳ����Ϣ
	ofstream hotspot_statistics( PATH_ROOT + PATH_LOG + FILE_HOTSPOT_STATISTICS, ios::app);
	if(currentTime == CHotspot::TIME_HOSPOT_SELECT_START)
	{
		hotspot_statistics << endl << INFO_LOG << endl ;
		hotspot_statistics << INFO_HOTSPOT_STATISTICS ;
	}
	int sumCover = 0;
	for(vector<CHotspot *>::iterator it = m_hotspots.begin(); it != m_hotspots.end(); ++it)
		sumCover += (*it)->getNCoveredPosition();
	hotspot_statistics << currentTime << TAB << sumCover << TAB << m_hotspots.size() << TAB << double( sumCover ) / double( m_hotspots.size() ) << endl;
	hotspot_statistics.close();

	if( HOTSPOT_SELECT == _merge )
	{
		//�ȵ�鲢����ͳ����Ϣ��������ѡȡ�����ȵ㼯���У�
		if( HOTSPOT_SELECT == _merge )
		{
			int mergeCount = 0;
			int oldCount = 0;
			int newCount = 0;
			ofstream merge( PATH_ROOT + PATH_LOG + FILE_MERGE, ios::app);
			ofstream merge_details( PATH_ROOT + PATH_LOG + FILE_MERGE_DETAILS, ios::app);

			if(currentTime == CHotspot::TIME_HOSPOT_SELECT_START)
			{
				merge << endl << INFO_LOG << endl ;
				merge << INFO_MERGE ;
				merge_details << endl << INFO_LOG << endl ;
				merge_details << INFO_MERGE_DETAILS ;
			}
			merge_details << currentTime << TAB;

			//�ȵ����ͼ�����ͳ����Ϣ
			for(vector<CHotspot *>::iterator ihotspot = m_hotspots.begin(); ihotspot != m_hotspots.end(); ++ihotspot)
			{
				if( (*ihotspot)->getTypeHotspotCandidate() == CHotspot::_merge_hotspot )
				{
					merge_details << "M/" << (*ihotspot)->getAge() << TAB;
					++mergeCount;
				}
				else if( (*ihotspot)->getTypeHotspotCandidate() == CHotspot::_old_hotspot )
				{
					merge_details << "O/" << (*ihotspot)->getAge() << TAB;
					++oldCount;
				}
				else
				{
					merge_details << "N/" << (*ihotspot)->getAge() << TAB;
					++newCount;
				}
			}

			//�����ȵ���ռ�ı���
			int total = m_hotspots.size();
			merge << currentTime << TAB << mergeCount << TAB << double( mergeCount ) / double( total ) << TAB << oldCount << TAB 
				<< double( oldCount ) / double( total ) << TAB << newCount << TAB << double( newCount ) / double( total ) << endl;

			//���ڼ���鲢�ȵ�;��ȵ���ռ��������ʷƽ��ֵ��Ϣ
			SUM_PERCENT_MERGE += double( mergeCount ) / double( total );
			++COUNT_PERCENT_MERGE;
			SUM_PERCENT_OLD += double( oldCount ) / double( total );
			++COUNT_PERCENT_OLD;

			merge.close();
			merge_details.close();
		}
	}

	//MA�ڵ����
	ofstream ma( PATH_ROOT + PATH_LOG + FILE_MA, ios::app);
	if(currentTime == CHotspot::TIME_HOSPOT_SELECT_START)
	{
		ma << endl << INFO_LOG << endl ;
		ma << INFO_MA ;
	}
	ma << currentTime << TAB << m_routes.size() << TAB << ( double( m_hotspots.size() ) / double( m_routes.size() ) ) << endl;
	ma.close();

	//���ڼ���MA�ڵ��������ʷƽ��ֵ��Ϣ
	SUM_MA_COST += m_routes.size();
	++COUNT_MA_COST;
	//���ڼ���MA·�㣨�ȵ㣩ƽ����������ʷƽ��ֵ��Ϣ
	SUM_WAYPOINT_PER_MA += double( m_hotspots.size() ) / double( m_routes.size() );
	++COUNT_WAYPOINT_PER_MA;

	//ED��ƽ��Ͷ���ӳٵ�����ֵ
	ofstream ed( PATH_ROOT + PATH_LOG + FILE_ED, ios::app);
	if(currentTime == CHotspot::TIME_HOSPOT_SELECT_START)
	{
		ed << endl << INFO_LOG << endl ;
		ed << INFO_ED ;
	}
	ed << currentTime << TAB << calculateEDTime(currentTime) << endl;
	ed.close();


	//Buffer
	if( currentTime % SLOT_LOG == 0 )
	{
		//ÿ��MA�ĵ�ǰbuffer״̬
		ofstream buffer_ma( PATH_ROOT + PATH_LOG + FILE_BUFFER_MA, ios::app);
		if(currentTime == CHotspot::TIME_HOSPOT_SELECT_START)
		{
			buffer_ma << endl << INFO_LOG << endl ;
			buffer_ma << INFO_BUFFER_MA ;
		}
		buffer_ma << currentTime << TAB;
		for(auto iMA = CMANode::getMANodes().begin(); iMA != CMANode::getMANodes().end(); ++iMA)
			buffer_ma << (*iMA)->getSizeBuffer() << TAB ;
		buffer_ma << endl;
		buffer_ma.close();

	}

}

void HAR::PrintFinal(int currentTime)
{
	CRoutingProtocol::PrintFinal(currentTime);

	//����final��������䣩
	ofstream final( PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app);
	final << getAverageMACost() << TAB ;
	final << CData::getPercentDeliveryAtHotspot() << TAB ;
	if( HOTSPOT_SELECT == _merge )
		final << getAveragePercentMerge() << TAB << getAveragePercentOld() << TAB ;
	if( TEST_HOTSPOT_SIMILARITY )
		final << getAverageSimilarityRatio() << TAB ;
	final.close();
	
}

void HAR::CompareWithOldHotspots(int currentTime)
{
	if( CHotspot::oldSelectedHotspots.empty() )
		return ;

	double overlapArea = CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots, CHotspot::selectedHotspots);
	double oldArea = CHotspot::oldSelectedHotspots.size() * AreaCircle(CGeneralNode::RANGE_TRANS) - CHotspot::getOverlapArea(CHotspot::oldSelectedHotspots);
	double newArea = CHotspot::selectedHotspots.size() * AreaCircle(CGeneralNode::RANGE_TRANS) - CHotspot::getOverlapArea(CHotspot::selectedHotspots);

	ofstream similarity( PATH_ROOT + PATH_LOG + FILE_HOTSPOT_SIMILARITY, ios::app);
	if( currentTime == CHotspot::TIME_HOSPOT_SELECT_START + CHotspot::SLOT_HOTSPOT_UPDATE )
	{
		similarity << endl << endl << INFO_LOG << endl ;
		similarity << INFO_HOTSPOT_SIMILARITY;
	}
	similarity << currentTime << TAB << ( overlapArea / oldArea ) << TAB << ( overlapArea / newArea ) << TAB
			   << overlapArea << TAB << oldArea << TAB << newArea << endl;
	similarity.close();

	//���ڼ�������ѡȡ�����ȵ��ǰ�����ƶȵ���ʷƽ��ֵ��Ϣ
	SUM_SIMILARITY_RATIO += overlapArea / oldArea;
	++COUNT_SIMILARITY_RATIO;
}

//void HAR::DecayPositionsWithoutDeliveryCount(int currentTime)
//{
//	if(currentTime == 0)
//		return ;
//	vector<CPosition*> badPositions;
//	if( CHotspot::oldSelectedHotspots.empty() )
//		return ;
//
//	for(vector<CHotspot*>::iterator ihotspot = CHotspot::oldSelectedHotspots.begin(); ihotspot != CHotspot::oldSelectedHotspots.end(); )
//	{
//		if( (*ihotspot)->getCountDelivery(currentTime) == 0 )
//		{
//			AddToListUniquely( badPositions, (*ihotspot)->getCoveredPositions() );
//			//free(*ihotspot);
//			//��mHAR�У�Ӧ�ÿ����Ƿ���Щ�ȵ��ų��ڹ鲢֮��
//			//CHotspot::deletedHotspots.push_back(*ihotspot);
//			//ihotspot = CHotspot::oldSelectedHotspots.erase(ihotspot);
//			++ihotspot;
//		}
//		else
//			++ihotspot;
//	}
//	for(vector<CPosition*>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
//	{
//		if( IfExists(badPositions, *ipos) )
//		{
//			(*ipos)->decayWeight();
//			//Reduce complexity
//			RemoveFromList(badPositions, *ipos);
//			//���Ȩֵ������Сֵ��ֱ��ɾ����MIN_POSITION_WEIGHTĬ��ֵΪ1��������ɾ���κ�position
////			if( (*ipos)->getWeight() < MIN_POSITION_WEIGHT )
////			{
////				CPosition::deletedPositions.push_back(*ipos);
////				ipos = CPosition::positions.erase(ipos);
////			}
////			else
////				++ipos;
//			++ipos;
//		}
//		else
//			++ipos;
//	}
//}

bool HAR::Operate(int currentTime)
{
//	if( MAC_PROTOCOL == _hdc )
//		CHDC::Operate(currentTime);
//	else
//		CSMac::Operate(currentTime);
//
//	if( ! CNode::hasNodes(currentTime) )
//		return false;
//
//	HotspotSelection(currentTime);
//
//	SendData(currentTime);
//
//	PrintInfo(currentTime);

	return true;
}