#include "HAR.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "NodeRepair.h"
#include "Configuration.h"
#include "PrintHelper.h"
#include "Sink.h"
#include "MANode.h"
#include "HDC.h"
#include "SMac.h"

map<int, double> HAR::mapDataCountRates;

vector<CHarMANode *> HAR::allMAs;
vector<CHarMANode *> HAR::busyMAs;
vector<CHarMANode *> HAR::freeMAs;

void CHarMANode::updateRoute(CHarRoute * route, int now)
{
	CMANode::updateRoute(route);
	CPrintHelper::PrintBrief(now, this->getName() + " is assigned with route " + route->toString() + ".");
}

void CHarMANode::updateStatus(int now)
{
	if( this->time < 0 )
		this->time = now;

	/********************************* Vacant MA ************************************/

	if( !this->isBusy() )
	{
		if( !CBasicEntity::withinRange(*this, *CSink::getSink(), getConfig<int>("trans", "range")) )
			throw string("CHarMANode::updateStatus(): " + this->getName() + " is free but not around Sink.");

		this->setTime(now);
		return;
	}

	/********************************* Busy MA ************************************/

	int duration = now - this->time;

	//updateTimerOccupied(time);

	//if route has expired, return to sink
	if( !this->isReturningToSink() 
	   && this->routeHasExpired(now) )
	{
		CPrintHelper::PrintDetail(time, this->getName() + " is returning to Sink due to route expiration.", 2);
		this->setReturningToSink();
	}

	//如果缓存已满，立即返回sink
	if( !this->isReturningToSink()
	   && this->isFull() )
	{
		CPrintHelper::PrintDetail(time, this->getName() + " is returning to Sink due to buffer filled.", 2);
		this->setReturningToSink();
	}

	/********************* Returning to Sink **********************/

	if( isReturningToSink() )
	{
		this->moveToward(*CSink::getSink(), duration, this->getSpeed());
		this->setTime(now);
		return;
	}

	if( isReturningToSink() )
	{
		this->moveToward(*CSink::getSink(), duration, this->getSpeed());
		this->setTime(now);
		return;
	}

	/********************* Moving on the Route **********************/

	/***************** Waiting ****************/

	int timeLeftAfterWaiting = 0;
	if( this->isWaiting() )
	{
		CHotspot* atHotspot = getAtHotspot();
		int copyWaitingWindow = this->waitingWindow;
		timeLeftAfterWaiting = this->wait(duration);

		//如果等待已经结束，记录一次等待
		if( !this->isWaiting() )
		{
			if( atHotspot == nullptr )
				throw string("CHarMANode::updateStatus(): atHotspot = nullptr");

			int timeStartWaiting = now - timeLeftAfterWaiting - copyWaitingWindow;
			atHotspot->recordWaitingTime(timeStartWaiting, copyWaitingWindow);
			CPrintHelper::PrintDetail(this->time + duration - timeLeftAfterWaiting
									  , this->getName() + " has waited at " + atHotspot->toString() 
									  + " for " + STRING(copyWaitingWindow) + "s.", 3);
		}
	}
	else
	{
		timeLeftAfterWaiting = duration;
	}

	if( timeLeftAfterWaiting == 0 )
	{
		this->setTime(now);
		return;
	}

	/***************** Actually Moving ****************/

	this->setTime(now - timeLeftAfterWaiting);  //将时间置于等待结束，移动即将开始的时间点
	atPoint = nullptr;  //离开之前的路点

	CBasicEntity *toPoint = route->getToPoint();
	int timeLeftAfterArrival = this->moveToward(*toPoint, timeLeftAfterWaiting, this->getSpeed());

	if( timeLeftAfterArrival >= 0 )
	{

		CSink *psink = nullptr;

		//若目的地的类型是 hotspot
		CHotspot *photspot = nullptr;
		//FIXME: nullptr ?
		if( ( photspot = dynamic_cast< CHotspot * >( toPoint ) ) != nullptr )
		{
			this->atPoint = toPoint;

			CPrintHelper::PrintDetail(time, this->getName() + " arrives at waypoint " + photspot->toString() + ".", 2);
			
			//set waiting time
			this->setWaiting(HAR::calculateWaitingTime(now, photspot));
		}
		//若目的地的类型是 sink
		else if( ( psink = dynamic_cast<CSink *>( toPoint ) ) != nullptr )
		{
			CPrintHelper::PrintDetail(time, this->getName() + " has returned to Sink.", 2);
			setReturningToSink();
		}
	}
	return;

}


