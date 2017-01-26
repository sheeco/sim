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
int HAR::SUM_MA_COST = 0;
int HAR::COUNT_MA_COST = 0;
double HAR::SUM_WAYPOINT_PER_MA = 0;
int HAR::COUNT_WAYPOINT_PER_MA = 0;

double HAR::BETA = 0.0025;  //ratio for true hotspot
//double HAR::GAMMA = 0.5;  //ratio for HotspotsAboveAverage
double HAR::CO_HOTSPOT_HEAT_A1 = 1;
double HAR::CO_HOTSPOT_HEAT_A2 = 30;

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
		sum_generationRate += CNode::getNodeByID( coveredNodes[i] )->getDataCountRate();
	}

	double ratio = 1;

	return ratio * ( CO_HOTSPOT_HEAT_A1 * nCoveredNodes + CO_HOTSPOT_HEAT_A2 * sum_generationRate ) ;
}

double HAR::calculateWaitingTime(int currentTime, CHotspot *hotspot)
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
			temp_time = min(currentTime, CHotspotSelect::LIFETIME_POSITION);
		}

		nCoveredPositionsForNode.push_back( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) );
		temp = double( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) ) / double( temp_time + CHotspotSelect::SLOT_HOTSPOT_UPDATE );

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
		sum += CNode::getNodeByID( nodes[i] )->getDataCountRate();
	}
	return sum;
}

double HAR::getTimeIncreForInsertion(int currentTime, CRoute route, int front, CHotspot *hotspot)
{
	double result = calculateWaitingTime(currentTime, hotspot) + ( route.getIncreDistance(front, hotspot) / CMANode::getSpeed() );
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
		sum_waitingTime += calculateWaitingTime(currentTime, *ihotspot);
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

void HAR::HotspotClassification(int currentTime)
{
	if( ! ( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0 
		&& currentTime >= CHotspotSelect::STARTTIME_HOSPOT_SELECT ) )
		return;

	vector<CHotspot *> temp_hotspots = CHotspot::getSelectedHotspots();
	m_hotspots = CHotspot::getSelectedHotspots();
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
				int best_front = -1;
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
				//throw string("HAR::HotspotClassification() : A single hotspot's buffer expection > BUFFER_CAPACITY_MA");

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
	if( !( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
		  && currentTime >= CHotspotSelect::STARTTIME_HOSPOT_SELECT ) )
		return;

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

	flash_cout << "######  [ MA ]  " << CMANode::getMANodes().size() << endl;
}

vector<CPacket*> HAR::receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int currentTime)
{
	vector<CPacket*> packetsToSend;

	if( typeid( gToNode ) == typeid( CSink ) )
	{
		CSink* toSink = dynamic_cast< CSink* >( &gToNode );

		/*********************************************** Sink <- MA *******************************************************/

		if( typeid( gFromNode ) == typeid( CMANode ) )
		{
			CMANode* fromMA = dynamic_cast<CMANode*>( &gFromNode );
			packetsToSend = HAR::receivePackets(toSink, fromMA, packets, currentTime);
		}
	}

	else if( typeid( gToNode ) == typeid( CMANode ) )
	{
		CMANode* toMA = dynamic_cast<CMANode*>( &gToNode );

		/************************************************ MA <- sink *******************************************************/

		if( typeid( gFromNode ) == typeid( CSink ) )
		{
			CSink* fromSink = dynamic_cast< CSink* >( &gFromNode );
			packetsToSend = HAR::receivePackets(toMA, fromSink, packets, currentTime);
		}

		/************************************************ MA <- node *******************************************************/

		else if( typeid( gFromNode ) == typeid( CNode ) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( &gFromNode );
			packetsToSend = HAR::receivePackets(toMA, fromNode, packets, currentTime);
		}
	}

	else if( typeid( gToNode ) == typeid( CNode ) )
	{
		CNode* node = dynamic_cast<CNode*>( &gToNode );

		/************************************************ Node <- MA *******************************************************/

		if( typeid( gFromNode ) == typeid( CMANode ) )
		{
			CMANode* fromMA = dynamic_cast<CMANode*>( &gFromNode );
			packetsToSend = HAR::receivePackets(node, fromMA, packets, currentTime);
		}
	}

	// TODO: + comm : node <--> sink ?

	return packetsToSend;
}

