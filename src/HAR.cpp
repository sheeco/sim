#include "Global.h"
#include "Configuration.h"
#include "HAR.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "NodeRepair.h"
#include "Sink.h"
#include "MANode.h"
#include "Node.h"
#include "HDC.h"
#include "SMac.h"
#include "PrintHelper.h"

map<int, double> HAR::mapDataCountRates;

vector<CHARMANode *> HAR::allMAs;
vector<CHARMANode *> HAR::busyMAs;
vector<CHARMANode *> HAR::freeMAs;
int HAR::INIT_NUM_MA;
int HAR::MAX_NUM_MA;

void CHARMANode::updateStatus(int time)
{
	if( this->time < 0 )
		this->time = time;
	int interval = time - this->time;

	//updateTimerOccupied(time);

	if(this->routeIsOverdue())
		setReturnAtOnce(true);

	//·�߹��ڻ򻺴���������������sink
	if( ifReturnAtOnce()
	   || ( getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_selfish
		   && buffer.size() >= capacityBuffer ) )
	{
		//��¼һ��δ�������ڵĵȴ�������¼��(t, 0)����˵������·�߹��ڶ������ȴ�
		if( isAtWaypoint() )
			getAtHotspot()->recordWaitingTime(time - waitingState, waitingState);

		waitingWindow = waitingState = 0;
		route->updateToPointWithSink();
	}

	//���ڵȴ���
	if( waitingWindow > 0 )
	{
		//�ȴ���������		
		if( ( waitingState + interval ) >= waitingWindow )
		{
			//��¼һ�������ڵĵȴ�
			getAtHotspot()->recordWaitingTime(time - waitingWindow, waitingWindow);

			interval = waitingState + interval - waitingWindow;  //�ȴ�������ʣ��Ľ������ƶ���ʱ��
			waitingWindow = waitingState = 0;  //�ȴ���������ʱ�䴰����
		}

		//�ȴ���δ����
		else
		{
			waitingState += interval;
			interval = 0;  //���ƶ�
		}
	}
	if( interval == 0 )
	{
		this->setTime(time);
		return;
	}

	//��ʼ��·�����ƶ�
	this->setTime(time - interval);  //��ʱ�����ڵȴ��������ƶ�������ʼ��ʱ���
	atPoint = nullptr;  //�뿪�ȵ�

	CBasicEntity *toPoint = route->getToPoint();
	int timeLeftAfterArrival = this->moveTo(*toPoint, interval, getConfig<int>("ma", "speed"));

	//����ѵ���Ŀ�ĵ�
	if( timeLeftAfterArrival >= 0 )
	{

		//��Ŀ�ĵص������� hotspot
		CHotspot *photspot = nullptr;
		CSink *psink = nullptr;
		//FIXME: nullptr ?
		if( ( photspot = dynamic_cast< CHotspot * >( toPoint ) ) != nullptr )
		{
			this->atPoint = toPoint;
			waitingWindow = int(HAR::calculateWaitingTime(time, photspot));
			waitingState = 0;  //���¿�ʼ�ȴ�

#ifdef DEBUG
			CPrintHelper::PrintBrief(time, this->getName() + " arrives at waypoint " + photspot->getLocation().format() + ".");
#endif // DEBUG
			route->updateToPoint();
		}
		//��Ŀ�ĵص������� sink
		else if( ( psink = dynamic_cast<CSink *>( toPoint ) ) != nullptr )
		{
#ifdef DEBUG
			CPrintHelper::PrintBrief(time, this->getName() + " arrives at sink.");
#endif // DEBUG
			setReturnAtOnce(true);
		}
	}
	return;

}


vector<CHotspot *> HAR::hotspots;
vector<CHARRoute *> HAR::maRoutes;
vector<CHARRoute *> HAR::oldRoutes;
int HAR::indexRoute = 0;
int HAR::SUM_MA_COST = 0;
int HAR::COUNT_MA_COST = 0;
double HAR::SUM_WAYPOINT_PER_MA = 0;
int HAR::COUNT_WAYPOINT_PER_MA = 0;


double HAR::getHotspotHeat(CHotspot *hotspot)
{
	int nCoveredNodes = hotspot->getNCoveredNodes();
	double sum_generationRate = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	for(int nodeId: coveredNodes)
	{
		sum_generationRate += mapDataCountRates[nodeId];
	}

	double ratio = 1;

	return ratio * ( getConfig<double>("har", "heat_1") * nCoveredNodes + getConfig<double>("har", "heat_2") * sum_generationRate ) ;
}