vector<CHotspot *> HAR::hotspots;
vector<CHarRoute *> HAR::maRoutes;
vector<CHarRoute *> HAR::oldRoutes;
int HAR::indexRoute = 0;
int HAR::SUM_MA_COST = 0;
int HAR::COUNT_MA_COST = 0;
double HAR::SUM_WAYPOINT_PER_MA = 0;
int HAR::COUNT_WAYPOINT_PER_MA = 0;


bool HAR::newMANode(int now)
{
	if( allMAs.size() >= CMANode::MAX_NUM_MA )
		return false;
	else
	{
		CHarMANode* ma = new CHarMANode(now);
		allMAs.push_back(ma);
		freeMAs.push_back(ma);
		CPrintHelper::PrintBrief(ma->getName() + " is created. (" + STRING(allMAs.size()) + " in total)");
		return true;
	}
}

bool HAR::newMANode(int n, int now)
{
	for( int i = 0; i < n; ++i )
	{
		if( !newMANode(now) )
			return false;
	}
	return true;
}

void HAR::initNodeInfo()
{
	for( CNode* pnode : CNode::getAllNodes() )
		mapDataCountRates[pnode->getID()] = pnode->getDataCountRate();
}


//取得新的路线集合

void HAR::initMANodes(int now)
{
	newMANode(CMANode::INIT_NUM_MA, now);

	freeMAs = allMAs;
}

void HAR::turnFree(CHarMANode * ma)
{
	ma->endRoute();
	ma->setBusy(false);
	AddToListUniquely(freeMAs, ma);
	RemoveFromList(busyMAs, ma);
}

void HAR::turnBusy(CHarMANode * ma)
{
	ma->setBusy(true);
	RemoveFromList(freeMAs, ma);
	AddToListUniquely(busyMAs, ma);
}

void HAR::updateRoutes(vector<CHarRoute*> newRoutes)
{
	oldRoutes.insert(oldRoutes.end(), maRoutes.begin(), maRoutes.end());
	maRoutes = newRoutes;
	indexRoute = 0;
}

vector<CHarRoute*> HAR::getRoutes()
{
	return maRoutes;
}

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

double HAR::getSumDataRate(vector<int> nodes)
{
	double sum = 0;
	for(int nodeId: nodes)
	{
		sum += mapDataCountRates[nodeId];
	}
	return sum;
}

double HAR::getTimeIncreForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot)
{
	double result = calculateWaitingTime(now, hotspot) + ( route.getIncreDistance(front, hotspot) / CMANode::getMASpeed() );
	return result;
}

double HAR::calculateRatioForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot)
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

	for(vector<CHarRoute*>::iterator iroute = maRoutes.begin(); iroute != maRoutes.end(); ++iroute)
		sum_length += (*iroute)->getLength();
	avg_length = sum_length / hotspots.size() + 1;
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
	{
		sum_waitingTime += calculateWaitingTime(now, *ihotspot);
		sum_pm += exp( -1 / (*ihotspot)->getHeat() );
	}
	avg_waitingTime = sum_waitingTime / hotspots.size();
	avg_u = avg_length / CMANode::getMASpeed() + avg_waitingTime;
	avg_pw = sum_pm / hotspots.size();
	pmh = sum_waitingTime / (sum_length / CMANode::getMASpeed() + sum_waitingTime);
	EM = (1 - pmh) * ( ( (1 - avg_pw) / avg_pw) * avg_u + (avg_length / (2 * CMANode::getMASpeed()) + avg_waitingTime) ) + pmh * ( ( (1 - avg_pw) / avg_pw) * avg_u + avg_waitingTime);
	EIM = avg_u / avg_pw;
	ED = EM + ( (1 - getConfig<double>("trans", "probability")) / getConfig<double>("trans", "probability") ) * EIM + ( double( hotspots.size() ) / (2 * maRoutes.size()) ) * avg_u;

	return ED;
}


