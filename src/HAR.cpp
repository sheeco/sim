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

vector<CHARMANode *> HAR::allMAs;
vector<CHARMANode *> HAR::busyMAs;
vector<CHARMANode *> HAR::freeMAs;

void CHARMANode::updateStatus(int time)
{
	if( this->time < 0 )
		this->time = time;
	int interval = time - this->time;

	//updateTimerOccupied(time);

	//更新时间戳

	//路线过期或缓存已满，立即返回sink
	if( ifReturnAtOnce()
	   || ( getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_selfish
		   && buffer.size() >= capacityBuffer ) )
	{
		//记录一次未填满窗口的等待；可能录入(t, 0)，即说明由于路线过期而跳过等待
		if( isAtWaypoint() )
			getAtHotspot()->recordWaitingTime(time - waitingState, waitingState);

		waitingWindow = waitingState = 0;
		route->updateToPointWithSink();
	}

	//处于等待中
	if( waitingWindow > 0 )
	{
		//等待即将结束		
		if( ( waitingState + interval ) >= waitingWindow )
		{
			//记录一次满窗口的等待
			getAtHotspot()->recordWaitingTime(time - waitingWindow, waitingWindow);

			interval = waitingState + interval - waitingWindow;  //等待结束后，剩余的将用于移动的时间
			waitingWindow = waitingState = 0;  //等待结束，将时间窗重置
		}

		//等待还未结束
		else
		{
			waitingState += interval;
			interval = 0;  //不移动
		}
	}
	if( interval == 0 )
	{
		this->setTime(time);
		return;
	}

	//开始在路线上移动
	this->setTime(time - interval);  //将时间置于等待结束，移动即将开始的时间点
	atPoint = nullptr;  //离开热点

	CBasicEntity *toPoint = route->getToPoint();
	int timeLeftAfterArrival = this->moveTo(*toPoint, interval, getConfig<int>("ma", "speed"));

	//如果已到达目的地
	if( timeLeftAfterArrival >= 0 )
	{

		//若目的地的类型是 hotspot
		CHotspot *photspot = nullptr;
		CSink *psink = nullptr;
		//FIXME: nullptr ?
		if( ( photspot = dynamic_cast< CHotspot * >( toPoint ) ) != nullptr )
		{
			this->atPoint = toPoint;
			waitingWindow = int(HAR::calculateWaitingTime(time, photspot));
			waitingState = 0;  //重新开始等待

			route->updateToPoint();
		}
		//若目的地的类型是 sink
		else if( ( psink = dynamic_cast<CSink *>( toPoint ) ) != nullptr )
		{
			if( routeIsOverdue() )
			{
				HAR::atMAReturn(this, time);
			}
			else
				route->updateToPoint();
		}
	}
	return;

}


vector<CHARRoute> HAR::m_newRoutes;

vector<CHotspot *> HAR::m_hotspots;
vector<CHARRoute> HAR::m_routes;
int HAR::SUM_MA_COST = 0;
int HAR::COUNT_MA_COST = 0;
double HAR::SUM_WAYPOINT_PER_MA = 0;
int HAR::COUNT_WAYPOINT_PER_MA = 0;


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
		temp = double( hotspot->getNCoveredPositionsForNode(coveredNodes[i]) ) / double( temp_time + getConfig<int>("hs", "slot_hotspot_update") );

		//merge-HAR: ratio
		temp *= pow( hotspot->getRatioByTypeHotspotCandidate(), hotspot->getAge() );

		if(temp >= getConfig<double>("har", "beta"))
		{
			result *= temp;
			++count_trueHotspot;
		}
	}
	//FIXME: 如果不是true hotspot，waiting time为0
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
	for(int i = 0; i < nodes.size(); ++i)
	{
		if( ! CNode::ifNodeExists( nodes[i] ) )
			continue;
		sum += CNode::getNodeByID( nodes[i] )->getDataCountRate();
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

	for(vector<CHARRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); ++iroute)
		sum_length += iroute->getLength();
	avg_length = sum_length / m_hotspots.size() + 1;
	for(vector<CHotspot *>::iterator ihotspot = m_hotspots.begin(); ihotspot != m_hotspots.end(); ++ihotspot)
	{
		sum_waitingTime += calculateWaitingTime(now, *ihotspot);
		sum_pm += exp( -1 / (*ihotspot)->getHeat() );
	}
	avg_waitingTime = sum_waitingTime / m_hotspots.size();
	avg_u = avg_length / CHARMANode::getSpeed() + avg_waitingTime;
	avg_pw = sum_pm / m_hotspots.size();
	pmh = sum_waitingTime / (sum_length / CHARMANode::getSpeed() + sum_waitingTime);
	EM = (1 - pmh) * ( ( (1 - avg_pw) / avg_pw) * avg_u + (avg_length / (2 * CHARMANode::getSpeed()) + avg_waitingTime) ) + pmh * ( ( (1 - avg_pw) / avg_pw) * avg_u + avg_waitingTime);
	EIM = avg_u / avg_pw;
	ED = EM + ( (1 - getConfig<double>("trans", "prob_trans")) / getConfig<double>("trans", "prob_trans") ) * EIM + ( double( m_hotspots.size() ) / (2 * m_routes.size()) ) * avg_u;

	return ED;
}