double HAR::calculateWaitingTime(int now, CHotspot *hotspot)
{
	double result = 1;
	int count_trueHotspot = 0;
	vector<int> coveredNodes = hotspot->getCoveredNodes();
	vector<int> nCoveredPositionsForNode;

	for(int i = 0; i < coveredNodes.size(); ++i)
	{
		int temp_time = now;
		double temp;

		//IHAR: Reduce Memory now
		if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved )
		{
			temp_time = min(now, getConfig<int>("ihs", "lifetime_position"));
		}

		nCoveredPositionsForNode.push_back( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) );
		temp = double( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) ) / double( temp_time + CHotspotSelect::SLOT_HOTSPOT_UPDATE );

		//merge-HAR: ratio
		temp *= pow( hotspot->getRatioByTypeHotspotCandidate(), hotspot->getAge() );

		if(temp >= getConfig<double>("har", "beta"))
		{
			result *= temp;
			++count_trueHotspot;
		}
	}
	//FIXME: �������true hotspot��waiting timeΪ0
	if(count_trueHotspot == 0)
		return getConfig<int>("har", "min_waiting_time");
	double prob = exp( -1 / hotspot->getHeat() );
	result = prob / result;
	result = pow(result, ( 1 / double( count_trueHotspot ) ) );

	return result + getConfig<int>("har", "min_waiting_time");
}

double HAR::getSumDataRate(vector<int> nodes)
{
	double sum = 0;
	for(int nodeId: nodes)
	{
		sum += mapDataCountRates[nodeId];
	}
	return sum;
}

double HAR::getTimeIncreForInsertion(int now, CHARRoute route, int front, CHotspot *hotspot)
{
	double result = calculateWaitingTime(now, hotspot) + ( route.getIncreDistance(front, hotspot) / CHARMANode::getSpeed() );
	return result;
}

double HAR::calculateRatioForInsertion(int now, CHARRoute route, int front, CHotspot *hotspot)
{
	double time_incr = getTimeIncreForInsertion(now, route, front, hotspot);
	vector<int> temp_nodes = route.getCoveredNodes();
	AddToListUniquely(temp_nodes, hotspot->getCoveredNodes());

	double sumGenerationRate = getSumDataRate(temp_nodes);
	return ( time_incr * sumGenerationRate );
}

double HAR::calculateEDTime(int now)
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

	for(vector<CHARRoute*>::iterator iroute = maRoutes.begin(); iroute != maRoutes.end(); ++iroute)
		sum_length += (*iroute)->getLength();
	avg_length = sum_length / hotspots.size() + 1;
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
	{
		sum_waitingTime += calculateWaitingTime(now, *ihotspot);
		sum_pm += exp( -1 / (*ihotspot)->getHeat() );
	}
	avg_waitingTime = sum_waitingTime / hotspots.size();
	avg_u = avg_length / CHARMANode::getSpeed() + avg_waitingTime;
	avg_pw = sum_pm / hotspots.size();
	pmh = sum_waitingTime / (sum_length / CHARMANode::getSpeed() + sum_waitingTime);
	EM = (1 - pmh) * ( ( (1 - avg_pw) / avg_pw) * avg_u + (avg_length / (2 * CHARMANode::getSpeed()) + avg_waitingTime) ) + pmh * ( ( (1 - avg_pw) / avg_pw) * avg_u + avg_waitingTime);
	EIM = avg_u / avg_pw;
	ED = EM + ( (1 - getConfig<double>("trans", "prob_trans")) / getConfig<double>("trans", "prob_trans") ) * EIM + ( double( hotspots.size() ) / (2 * maRoutes.size()) ) * avg_u;

	return ED;
}


void HAR::OptimizeRoute(CHARRoute *route)
{
	vector<pair<CBasicEntity *, int>> waypoints = route->getWayPoints();
	CBasicEntity *current = CSink::getSink();
	CHARRoute result(route->getTimeCreation(), route->getTimeExpiration());
	waypoints.erase(waypoints.begin());
	while(! waypoints.empty())
	{
		double min_distance = -1;
		vector<pair<CBasicEntity *, int>>::iterator min_point;
		for(vector<pair<CBasicEntity *, int>>::iterator ipoint = waypoints.begin(); ipoint != waypoints.end(); ++ipoint)
		{
			double distance = CBasicEntity::getDistance(*ipoint->first, *current);
			if( min_distance < 0 
				|| distance < min_distance)
			{
				min_distance = distance;
				min_point = ipoint;
			}
		}
		result.AddHotspot(min_point->first);
		current = min_point->first;
		waypoints.erase(min_point);
	}
	result.updateLength();
	*route = result;
}