void HAR::OptimizeRoute(CHarRoute *route)
{
	vector<pair<CBasicEntity *, int>> waypoints = route->getWayPoints();
	CBasicEntity *current = CSink::getSink();
	CHarRoute result(route->getTimeCreation(), route->getTimeExpiration());
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
	
	vector<CHarRoute*> newRoutes;
	
	while(! temp_hotspots.empty())
	{
		//构造一个hotspot class
		CHarRoute* route = new CHarRoute(now, now + CHotspotSelect::SLOT_HOTSPOT_UPDATE);
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
			int size_waypoints = route->getNWayPoints();
			//遍历所有剩余hotspot，选择ratio最大的hotspot添加
			for(int ihotspot = 0; ihotspot < temp_hotspots.size(); ++ihotspot)
			{
				double min_length_increment = -1;
				int best_front = -1;
				for(int i = 0; i < size_waypoints; ++i)  //先寻找最小路径增量？？
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
			if( new_buffer > CHarMANode::getCapacityBuffer()
				&& route->getNWayPoints() == 1)
			{
				//throw string("HAR::HotspotClassification() : A single hotspot's buffer expection > BUFFER_CAPACITY_MA");

				route->AddHotspot(max_front, temp_hotspots[max_hotspot]);
				vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin() + max_hotspot;
				temp_hotspots.erase(ihotspot);
				break;
			}
			if( new_buffer > CHarMANode::getCapacityBuffer() )
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
		route->initToPoint();
		newRoutes.push_back(route);
	}
	//将得到的新的hotspot class放入sink的route列表
	updateRoutes(newRoutes);
}


void HAR::MANodeRouteDesign(int now)
{
	if( !( now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
		  && now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT ) )
		return;

	vector<CHarRoute*> routes = getRoutes();
	//对每个分类的路线用最近邻居算法进行优化
	for(vector<CHarRoute*>::iterator iroute = routes.begin(); iroute != routes.end(); ++iroute)
	{
		OptimizeRoute( *iroute );
	}

	CPrintHelper::PrintAttribute("MA Routes", routes.size());
}

//必须先调用hasRoutes判断

CHarRoute * HAR::popRoute()
{
	CHarRoute route = *maRoutes[indexRoute];
	indexRoute = ( indexRoute + 1 ) % maRoutes.size();
	return new CHarRoute(route);
}

void HAR::atMAReturn(CHarMANode * ma, int now)
{
	if( !CBasicEntity::withinRange(*ma, *CSink::getSink(), getConfig<int>("trans", "range")) )
		throw string("CHarMANode::atMAReturn(): " + ma->getName() + " is not around Sink.");

	if( hasRoutes() )
		ma->updateRoute(HAR::popRoute(), now);
	else
	{
		turnFree(ma);
	}
}

double HAR::getSumEnergyConsumption()
{
	double sumEnergyConsumption = 0;
	for( auto iMANode = busyMAs.begin(); iMANode != busyMAs.end(); ++iMANode )
		sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();
	for( auto iMANode = freeMAs.begin(); iMANode != freeMAs.end(); ++iMANode )
		sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();

	return sumEnergyConsumption;
}

//根据时间和热点，计算等待时间

double HAR::calculateWaitingTime(int now, CHotspot * hotspot)
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
		if(getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved)
		{
			temp_time = min(now, getConfig<int>("ihs", "lifetime_position"));
		}

		nCoveredPositionsForNode.push_back(hotspot->getNCoveredPositionsForNode(coveredNodes[i]));
		temp = double(hotspot->getNCoveredPositionsForNode(coveredNodes[i])) / double(temp_time + CHotspotSelect::SLOT_HOTSPOT_UPDATE);

		//merge-HAR: ratio
		temp *= pow(hotspot->getRatioByTypeHotspotCandidate(), hotspot->getAge());

		if(temp >= getConfig<double>("har", "beta"))
		{
			result *= temp;
			++count_trueHotspot;
		}
	}
	//FIXME: 如果不是true hotspot，waiting time为0
	if(count_trueHotspot == 0)
		return getConfig<int>("har", "min_waiting_time");
	double prob = exp(-1 / hotspot->getHeat());
	result = prob / result;
	result = pow(result, ( 1 / double(count_trueHotspot) ));

	return result + getConfig<int>("har", "min_waiting_time");
}