void HAR::OptimizeRoute(CHARRoute &route)
{
	vector<pair<CBasicEntity *, int>> waypoints = route.getWayPoints();
	CBasicEntity *current = CSink::getSink();
	CHARRoute result;
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
	route = result;
}

void HAR::HotspotClassification(int now)
{
	if( ! ( now % getConfig<int>("hs", "slot_hotspot_update") == 0 
		&& now >= getConfig<int>("hs", "starttime_hospot_select") ) )
		return;

	vector<CHotspot *> temp_hotspots = CHotspot::getSelectedHotspots();
	m_hotspots = CHotspot::getSelectedHotspots();
	for(vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin(); ihotspot != temp_hotspots.end(); ++ihotspot)
	{
		(*ihotspot)->setHeat( getHotspotHeat(*ihotspot) );
	}
	
	vector<CHARRoute> newRoutes;
	
	while(! temp_hotspots.empty())
	{
		//构造一个hotspot class
		CHARRoute route;
		double current_time_cost = 0;
		while(true)
		{
			if( temp_hotspots.empty() )
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
			for(int ihotspot = 0; ihotspot < temp_hotspots.size(); ++ihotspot)
			{
				double min_length_increment = -1;
				int best_front = -1;
				for(int i = 0; i < size_waypoints; ++i)  //先寻找最小路径增量？？
				{
					double length_increment = route.getIncreDistance(i, temp_hotspots[ihotspot]);
					if(length_increment < min_length_increment
						|| min_length_increment < 0)
					{
						min_length_increment = length_increment;
						best_front = i;
					}
				}

				time_increment = getTimeIncreForInsertion(now, route, best_front, temp_hotspots[ihotspot]);
				vector<int> temp_nodes = route.getCoveredNodes();
				AddToListUniquely(temp_nodes, temp_hotspots[ihotspot]->getCoveredNodes());
				sum_generationRate = getSumDataRate(temp_nodes);
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
			if( new_buffer > CHARMANode::getCapacityBuffer()
				&& route.getNWayPoints() == 1)
			{
				//throw string("HAR::HotspotClassification() : A single hotspot's buffer expection > BUFFER_CAPACITY_MA");

				route.AddHotspot(max_front, temp_hotspots[max_hotspot]);
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
				route.AddHotspot(max_front, temp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin() + max_hotspot;
				temp_hotspots.erase(ihotspot);
			}
		}
		newRoutes.push_back(route);
	}
	//将得到的新的hotspot class放入sink的route列表
	setNewRoutes(newRoutes);
}


void HAR::MANodeRouteDesign(int now)
{
	if( !( now % getConfig<int>("hs", "slot_hotspot_update") == 0
		  && now >= getConfig<int>("hs", "starttime_hospot_select") ) )
		return;

	vector<CHARRoute> routes = getNewRoutes();
	//对每个分类的路线用最近邻居算法进行优化
	for(vector<CHARRoute>::iterator iroute = routes.begin(); iroute != routes.end(); ++iroute)
	{
		OptimizeRoute( *iroute );
	}
	m_routes = routes;
	setNewRoutes(routes);

	//通知当前的所有MA路线已过期，立即返回
	for(auto iMANode = busyMAs.begin(); iMANode != busyMAs.end(); ++iMANode)
	{
		(*iMANode)->setRouteOverdue(true);
		( *iMANode )->setReturnAtOnce(true);
	}

	if( allMAs.size() < m_routes.size() )
	{
		int num_newMANodes = m_routes.size() - allMAs.size();
		for( int i = 0; i < num_newMANodes; ++i )
		{
			newMANode(popRoute(), now);
		}
	}

	CPrintHelper::PrintAttribute("MA", busyMAs.size());
}

inline bool HAR::transmitFrame(CGeneralNode & src, CFrame * frame, int now)
{
	vector<CGeneralNode*> neighbors = findNeighbors(src);
	return CMacProtocol::transmitFrame(src, frame, now, findNeighbors, receivePackets);
}

vector<CPacket*> HAR::receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;

	if( typeid( gToNode ) == typeid( CSink ) )
	{
		CSink* toSink = dynamic_cast< CSink* >( &gToNode );

		/*********************************************** Sink <- MA *******************************************************/

		if( typeid( gFromNode ) == typeid( CHARMANode ) )
		{
			CHARMANode* fromMA = dynamic_cast<CHARMANode*>( &gFromNode );
			packetsToSend = HAR::receivePackets(toSink, fromMA, packets, now);
		}
	}

	else if( typeid( gToNode ) == typeid( CHARMANode ) )
	{
		CHARMANode* toMA = dynamic_cast<CHARMANode*>( &gToNode );

		/************************************************ MA <- sink *******************************************************/

		if( typeid( gFromNode ) == typeid( CSink ) )
		{
			CSink* fromSink = dynamic_cast< CSink* >( &gFromNode );
			packetsToSend = HAR::receivePackets(toMA, fromSink, packets, now);
		}

		/************************************************ MA <- node *******************************************************/

		else if( typeid( gFromNode ) == typeid( CNode ) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( &gFromNode );
			packetsToSend = HAR::receivePackets(toMA, fromNode, packets, now);
		}
	}

	else if( typeid( gToNode ) == typeid( CNode ) )
	{
		CNode* node = dynamic_cast<CNode*>( &gToNode );

		/************************************************ Node <- MA *******************************************************/

		if( typeid( gFromNode ) == typeid( CHARMANode ) )
		{
			CHARMANode* fromMA = dynamic_cast<CHARMANode*>( &gFromNode );
			packetsToSend = HAR::receivePackets(node, fromMA, packets, now);
		}
	}

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

			//ACK（如果收到的数据全部被丢弃，发送空的ACK）
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
	vector<CData> dataToSend;  //空vector代表代表缓存为空

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
				ctrlToSend = new CCtrl(ma->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);
				// + DATA
				dataToSend = getDataForTrans(ma, 0, true);

				// TODO: mark skipRTS ?
				// TODO: connection established ?
				break;

			case CCtrl::_cts:

				if( !ma->hasData() )
				{
					return packetsToSend;
				}
				// + DATA
				dataToSend = getDataForTrans(ma, 0, true);

				break;

			case CCtrl::_capacity:

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_no_data:

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				if( ma->ifReturnAtOnce() )
					atMAReturn(ma, time);

				//收到空的ACK时，结束本次数据传输
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					ma->checkDataByAck( ctrl->getACK() );

				CPrintHelper::PrintCommunication(time, ma->format(), fromSink->format(), ctrl->getACK().size());

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
	vector<CData> dataToSend;  //空vector代表代表缓存为空
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
				dataToSend = getDataForTrans(node, 0, true);

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
						 && capacity < getConfig<int>("node", "buffer") 
						 && capacity < dataToSend.size() )
					CNode::removeDataByCapacity(dataToSend, capacity, false);

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_no_data:

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//收到空的ACK时，结束本次数据传输
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					node->checkDataByAck( ctrl->getACK() );

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

			//ACK（如果收到的数据全部被丢弃，发送空的ACK）
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


	/***************************************** 路由协议的通用输出 *********************************************/

	CRoutingProtocol::PrintInfo(now);

	/***************************************** 热点选取的相关输出 *********************************************/

	CHotspotSelect::PrintInfo(now);

	if( ! ( now >= getConfig<int>("hs", "starttime_hospot_select") ) )
		return;

	/**************************************** HAR 路由的补充输出 *********************************************/

	//hotspot选取结果、hotspot class数目、ED、Energy Consumption、MA节点buffer状态 ...

	if( now % getConfig<int>("hs", "slot_hotspot_update") == 0
	    || now == getConfig<int>("simulation", "runtime") )
	{
		//MA节点个数
		ofstream ma(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma"), ios::app);
		if( now == getConfig<int>("hs", "starttime_hospot_select") )
		{
			ma << endl << getConfig<string>("log", "info_log") << endl;
			ma << getConfig<string>("log", "info_ma") << endl;
		}
		ma << now << TAB << m_routes.size() << TAB << ( double(m_hotspots.size()) / double(m_routes.size()) ) << endl;
		ma.close();

		//
		ofstream ma_route(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma_route"), ios::app);
		if( now == getConfig<int>("hs", "starttime_hospot_select") )
		{
			ma_route << endl << getConfig<string>("log", "info_log") << endl;
			ma_route << getConfig<string>("log", "info_ma_route") << endl;
		}
		for( vector<CHARRoute>::iterator iroute = m_routes.begin(); iroute != m_routes.end(); iroute++ )
		{
			ma_route << now << TAB << iroute->format() << endl;
		}
		ma_route.close();			

		//用于计算MA节点个数的历史平均值信息
		SUM_MA_COST += m_routes.size();
		++COUNT_MA_COST;
		//用于计算MA路点（热点）平均个数的历史平均值信息
		SUM_WAYPOINT_PER_MA += double(m_hotspots.size()) / double(m_routes.size());
		++COUNT_WAYPOINT_PER_MA;

		//ED即平均投递延迟的理论值
		ofstream ed(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ed"), ios::app);
		if( now == getConfig<int>("hs", "starttime_hospot_select") )
		{
			ed << endl << getConfig<string>("log", "info_log") << endl;
			ed << getConfig<string>("log", "info_ed") << endl;
		}
		ed << now << TAB << calculateEDTime(now) << endl;
		ed.close();

		//热点质量、投递计数等统计信息
		ofstream hotspot_statistics(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_statistics"), ios::app);
		if( now == getConfig<int>("hs", "starttime_hospot_select") )
		{
			hotspot_statistics << endl << getConfig<string>("log", "info_log") << endl;
			hotspot_statistics << getConfig<string>("log", "info_hotspot_statistics") << endl;
		}
		//在 t 被时刻选出的热点，工作周期截至到 t + 900，在 t + 1800 时刻才被统计输出
		vector<int> timesToPrint;
		int timeBeforeYesterday = 0;
		//运行结束，补充输出上一轮的热点统计
		if( now == getConfig<int>("simulation", "runtime") )
		{
			timeBeforeYesterday = ( now / getConfig<int>("hs", "slot_hotspot_update") - 1 ) * getConfig<int>("hs", "slot_hotspot_update");
			int timeYesterday = timeBeforeYesterday + getConfig<int>("hs", "slot_hotspot_update");
			if( timeBeforeYesterday >= getConfig<int>("hs", "starttime_hospot_select") )
				timesToPrint.push_back(timeBeforeYesterday);
			if( timeYesterday >= getConfig<int>("hs", "starttime_hospot_select") )
				timesToPrint.push_back(timeYesterday);
		}
		else if( now % getConfig<int>("hs", "slot_hotspot_update") == 0 )
		{
			timeBeforeYesterday = now - 2 * getConfig<int>("hs", "slot_hotspot_update");
			if( timeBeforeYesterday >= getConfig<int>("hs", "starttime_hospot_select") )
				timesToPrint.push_back(timeBeforeYesterday);
		}
		if( ! timesToPrint.empty() )
		{
			for( vector<int>::iterator itime = timesToPrint.begin(); itime != timesToPrint.end(); itime++ )
			{
				vector<CHotspot *> hotspotsToPrint = CHotspot::getSelectedHotspots(*itime);
				hotspotsToPrint = CSortHelper::mergeSort(hotspotsToPrint, CSortHelper::descendByCountDelivery);
				for( vector<CHotspot *>::iterator it = hotspotsToPrint.begin(); it != hotspotsToPrint.end(); ++it )
					hotspot_statistics << *itime << '-' << *itime + getConfig<int>("hs", "slot_hotspot_update") << TAB
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
		//每个MA的当前buffer状态
		ofstream buffer_ma( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer_ma"), ios::app);
		if(now == getConfig<int>("hs", "starttime_hospot_select"))
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

	//最终final输出（补充）
	ofstream final( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_final"), ios::app);
	final << getAverageMACost() << TAB ;
	final << CData::getPercentDeliveryAtWaypoint() << TAB ;
	final.close();

	CHotspotSelect::PrintFinal(now);
}

//void HAR::DecayPositionsWithoutDeliveryCount(int now)
//{
//	if(now == 0)
//		return ;
//	vector<CPosition*> badPositions;
//	if( CHotspot::oldSelectedHotspots.empty() )
//		return ;
//
//	for(vector<CHotspot*>::iterator ihotspot = CHotspot::oldSelectedHotspots.begin(); ihotspot != CHotspot::oldSelectedHotspots.end(); )
//	{
//		if( (*ihotspot)->getCountDelivery(now) == 0 )
//		{
//			AddToListUniquely( badPositions, (*ihotspot)->getCoveredPositions() );
//			//free(*ihotspot);
//			//在mHAR中，应该考虑是否将这些热点排除在归并之外
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
//			//如果权值低于最小值，直接删除，MIN_POSITION_WEIGHT默认值为1，即不会删除任何position
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

bool HAR::Init()
{
	return true;
}

bool HAR::Operate(int now)
{
	bool hasNodes = true;
	// 不允许 xHAR 使用 HDC 作为 MAC 协议
	//if( config.MAC_PROTOCOL == config::_hdc )
	//	hasNodes = CHDC::Prepare(now);
	//else 
	if( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac )
		hasNodes = CSMac::Prepare(now);

	if( ! hasNodes )
		return false;

	HotspotClassification(now);

	MANodeRouteDesign(now);

	UpdateMANodeStatus(now);

	// 不允许 xHAR 使用 HDC 作为 MAC 协议
	//if( config.MAC_PROTOCOL == config::_hdc )
	//	CHDC::Operate(now);
	//else 
	CommunicateWithNeighbor(now);

	PrintInfo(now);

	return true;
}