vector<CPacket*> HAR::receivePackets(CSink* sink, CMANode* fromMA, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;

	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{
		if( typeid(**ipacket) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*ipacket);
			switch( ctrl->getType() )
			{
			case CCtrl::_rts:

				//CTS
				ctrlToSend = new CCtrl(sink->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_cts);

				break;

			case CCtrl::_cts:

				break;

			case CCtrl::_capacity:

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_ack:

				break;

			case CCtrl::_no_data:

				break;

			default:

				break;
			}
			++ipacket;
		}

		else if( typeid(**ipacket) == typeid(CData) )
		{
			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back( *dynamic_cast<CData*>(*ipacket) );
				++ipacket;
			} while( ipacket != packets.end() );

			//accept data into buffer
			vector<CData> ack = CSink::bufferData(time, datas);

			//ACK������յ�������ȫ�������������Ϳյ�ACK��
			ctrlToSend = new CCtrl(CSink::SINK_ID, ack, time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	CFrame* frameToSend = nullptr;
	if( ctrlToSend != nullptr )
		packetsToSend.push_back(ctrlToSend);

	return packetsToSend;

}

vector<CPacket*> HAR::receivePackets(CMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	vector<CData> dataToSend;  //��vector���������Ϊ��

	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid(**ipacket) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*ipacket);
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

			case CCtrl::_rts:

				if( ! ma->hasData() )
				{
					return packetsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(ma->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_cts);
				// + DATA
				dataToSend = ma->getAllData();

				// TODO: mark skipRTS ?
				// TODO: connection established ?
				break;

			case CCtrl::_cts:

				if( !ma->hasData() )
				{
					return packetsToSend;
				}
				// + DATA
				dataToSend = ma->getAllData();

				break;

			case CCtrl::_capacity:

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_no_data:

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//�յ��յ�ACKʱ�������������ݴ���
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					ma->checkDataByAck( ctrl->getACK() );

				flash_cout << "######  < " << time << " >  (  MA  " << ma->getID() << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Sink )       " ;

				return packetsToSend;

				break;

			default:
				break;
			}
			++ipacket;
		}
		else
		{
			++ipacket;
		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
	{
		packetsToSend.push_back(ctrlToSend);
	}
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			packetsToSend.push_back(new CData(*idata));
	}

	return packetsToSend;
	
}

vector<CPacket*> HAR::receivePackets(CNode* node, CMANode* fromMA, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	vector<CData> dataToSend;  //��vector���������Ϊ��
	int capacity = -1;

	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid(**ipacket) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*ipacket);
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

			case CCtrl::_rts:

				if( node->getAllData().empty() )
				{
					return packetsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(node->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_cts);

				// + DATA
				dataToSend = node->getAllData();

				if( dataToSend.empty() )
					return packetsToSend;

				break;

			case CCtrl::_cts:

				break;

				/************************************* rcv capacity **********************************/

			case CCtrl::_capacity:

				capacity = ctrl->getCapacity();

				if( capacity == 0 )
					return packetsToSend;
				else if( capacity > 0
						 && capacity < CNode::CAPACITY_BUFFER 
						 && capacity < dataToSend.size() )
					CNode::removeDataByCapacity(dataToSend, capacity, false);

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_no_data:

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//�յ��յ�ACKʱ�������������ݴ���
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					node->checkDataByAck( ctrl->getACK() );

				flash_cout << "######  < " << time << " >  ( Node  " << NDigitString(node->getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->   MA  " << ctrl->getNode() << " )       " ;

				return packetsToSend;

				break;

			default:
				break;
			}
			++ipacket;
		}
		else
		{
			++ipacket;
		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
	{
		packetsToSend.push_back(ctrlToSend);
	}
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			packetsToSend.push_back(new CData(*idata));
	}

	return packetsToSend;
	
}