//更新所有 MA 的坐标、等待时间
//注意：必须在新一轮热点选取之后调用

void HAR::UpdateMANodeStatus(int now)
{
	//为空闲的MA分配路线
	while( !freeMAs.empty() )
	{
		CHarMANode *ma = freeMAs.front();
		if( !hasRoutes() )
			break;
		ma->updateRoute(popRoute(), now);
		turnBusy(ma);
	}

	for( CHarMANode *pMA : allMAs )
		pMA->updateStatus(now);
}

vector<CGeneralNode*> HAR::findNeighbors(CGeneralNode & src)
{
	vector<CGeneralNode*> neighbors = CMacProtocol::findNeighbors(src);

	/**************************************************** MA ***********************************************************/

	for( vector<CHarMANode*>::iterator iMA = busyMAs.begin(); iMA != busyMAs.end(); ++iMA )
	{
		//skip itself
		if( ( *iMA )->getID() == src.getID() )
			continue;

		if( CBasicEntity::withinRange(src, **iMA, getConfig<int>("trans", "range"))
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

		if(typeid( gFromNode ) == typeid( CHarMANode ))
		{
			CHarMANode* fromMA = dynamic_cast< CHarMANode* >( &gFromNode );
			packetsToSend = HAR::receivePackets(toSink, fromMA, packets, now);
		}
	}

	else if(typeid( gToNode ) == typeid( CHarMANode ))
	{
		CHarMANode* toMA = dynamic_cast< CHarMANode* >( &gToNode );

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

		if(typeid( gFromNode ) == typeid( CHarMANode ))
		{
			CHarMANode* fromMA = dynamic_cast< CHarMANode* >( &gFromNode );
			packetsToSend = HAR::receivePackets(node, fromMA, packets, now);
		}
	}
	else
		throw string("HAR::receivePackets(): Unexpected condition.");

	// TODO: + comm : node <--> sink ?

	return packetsToSend;
}

vector<CPacket*> HAR::receivePackets(CSink* sink, CHarMANode* fromMA, vector<CPacket*> packets, int time)
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

vector<CPacket*> HAR::receivePackets(CHarMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time)
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
					if( ma->isReturningToSink() )
						atMAReturn(ma, time);
					return packetsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(ma->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);
				// + DATA
				dataToSend = ma->getDataForTrans(INVALID);

				// TODO: mark skipRTS ?
				// TODO: connection established ?
				break;

			case CCtrl::_cts:

				if( !ma->hasData() )
				{
					if( ma->isReturningToSink() )
						atMAReturn(ma, time);
					return packetsToSend;
				}
				// + DATA
				dataToSend = ma->getDataForTrans(INVALID);

				break;

			case CCtrl::_capacity:

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_no_data:

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//收到空的ACK时，结束本次数据传输
				if( ctrl->getACK().empty() )
				{
					if( ma->isReturningToSink() )
						atMAReturn(ma, time);
					return packetsToSend;
				}
				//clear data with ack
				else
				{
					ma->dropDataByAck(ctrl->getACK());

					CPrintHelper::PrintCommunication(time, ma->toString(), fromSink->toString(), ctrl->getACK().size());
					if( ma->isReturningToSink() )
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

vector<CPacket*> HAR::receivePackets(CNode* node, CHarMANode* fromMA, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	vector<CData> dataToSend;  //空vector代表代表缓存为空
	int capacity = INVALID;

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

				break;

			case CCtrl::_cts:

				break;

				/************************************* rcv capacity **********************************/

			case CCtrl::_capacity:

				capacity = ctrl->getCapacity();

				// + DATA
				if( capacity == 0 )
					return packetsToSend;
				else
					dataToSend = node->getDataForTrans(capacity);

				if( dataToSend.empty() )
					return packetsToSend;

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
					node->dropDataByAck( ctrl->getACK() );

				CPrintHelper::PrintCommunication(time, node->toString(), fromMA->toString(), ctrl->getACK().size());

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

vector<CPacket*> HAR::receivePackets(CHarMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time)
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

void HAR::CommunicateBetweenNeighbors(int now)
{
	if( now % getConfig<int>("log", "slot_log") == 0 )
	{
		if( now > 0 )
			CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());

		CPrintHelper::PrintNewLine();
		CPrintHelper::PrintHeading(now, "DATA DELIVERY");
	}

	// TODO: sink receive RTS / send by slot ?
	// xHAR: sink => MAs
	CSink* sink = CSink::getSink();
	transmitFrame(*sink, sink->sendRTS(now), now);

	vector<CHarMANode*> MAs = allMAs;
	// xHAR: MAs => nodes
	for( CHarMANode* pMA : busyMAs )
	{
		// skip discover if buffer is full
		if( pMA->isBusy()
		   && pMA->getBufferVacancy() > 0 )
			transmitFrame(*pMA, pMA->sendRTSWithCapacity(now), now);
	}

	// xHAR: no forward between nodes
}

void HAR::PrintInfo(int now)
{
	if(!( now % getConfig<int>("log", "slot_log") == 0
		 || now == getConfig<int>("simulation", "runtime") ))
		return;


	/***************************************** 路由协议的通用输出 *********************************************/

	CRoutingProtocol::PrintInfo(now);

	/***************************************** 热点选取的相关输出 *********************************************/

	CHotspotSelect::PrintInfo(now);

	if(!( now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT ))
		return;

	/**************************************** HAR 路由的补充输出 *********************************************/

	//hotspot选取结果、hotspot class数目、ED、Energy Consumption、MA节点buffer状态 ...

	if(now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
	   || now == getConfig<int>("simulation", "runtime"))
	{
		//MA节点个数
		ofstream ma(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma"), ios::app);
		if(now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
		{
			ma << endl << getConfig<string>("log", "info_log") << endl;
			ma << getConfig<string>("log", "info_ma") << endl;
		}
		ma << now << TAB << maRoutes.size() << TAB << ( double(hotspots.size()) / double(maRoutes.size()) ) << endl;
		ma.close();

		//
		ofstream ma_route(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma_route"), ios::app);
		if(now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
		{
			ma_route << endl << getConfig<string>("log", "info_log") << endl;
			ma_route << getConfig<string>("log", "info_ma_route") << endl;
		}
		for(vector<CHarRoute*>::iterator iroute = maRoutes.begin(); iroute != maRoutes.end(); iroute++)
		{
			ma_route << now << TAB << ( *iroute )->toString() << endl;
		}
		ma_route.close();

		//用于计算MA节点个数的历史平均值信息
		SUM_MA_COST += maRoutes.size();
		++COUNT_MA_COST;
		//用于计算MA路点（热点）平均个数的历史平均值信息
		SUM_WAYPOINT_PER_MA += double(hotspots.size()) / double(maRoutes.size());
		++COUNT_WAYPOINT_PER_MA;

		//ED即平均投递延迟的理论值
		ofstream ed(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ed"), ios::app);
		if(now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
		{
			ed << endl << getConfig<string>("log", "info_log") << endl;
			ed << getConfig<string>("log", "info_ed") << endl;
		}
		ed << now << TAB << calculateEDTime(now) << endl;
		ed.close();

		//热点质量、投递计数等统计信息
		ofstream hotspot_statistics(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_statistics"), ios::app);
		if(now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
		{
			hotspot_statistics << endl << getConfig<string>("log", "info_log") << endl;
			hotspot_statistics << getConfig<string>("log", "info_hotspot_statistics") << endl;
		}
		//在 t 被时刻选出的热点，工作周期截至到 t + 900，在 t + 1800 时刻才被统计输出
		vector<int> timesToPrint;
		int timeBeforeYesterday = 0;
		//运行结束，补充输出上一轮的热点统计
		if(now == getConfig<int>("simulation", "runtime"))
		{
			timeBeforeYesterday = ( now / CHotspotSelect::SLOT_HOTSPOT_UPDATE - 1 ) * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
			int timeYesterday = timeBeforeYesterday + CHotspotSelect::SLOT_HOTSPOT_UPDATE;
			if(timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
				timesToPrint.push_back(timeBeforeYesterday);
			if(timeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
				timesToPrint.push_back(timeYesterday);
		}
		else if(now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0)
		{
			timeBeforeYesterday = now - 2 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
			if(timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
				timesToPrint.push_back(timeBeforeYesterday);
		}
		if(!timesToPrint.empty())
		{
			for(vector<int>::iterator itime = timesToPrint.begin(); itime != timesToPrint.end(); itime++)
			{
				vector<CHotspot *> hotspotsToPrint = CHotspotSelect::getSelectedHotspots(*itime);
				hotspotsToPrint = CSortHelper::mergeSort(hotspotsToPrint, CSortHelper::descendByCountDelivery);
				for(vector<CHotspot *>::iterator it = hotspotsToPrint.begin(); it != hotspotsToPrint.end(); ++it)
					hotspot_statistics << *itime << '-' << *itime + CHotspotSelect::SLOT_HOTSPOT_UPDATE << TAB
					<< ( *it )->getID() << TAB << ( *it )->toString() << TAB << ( *it )->getNCoveredPosition() << "," << ( *it )->getNCoveredNodes() << TAB
					<< ( *it )->getRatio() << TAB << ( *it )->getWaitingTimesString(true) << TAB << ( *it )->getCountDelivery(*itime) << endl;
			}
		}
		hotspot_statistics.close();
	}

	//MA Buffer
	if(now % getConfig<int>("log", "slot_log") == 0
	   || now == getConfig<int>("simulation", "runtime"))
	{
		//每个MA的当前buffer状态
		ofstream buffer_ma(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer_ma"), ios::app);
		if(now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT)
		{
			buffer_ma << endl << getConfig<string>("log", "info_log") << endl;
			buffer_ma << getConfig<string>("log", "info_buffer_ma") << endl;
		}
		buffer_ma << now << TAB;
		for(auto iMA = busyMAs.begin(); iMA != busyMAs.end(); ++iMA)
			buffer_ma << ( *iMA )->getBufferSize() << TAB;
		buffer_ma << endl;
		buffer_ma.close();

	}

}

void HAR::PrintFinal(int now)
{
	CRoutingProtocol::PrintFinal(now);

	CHotspotSelect::PrintFinal(now);
}

bool HAR::Init(int now)
{
	CMANode::Init();
	CHotspotSelect::Init();

	initNodeInfo();
	return true;
}

bool HAR::Operate(int now)
{
	if( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac )
		CSMac::Prepare(now);
	// 不允许 xHAR 使用 HDC 作为 MAC 协议
	else
		throw string("HAR::Operate(): Only SMac is allowed as MAC protocol for HAR.");

	if( !CNode::UpdateNodeStatus(now) )
		return false;

	CHotspotSelect::RemovePositionsForDeadNodes(CNode::getIdNodes(CNode::getDeadNodes()), now);
	CHotspotSelect::CollectNewPositions(now, CNode::getAliveNodes());

	hotspots = CHotspotSelect::HotspotSelect(CNode::getIdNodes(CNode::getAliveNodes()), now);

	//检测节点所在热点区域
	CHotspot::UpdateAtHotspotForNodes(CNode::getAliveNodes(), hotspots, now);

	if( now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT
	   && now <= CHotspotSelect::STARTTIME_HOTSPOT_SELECT + getConfig<int>("simulation", "slot") )
		initMANodes(now);

	UpdateMANodeStatus(now);

	HotspotClassification(now);

	MANodeRouteDesign(now);

	// 不允许 xHAR 使用 HDC 作为 MAC 协议
	//if( config.MAC_PROTOCOL == config::_hdc )
	//	CHDC::Operate(now);
	//else 
	if( now < getConfig<int>("simulation", "runtime") )
		CommunicateBetweenNeighbors(now);

	PrintInfo(now);

	return true;
}

//将给定的元素放到waypoint列表的最后

void CHarRoute::AddHotspot(CBasicEntity * hotspot)
{
	AddWaypoint(hotspot, getConfig<int>("har", "min_waiting_time"));
	AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>( hotspot )->getCoveredNodes());
}

//将给定hotspot插入到路径中给定的位置

void CHarRoute::AddHotspot(int front, CBasicEntity * hotspot)
{
	AddWaypoint(front, hotspot, getConfig<int>("har", "min_waiting_time"));
	AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>( hotspot )->getCoveredNodes());
}