void HAR::HotspotClassification(int now)
{
	if( ! ( now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0 
		&& now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT ) )
		return;

	vector<CHotspot *> temp_hotspots = hotspots;
	for(vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin(); ihotspot != temp_hotspots.end(); ++ihotspot)
	{
		(*ihotspot)->setHeat( getHotspotHeat(*ihotspot) );
	}
	
	vector<CHARRoute*> newRoutes;
	
	while(! temp_hotspots.empty())
	{
		//����һ��hotspot class
		CHARRoute* route = new CHARRoute(now, now + CHotspotSelect::SLOT_HOTSPOT_UPDATE);
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
			int size_waypoints = route->getNWayPoints();
			//��������ʣ��hotspot��ѡ��ratio����hotspot���
			for(int ihotspot = 0; ihotspot < temp_hotspots.size(); ++ihotspot)
			{
				double min_length_increment = -1;
				int best_front = -1;
				for(int i = 0; i < size_waypoints; ++i)  //��Ѱ����С·����������
				{
					double length_increment = route->getIncreDistance(i, temp_hotspots[ihotspot]);
					if(length_increment < min_length_increment
						|| min_length_increment < 0)
					{
						min_length_increment = length_increment;
						best_front = i;
					}
				}

				time_increment = getTimeIncreForInsertion(now, *route, best_front, temp_hotspots[ihotspot]);
				vector<int> temp_nodes = route->getCoveredNodes();
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
			if( new_buffer > CHARMANode::getCapacityBuffer()
				&& route->getNWayPoints() == 1)
			{
				//throw string("HAR::HotspotClassification() : A single hotspot's buffer expection > BUFFER_CAPACITY_MA");

				route->AddHotspot(max_front, temp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin() + max_hotspot;
				temp_hotspots.erase(ihotspot);
				break;
			}
			if( new_buffer > CHARMANode::getCapacityBuffer() )
				break;
			else
			{
				current_time_cost += max_time_increment;
				//current_buffer = new_buffer;
				route->AddHotspot(max_front, temp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin() + max_hotspot;
				temp_hotspots.erase(ihotspot);
			}
		}
		newRoutes.push_back(route);
	}
	//���õ����µ�hotspot class����sink��route�б�
	updateRoutes(newRoutes);
}


void HAR::MANodeRouteDesign(int now)
{
	if( !( now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
		  && now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT ) )
		return;

	vector<CHARRoute*> routes = getRoutes();
	//��ÿ�������·��������ھ��㷨�����Ż�
	for(vector<CHARRoute*>::iterator iroute = routes.begin(); iroute != routes.end(); ++iroute)
	{
		OptimizeRoute( *iroute );
	}

	CPrintHelper::PrintAttribute("MA Routes", routes.size());
}

vector<CGeneralNode*> HAR::findNeighbors(CGeneralNode & src)
{
	vector<CGeneralNode*> neighbors = CMacProtocol::findNeighbors(src);

	/**************************************************** MA ***********************************************************/

	for( vector<CHARMANode*>::iterator iMA = busyMAs.begin(); iMA != busyMAs.end(); ++iMA )
	{
		//skip itself
		if( ( *iMA )->getID() == src.getID() )
			continue;

		if( CBasicEntity::withinRange(src, **iMA, getConfig<int>("trans", "range_trans"))
		   && ( *iMA )->isAwake() )
		{
			neighbors.push_back(*iMA);
		}
	}
	return neighbors;
	// TODO: sort by distance with src node ?
}

bool HAR::transmitFrame(CGeneralNode & src, CFrame * frame, int now)
{
	vector<CGeneralNode*> neighbors = findNeighbors(src);
	return CMacProtocol::transmitFrame(src, frame, now, findNeighbors, receivePackets);
}

vector<CPacket*> HAR::receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;

	if(typeid( gToNode ) == typeid( CSink ))
	{
		CSink* toSink = dynamic_cast< CSink* >( &gToNode );

		/*********************************************** Sink <- MA *******************************************************/

		if(typeid( gFromNode ) == typeid( CHARMANode ))
		{
			CHARMANode* fromMA = dynamic_cast< CHARMANode* >( &gFromNode );
			packetsToSend = HAR::receivePackets(toSink, fromMA, packets, now);
		}
	}

	else if(typeid( gToNode ) == typeid( CHARMANode ))
	{
		CHARMANode* toMA = dynamic_cast< CHARMANode* >( &gToNode );

		/************************************************ MA <- sink *******************************************************/

		if(typeid( gFromNode ) == typeid( CSink ))
		{
			CSink* fromSink = dynamic_cast< CSink* >( &gFromNode );
			packetsToSend = HAR::receivePackets(toMA, fromSink, packets, now);
		}

		/************************************************ MA <- node *******************************************************/

		else if(typeid( gFromNode ) == typeid( CNode ))
		{
			CNode* fromNode = dynamic_cast< CNode* >( &gFromNode );
			packetsToSend = HAR::receivePackets(toMA, fromNode, packets, now);
		}
	}

	else if(typeid( gToNode ) == typeid( CNode ))
	{
		CNode* node = dynamic_cast< CNode* >( &gToNode );

		/************************************************ Node <- MA *******************************************************/

		if(typeid( gFromNode ) == typeid( CHARMANode ))
		{
			CHARMANode* fromMA = dynamic_cast< CHARMANode* >( &gFromNode );
			packetsToSend = HAR::receivePackets(node, fromMA, packets, now);
		}
	}
	else
		throw string("HAR::receivePackets(): Unexpected condition.");

	// TODO: + comm : node <--> sink ?

	return packetsToSend;
}

vector<CPacket*> HAR::receivePackets(CSink* sink, CHARMANode* fromMA, vector<CPacket*> packets, int time)
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
				ctrlToSend = new CCtrl(sink->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);

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
			ctrlToSend = new CCtrl(CSink::getSink()->getID(), ack, time, getConfig<int>("data", "size_ctrl"), CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	CFrame* frameToSend = nullptr;
	if( ctrlToSend != nullptr )
		packetsToSend.push_back(ctrlToSend);

	return packetsToSend;

}

vector<CPacket*> HAR::receivePackets(CHARMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time)
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
					if( ma->ifReturnAtOnce() )
						atMAReturn(ma, time);
					return packetsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(ma->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);
				// + DATA
				dataToSend = ma->getDataForTrans(0, true);

				// TODO: mark skipRTS ?
				// TODO: connection established ?
				break;

			case CCtrl::_cts:

				if( !ma->hasData() )
				{
					if( ma->ifReturnAtOnce() )
						atMAReturn(ma, time);
					return packetsToSend;
				}
				// + DATA
				dataToSend = ma->getDataForTrans(0, true);

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
				{
					if( ma->ifReturnAtOnce() )
						atMAReturn(ma, time);
					return packetsToSend;
				}
				//clear data with ack
				else
				{
					ma->dropDataByAck(ctrl->getACK());

					CPrintHelper::PrintCommunication(time, ma->format(), fromSink->format(), ctrl->getACK().size());
					if( ma->ifReturnAtOnce() )
					{
						if( !ma->hasData() )
							atMAReturn(ma, time);
					}
				}
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

vector<CPacket*> HAR::receivePackets(CNode* node, CHARMANode* fromMA, vector<CPacket*> packets, int time)
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

				if( ! node->hasData() )
				{
					return packetsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(node->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);

				// + DATA
				dataToSend = node->getDataForTrans(0, true);

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
						 && capacity < dataToSend.size() )
					CGeneralNode::removeDataByCapacity(dataToSend, capacity, false);

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
					node->dropDataByAck( ctrl->getACK() );

				CPrintHelper::PrintCommunication(time, node->format(), fromMA->format(), ctrl->getACK().size());

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

vector<CPacket*> HAR::receivePackets(CHARMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time)
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
			ctrlToSend = new CCtrl(ma->getID(), ack, time, getConfig<int>("data", "size_ctrl"), CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	CFrame* frameToSend = nullptr;
	if( ctrlToSend != nullptr )
		packetsToSend.push_back(ctrlToSend);

	return packetsToSend;

}

void HAR::PrintInfo(int now)
{
	if( ! ( now % getConfig<int>("log", "slot_log") == 0 
			|| now == getConfig<int>("simulation", "runtime")  ) )
		return;


	/***************************************** ·��Э���ͨ����� *********************************************/

	CRoutingProtocol::PrintInfo(now);

	/***************************************** �ȵ�ѡȡ�������� *********************************************/

	CHotspotSelect::PrintInfo(now);

	if( ! ( now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT ) )
		return;

	/**************************************** HAR ·�ɵĲ������ *********************************************/

	//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption��MA�ڵ�buffer״̬ ...

	if( now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
	    || now == getConfig<int>("simulation", "runtime") )
	{
		//MA�ڵ����
		ofstream ma(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma"), ios::app);
		if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
		{
			ma << endl << getConfig<string>("log", "info_log") << endl;
			ma << getConfig<string>("log", "info_ma") << endl;
		}
		ma << now << TAB << maRoutes.size() << TAB << ( double(hotspots.size()) / double(maRoutes.size()) ) << endl;
		ma.close();

		//
		ofstream ma_route(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma_route"), ios::app);
		if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
		{
			ma_route << endl << getConfig<string>("log", "info_log") << endl;
			ma_route << getConfig<string>("log", "info_ma_route") << endl;
		}
		for( vector<CHARRoute*>::iterator iroute = maRoutes.begin(); iroute != maRoutes.end(); iroute++ )
		{
			ma_route << now << TAB << (*iroute)->format() << endl;
		}
		ma_route.close();			

		//���ڼ���MA�ڵ��������ʷƽ��ֵ��Ϣ
		SUM_MA_COST += maRoutes.size();
		++COUNT_MA_COST;
		//���ڼ���MA·�㣨�ȵ㣩ƽ����������ʷƽ��ֵ��Ϣ
		SUM_WAYPOINT_PER_MA += double(hotspots.size()) / double(maRoutes.size());
		++COUNT_WAYPOINT_PER_MA;

		//ED��ƽ��Ͷ���ӳٵ�����ֵ
		ofstream ed(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ed"), ios::app);
		if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
		{
			ed << endl << getConfig<string>("log", "info_log") << endl;
			ed << getConfig<string>("log", "info_ed") << endl;
		}
		ed << now << TAB << calculateEDTime(now) << endl;
		ed.close();

		//�ȵ�������Ͷ�ݼ�����ͳ����Ϣ
		ofstream hotspot_statistics(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_statistics"), ios::app);
		if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
		{
			hotspot_statistics << endl << getConfig<string>("log", "info_log") << endl;
			hotspot_statistics << getConfig<string>("log", "info_hotspot_statistics") << endl;
		}
		//�� t ��ʱ��ѡ�����ȵ㣬�������ڽ����� t + 900���� t + 1800 ʱ�̲ű�ͳ�����
		vector<int> timesToPrint;
		int timeBeforeYesterday = 0;
		//���н��������������һ�ֵ��ȵ�ͳ��
		if( now == getConfig<int>("simulation", "runtime") )
		{
			timeBeforeYesterday = ( now / CHotspotSelect::SLOT_HOTSPOT_UPDATE - 1 ) * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
			int timeYesterday = timeBeforeYesterday + CHotspotSelect::SLOT_HOTSPOT_UPDATE;
			if( timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
				timesToPrint.push_back(timeBeforeYesterday);
			if( timeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
				timesToPrint.push_back(timeYesterday);
		}
		else if( now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0 )
		{
			timeBeforeYesterday = now - 2 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
			if( timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
				timesToPrint.push_back(timeBeforeYesterday);
		}
		if( ! timesToPrint.empty() )
		{
			for( vector<int>::iterator itime = timesToPrint.begin(); itime != timesToPrint.end(); itime++ )
			{
				vector<CHotspot *> hotspotsToPrint = CHotspotSelect::getSelectedHotspots(*itime);
				hotspotsToPrint = CSortHelper::mergeSort(hotspotsToPrint, CSortHelper::descendByCountDelivery);
				for( vector<CHotspot *>::iterator it = hotspotsToPrint.begin(); it != hotspotsToPrint.end(); ++it )
					hotspot_statistics << *itime << '-' << *itime + CHotspotSelect::SLOT_HOTSPOT_UPDATE << TAB
					<< ( *it )->getID() << TAB << ( *it )->getLocation().format() << TAB << ( *it )->getNCoveredPosition() << "," << ( *it )->getNCoveredNodes() << TAB
					<< ( *it )->getRatio() << TAB << ( *it )->getWaitingTimesString(true) << TAB << ( *it )->getCountDelivery(*itime) << endl;
			}
		}
		hotspot_statistics.close();
	}

	//MA Buffer
	if( now % getConfig<int>("log", "slot_log") == 0
	    || now == getConfig<int>("simulation", "runtime") )
	{
		//ÿ��MA�ĵ�ǰbuffer״̬
		ofstream buffer_ma( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer_ma"), ios::app);
		if(now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
		{
			buffer_ma << endl << getConfig<string>("log", "info_log") << endl ;
			buffer_ma << getConfig<string>("log", "info_buffer_ma") << endl;
		}
		buffer_ma << now << TAB;
		for(auto iMA = busyMAs.begin(); iMA != busyMAs.end(); ++iMA)
			buffer_ma << (*iMA)->getBufferSize() << TAB ;
		buffer_ma << endl;
		buffer_ma.close();

	}

}

void HAR::PrintFinal(int now)
{
	CRoutingProtocol::PrintFinal(now);

	CHotspotSelect::PrintFinal(now);
}