vector<CPacket*> HAR::receivePackets(CMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;

	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{
		if( typeid(**ipacket) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*ipacket);
			switch( ctrl->getType() )
			{
			case CCtrl::_rts:

				break;

			case CCtrl::_cts:

				break;

			case CCtrl::_capacity:

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_ack:

				break;

			case CCtrl::_no_data:

				break;

			default:

				break;
			}
			++ipacket;
		}

		else if( typeid(**ipacket) == typeid(CData) )
		{
			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back( *dynamic_cast<CData*>(*ipacket) );
				++ipacket;
			} while( ipacket != packets.end() );

			//accept data into buffer
			vector<CData> ack = ma->bufferData(time, datas);

			//ACK������յ�������ȫ�������������Ϳյ�ACK��
			ctrlToSend = new CCtrl(ma->getID(), ack, time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	CFrame* frameToSend = nullptr;
	if( ctrlToSend != nullptr )
		packetsToSend.push_back(ctrlToSend);

	return packetsToSend;

}

void HAR::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_LOG == 0 
			|| currentTime == RUNTIME  ) )
		return;


	/***************************************** ·��Э���ͨ����� *********************************************/

	CRoutingProtocol::PrintInfo(currentTime);

	/***************************************** �ȵ�ѡȡ�������� *********************************************/

	CHotspotSelect::PrintInfo(currentTime);

	if( ! ( currentTime >= CHotspotSelect::STARTTIME_HOSPOT_SELECT ) )
		return;

	/**************************************** HAR ·�ɵĲ������ *********************************************/

	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption��MA�ڵ�buffer״̬ ...

	if( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
	    || currentTime == RUNTIME )
	{
		//MA�ڵ����
		ofstream ma(DIR_LOG + PATH_TIMESTAMP + FILE_MA, ios::app);
		if( currentTime == CHotspotSelect::STARTTIME_HOSPOT_SELECT )
		{
			ma << endl << INFO_LOG << endl;
			ma << INFO_MA;
		}
		ma << currentTime << TAB << m_routes.size() << TAB << ( double(m_hotspots.size()) / double(m_routes.size()) ) << endl;
		ma.close();

		//
		ofstream ma_route(DIR_LOG + PATH_TIMESTAMP + FILE_MA_ROUTE, ios::app);
		if( currentTime == CHotspotSelect::STARTTIME_HOSPOT_SELECT )
		{
			ma_route << endl << INFO_LOG << endl;
			ma_route << INFO_MA_ROUTE;
		}
		for( vector<CRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); iroute++ )
		{
			ma_route << currentTime << TAB << iroute->toString() << endl;
		}
		ma_route.close();			

		//���ڼ���MA�ڵ��������ʷƽ��ֵ��Ϣ
		SUM_MA_COST += m_routes.size();
		++COUNT_MA_COST;
		//���ڼ���MA·�㣨�ȵ㣩ƽ����������ʷƽ��ֵ��Ϣ
		SUM_WAYPOINT_PER_MA += double(m_hotspots.size()) / double(m_routes.size());
		++COUNT_WAYPOINT_PER_MA;

		//ED��ƽ��Ͷ���ӳٵ�����ֵ
		ofstream ed(DIR_LOG + PATH_TIMESTAMP + FILE_ED, ios::app);
		if( currentTime == CHotspotSelect::STARTTIME_HOSPOT_SELECT )
		{
			ed << endl << INFO_LOG << endl;
			ed << INFO_ED;
		}
		ed << currentTime << TAB << calculateEDTime(currentTime) << endl;
		ed.close();

		//�ȵ�������Ͷ�ݼ�����ͳ����Ϣ
		ofstream hotspot_statistics(DIR_LOG + PATH_TIMESTAMP + FILE_HOTSPOT_STATISTICS, ios::app);
		if( currentTime == CHotspotSelect::STARTTIME_HOSPOT_SELECT )
		{
			hotspot_statistics << endl << INFO_LOG << endl;
			hotspot_statistics << INFO_HOTSPOT_STATISTICS;
		}
		//�� t ��ʱ��ѡ�����ȵ㣬�������ڽ����� t + 900���� t + 1800 ʱ�̲ű�ͳ�����
		int timeBeforeYesterday = currentTime - 2 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
		vector<int> timesToPrint;
		if( timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOSPOT_SELECT )
			timesToPrint.push_back(timeBeforeYesterday);
		if( ! timesToPrint.empty()
		   || currentTime == RUNTIME )
		{
			//���н��������������һ�ֵ��ȵ�ͳ��
 			if( currentTime == RUNTIME )
			{
				timeBeforeYesterday = ( currentTime / CHotspotSelect::SLOT_HOTSPOT_UPDATE - 1 ) * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
				int timeYesterday = timeBeforeYesterday + CHotspotSelect::SLOT_HOTSPOT_UPDATE;
				if( timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOSPOT_SELECT )
					timesToPrint.push_back(timeBeforeYesterday);
				if( timeYesterday >= CHotspotSelect::STARTTIME_HOSPOT_SELECT )
					timesToPrint.push_back(timeYesterday);
			}

			for( vector<int>::iterator itime = timesToPrint.begin(); itime != timesToPrint.end(); itime++ )
			{
				vector<CHotspot *> hotspotsToPrint = CHotspot::getSelectedHotspots(*itime);
				hotspotsToPrint = CSortHelper::mergeSort(hotspotsToPrint, CSortHelper::descendByCountDelivery);
				for( vector<CHotspot *>::iterator it = hotspotsToPrint.begin(); it != hotspotsToPrint.end(); ++it )
					hotspot_statistics << *itime << '-' << *itime + CHotspotSelect::SLOT_HOTSPOT_UPDATE << TAB
					<< ( *it )->getID() << TAB << ( *it )->getLocation().toString() << TAB << ( *it )->getNCoveredPosition() << "," << ( *it )->getNCoveredNodes() << TAB
					<< ( *it )->getRatio() << TAB << ( *it )->getWaitingTimesString(true) << TAB << ( *it )->getCountDelivery(*itime) << endl;
			}
		}
		hotspot_statistics.close();
	}

	//MA Buffer
	if( currentTime % SLOT_LOG == 0
	    || currentTime == RUNTIME )
	{
		//ÿ��MA�ĵ�ǰbuffer״̬
		ofstream buffer_ma( DIR_LOG + PATH_TIMESTAMP + FILE_BUFFER_MA, ios::app);
		if(currentTime == CHotspotSelect::STARTTIME_HOSPOT_SELECT)
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
	ofstream final( DIR_LOG + PATH_TIMESTAMP + FILE_FINAL, ios::app);
	final << getAverageMACost() << TAB ;
	final << CData::getPercentDeliveryAtHotspot() << TAB ;
	final.close();

	CHotspotSelect::PrintFinal(currentTime);
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
	bool hasNodes = true;
	// ������ xHAR ʹ�� HDC ��Ϊ MAC Э��
	//if( MAC_PROTOCOL == _hdc )
	//	hasNodes = CHDC::Prepare(currentTime);
	//else 
	if( MAC_PROTOCOL == _smac )
		hasNodes = CSMac::Prepare(currentTime);

	if( ! hasNodes )
		return false;

	HotspotClassification(currentTime);

	MANodeRouteDesign(currentTime);

	CMacProtocol::UpdateMANodeStatus(currentTime);

	// ������ xHAR ʹ�� HDC ��Ϊ MAC Э��
	//if( MAC_PROTOCOL == _hdc )
	//	CHDC::Operate(currentTime);
	//else 
	if( MAC_PROTOCOL == _smac )
		CSMac::Operate(currentTime);

	PrintInfo(currentTime);

	return true;
}