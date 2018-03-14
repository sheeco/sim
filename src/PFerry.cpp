#include "PFerry.h"
#include "SortHelper.h"
#include "PrintHelper.h"
#include "FileHelper.h"


string CTracePrediction::KEYWORD_PREDICT;
string CTracePrediction::EXTENSION_PAN_FILE;


int CPFerryTask::COUNT_TASK_MET;
int CPFerryTask::COUNT_TASK;
map<int, int> CPFerryTask::countTaskForNodes;

bool CPFerryMANode::RETURN_ONCE_MET = false;

vector<CPFerryMANode*> CPFerry::allMAs;
vector<CPFerryMANode*> CPFerry::freeMAs;
vector<CPFerryMANode*> CPFerry::busyMAs;


vector<CNode*> CPFerry::targetNodes;
//vector<CNode*> CPFerry::missNodes;
vector<CNode*> CPFerry::candidateNodes; //untargetted nodes

map<int, map<int, CTracePrediction*>> CPFerry::predictions;
vector<int> CPFerry::strides;
map<int, CPFerry::CNodeRecord> CPFerry::collectionRecords;  //known latest collection time for nodes

string CPFerry::PATH_PREDICT;
config::EnumRoutingProtocolScheme CPFerry::PRESTAGE_PROTOCOL = config::_xhar;
int CPFerry::STARTTIME = INVALID;


CPFerryMANode::~CPFerryMANode()
{
	this->tasks.clear();
	for( pair<int, CPFerryTask*> p : tasks )
		FreePointer(p.second);
	FreePointerVector(this->taskHistory);
}

void CPFerryMANode::assignTask(CNode * node, CPosition location, int waitingTime, int now)
{
	this->endTask();

	CPFerryTask* task = new CPFerryTask(node, location);
	this->tasks[node->getID()] = task;
	if( !this->hasRoute() )
		this->route = new CRoute(CSink::getSink());
	this->route->AddWaypoint(new CPosition(location), waitingTime);
	this->route->initToPoint();
	this->setTime(now);
	this->setBusy(true);
}

void CPFerryMANode::endTask()
{
	for( pair<int, CPFerryTask*> p : tasks )
		this->taskHistory.push_back(p.second);
	this->tasks.clear();
	this->collections.clear();
	this->endRoute();
}

void CPFerryMANode::recordCollection(int nodeId, CCoordinate location, int bufferVacancy, int time)
{
	this->collections[nodeId] = CCollectionRecord(time, location, bufferVacancy);
	CPFerryTask* task = findTask(nodeId);
	if( task != nullptr )
	{
		task->setMet(true);

		if( RETURN_ONCE_MET )
		{
			CPrintHelper::PrintDetail(time, this->getName() + " is returning to Sink due to task met.", 2);
			this->setReturningToSink();
		}
	}
}

void CPFerryMANode::updateStatus(int now)
{
	if( this->time < 0 )
		this->time = now;

	/********************************* Vacant MA ************************************/
	
	if( !this->isBusy() )
	{
		if( !CBasicEntity::withinRange(*this, *CSink::getSink(), getConfig<int>("trans", "range_trans")) )
			throw string("CPFerryMANode::updateStatus(): " + this->getName() + " is free but not around Sink.");

		this->setTime(now);
		return;
	}

	/********************************* Busy MA ************************************/

	int duration = now - this->time;

	//updateTimerOccupied(time);

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

	/********************* Moving on the Route **********************/

	/***************** Waiting ****************/

	int timeLeftAfterWaiting = 0;
	if( this->isWaiting() )
	{
		CBasicEntity* waypoint = getAtWaypoint();
		int copyWaitingWindow = this->waitingWindow;
		timeLeftAfterWaiting = this->wait(duration);

		//如果等待已经结束，记录一次等待
		if( !this->isWaiting() )
		{
			if( waypoint == nullptr )
				throw string("CPFerryMANode::updateStatus(): waypoint = nullptr");

			CPrintHelper::PrintDetail(this->time + duration - timeLeftAfterWaiting
									  , this->getName() + " has waited at " + waypoint->toString() + " for " 
									  + STRING(copyWaitingWindow) + "s.", 3);
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
	int minWaitingTime = route->getWaitingTime();
	int timeLeftAfterArrival = this->moveToward(*toPoint, timeLeftAfterWaiting, this->getSpeed());

	//如果已到达目的地
	if( timeLeftAfterArrival >= 0 )
	{
		CSink *psink = nullptr;

		//若目的地的类型是waypoint
		CPosition *ppos = nullptr;
		if( ( ppos = dynamic_cast< CPosition * >( toPoint ) ) != nullptr )
		{
			this->atPoint = toPoint;

			CPrintHelper::PrintDetail(now, this->getName() + " arrives at task position " + ppos->toString() + ".", 2);

			CPFerryTask* task = this->findTask(ppos->getNode());
			if( !task )
				throw string("CPFerryMANode::updateStatus(): Cannot find task for node id " + STRING(ppos->getNode()) + ".");
			else
			{
				if(!RETURN_ONCE_MET
				   || !task->isMet())
				{
					//set waiting time
					int timePred = task->getTime();
					this->setWaiting(max(minWaitingTime, timePred - this->getTime() + minWaitingTime));
				}
			}
		}
		//若目的地的类型是 sink
		else if( ( psink = dynamic_cast< CSink * >( toPoint ) ) != nullptr )
		{
			CPrintHelper::PrintDetail(time, this->getName() + " has returned to Sink.", 2);
			this->setReturningToSink();
		}
	}
	return;
}


//init based on newly loaded CNode

void CPFerry::initNodeInfo(int now)
{
	vector<CNode*> nodes = CNode::getAliveNodes();
	for( CNode* pnode : nodes )
	{
		updatecollectionRecords(pnode->getID(), pnode->getBufferVacancy(), now);

	}
	candidateNodes = nodes;
}

void CPFerry::initMANodes()
{
	for( CHarMANode* harMA : HAR::getAllMAs() )
	{
		CPFerryMANode* ma = new CPFerryMANode(*harMA);

		CPrintHelper::PrintBrief(ma->getName() + " has switched to PFerry protocol.");

		allMAs.push_back(ma);
		if( ma->isBusy() )
		{
			ma->setReturningToSink();
			busyMAs.push_back(ma);
		}
		else
			freeMAs.push_back(ma);
	}
}

void CPFerry::initPredictions()
{
	strides.clear();
	predictions.clear();

	vector<CNode *> nodes = CNode::getAllNodes();
	string dir = PATH_PREDICT; //e.g. ../res/predict
	vector<string> subdirs = CFileHelper::ListDirectory(dir);
	int minStartTime = INVALID;

	if( subdirs.empty() )
		throw string("CPFerry::initPredictions(): Cannot find any trace files under \"" + dir + "\".");
	for( string subdir : subdirs )  //e.g. ../res/predict/1
	{
		if( CFileHelper::IsDirectory(subdir) )
		{
			string nameDir = CFileHelper::SplitPath(subdir).second;
			int stride = 0;
			try
			{
				stride = CParseHelper::ParseInt(nameDir);
			}
			catch( string error )
			{
				throw string("CPFerry::initPredictions(): Cannot parse directory name \"" + nameDir + "\" into int.");
			}
			strides.push_back(stride);
			predictions[stride] = map<int, CTracePrediction*>();
			for( CNode *pnode : nodes )
			{
				CTracePrediction *pPred = new CTracePrediction(pnode->getID(), pnode->getIdentifier(), subdir);

				double hitrate = CTracePrediction::calculateHitrate(pnode->getTrace(), *pPred, 100);
				CPrintHelper::PrintBrief("Trace prediction for " + pnode->getName()
										 + " from " + STRING(pPred->getStartTime()) + "s to "
										 + STRING(pPred->getEndTime()) + "s is loaded, hitrate "
										 + STRING(NDigitFloat(hitrate * 100, 1)) + "%.");

				int starttime = pPred->getStartTime();
				if( STARTTIME >= 0
				   && starttime > STARTTIME + Trace::getInterval() )
					throw string("CPFerry::initPredictions(): Range of predictions cannot reach given STARTTIME ("
								 + STRING(STARTTIME) + "s). ");

				if( minStartTime < 0
				   || starttime < minStartTime )
					minStartTime = starttime;
				predictions[stride][pnode->getID()] = pPred;
			}
		}
	}

	//Use configured value if provided, 
	//read from prediction files otherwise 
	if( STARTTIME < 0 )
		STARTTIME = minStartTime;
}

void CPFerry::updatecollectionRecords(int nodeId, int bufferVacancy, int time)
{
	if( collectionRecords.find(nodeId) == collectionRecords.end()
	   || collectionRecords[nodeId].timeCollect < time )
		collectionRecords[nodeId] = CNodeRecord{ time, bufferVacancy };
}

// 60 -> 90; 50 -> 60

int CPFerry::nextInterval(int now, int stride)
{
	int interval = Trace::getInterval();
	return ( now / interval + stride ) * interval;
}

bool CPFerry::hasPrediction(CNode * node, int time, int stride)
{
	CTracePrediction* prediction = predictions[stride][node->getID()];
	return prediction->isValid(nextInterval(time, stride));
}

CPosition CPFerry::getNextPrediction(CNode * node, int time, int stride)
{
	int timePred = nextInterval(time, stride);
	CTracePrediction* prediction = predictions[stride][node->getID()];

	return prediction->getPrediction(timePred);
}

bool CPFerry::UpdateNodeStatus(int now)
{
	if( !CNode::UpdateNodeStatus(now) )
		return false;

	return true;
}

bool CPFerry::updateMAStatus(int now)
{
	vector<CPFerryMANode*> mas = allMAs;
	for( CPFerryMANode* ma : mas )
	{
		ma->updateStatus(now);
	}
	return true;
}

void CPFerry::recordTargetMet(CNode * node)
{
	RemoveFromList(targetNodes, node);
	AddToListUniquely(candidateNodes, node);
	CPFerryTask::recordTaskMet(node->getID());
}

void CPFerry::recordTargetMiss(CNode * node)
{
	RemoveFromList(targetNodes, node);
	AddToListUniquely(candidateNodes, node);
	CPFerryTask::recordTaskMiss(node->getID());
}

void CPFerry::reportTask(CPFerryMANode * ma, int now)
{
	bool met = false;
	for( pair<int, CPFerryTask*> p : ma->getTasks() )
	{
		CPFerryTask* task = p.second;
		CNode* node = task->target;
		met = task->met;
		if( met )
			recordTargetMet(node);
		else
			recordTargetMiss(node);
	}

	map<int, CPFerryMANode::CCollectionRecord> collections = ma->getCollections();
	for( pair<int, CPFerryMANode::CCollectionRecord> collection : collections )
	{
		recordTargetMet(CNode::findNodeByID(collection.first));
		updatecollectionRecords(collection.first, collection.second.bufferVacancy, collection.second.time);
	}

	string message = ma->getName() + " returns with target ";
	if( met )
		message += "met";
	else
		message += "miss";
	message += " & " + STRING(collections.size()) + " collections.";
	CPrintHelper::PrintBrief(now, message);
}

bool CPFerry::dealWithUnreachableNodes(vector<pair<CNode*, CPosition>> nodes, int now)
{
	if( !nodes.empty() )
		CPrintHelper::PrintDetail(now, STRING(nodes.size()) + " nodes are unreachable.", 4);

	int i = 0;
	for( ; !freeMAs.empty() && i < nodes.size(); ++i )
	{
		CPFerryMANode* ma = freeMAs.front();
		CNode* node = nodes[i].first;
		CPosition pos = nodes[i].second;
		ma->assignTask(node, pos, minWaitingTime(), now);

		CPrintHelper::PrintBrief(now, ma->getName() + " is targetting (unreachable) " + node->getName()
								 + " around " + pos.toString()
								 + " at " + STRING(pos.getTime()) + "s.");

		RemoveFromList(freeMAs, ma);
		busyMAs.push_back(ma);
		RemoveFromList(candidateNodes, node);
		targetNodes.push_back(node);
		//missNodes.push_back(node);
	}

	return i > 0;
}

void CPFerry::atDataCollection(CPFerryMANode * ma, CNode * node, CCoordinate location, int time)
{
	ma->recordCollection(node->getID(), location, node->getBufferVacancy(), time);
}

void CPFerry::turnFree(CPFerryMANode * ma)
{
	ma->setBusy(false);
	AddToListUniquely(freeMAs, ma);
	RemoveFromList(busyMAs, ma);
}

void CPFerry::atMAReturn(CPFerryMANode * ma, int now)
{
	if( ma->hasTask() )
	{
		reportTask(ma, now);
		ma->endTask();
	}
	turnFree(ma);
}

double CPFerry::calculateMetric(CNode * node, CPosition prediction, int now)
{
#ifdef DEBUG
	static map<int, vector<pair<string, double>>> cache;

	if( cache.find(now) == cache.end()
	   && !cache.empty() )
	{
		cache.clear();
		cache[now] = vector<pair<string, double>>();
	}
#endif // DEBUG

	int datatime = getConfig<int>("simulation", "datatime");
	double timePrediction = prediction.getTime();
	int timeLastCollectoin = collectionRecords[node->getID()].timeCollect;
	int bufferVacancy = collectionRecords[node->getID()].bufferVacancy;
	int bufferCapacity = node->getCapacityBuffer();
	double dataRate = node->getDataCountRate();

	double timeForTravel = timePrediction - now;
	double timeTravel = CBasicEntity::getDistance(prediction, *CSink::getSink()) / CMANode::getMASpeed();
	double timeArrival = timeTravel + now;

	//double timeEstimate = min(timeArrival + now, timePrediction);
	double timeEstimate = timeArrival;
	int bufferEstimation = int(( min(timeEstimate, datatime) - timeLastCollectoin ) * dataRate
							   + bufferCapacity - bufferVacancy);
	int bufferOccupancy = min(bufferEstimation, bufferCapacity);
	int dataLoss = bufferEstimation - bufferOccupancy;
	//double timeOverflow = timeLastCollectoin + bufferVacancy / dataRate;

	//original metric

	//if( timeArrival + now > timePrediction )
	//	return -timeOverflow;
	//else
	//{
	//	if( timeOverflow >= now )
	//	//if( timeOverflow >= timePrediction )
	//		return timeArrival / max(1, timeOverflow - now);
	//		//return 1 / timeArrival + 1 / max(1, timeOverflow - now);
	//	else
	//		return (now - timeOverflow) * node->getDataCountRate();
	//		//return (timePrediction - timeOverflow) * node->getDataCountRate();
	//}

	//double dataMetric = dataLoss + ( double ) bufferOccupancy / bufferCapacity;
	double dataMetric = bufferOccupancy;
	//fix -0
	if( EQUAL(dataMetric, -0) )
		dataMetric = 0;

	if( timeArrival > timePrediction + Trace::getInterval() )
		dataMetric = bufferOccupancy;
	//dataMetric = -bufferEstimation;
	else
		dataMetric = bufferEstimation;

	double distanceMetric = timeTravel / timeForTravel;

	double metric = 0;
	metric = dataMetric;
	//metric = dataMetric + distanceMetric;

#ifdef DEBUG
	cache[now].push_back(pair<string, double>(node->getIdentifier(), metric));
#endif // DEBUG

	return metric;
}

pair<vector<pair<CNode*, CPosition>>, vector<pair<CNode*, CPosition>>> CPFerry::sortByPriority(vector<CNode*> nodes, int now)
{
	multimap<double, pair<CNode*, CPosition>> priNormal;
	multimap<double, pair<CNode*, CPosition>> priUnreachable;
	for( CNode* node : nodes )
	{
		CPosition prediction;
		bool reachable = false;
		double metric;
		for( int stride : strides )
		{
			if( !hasPrediction(node, now, stride) )
			{
				reachable = true;
				break;
			}
			prediction = getNextPrediction(node, now, stride);
			metric = calculateMetric(node, prediction, now);
			if( metric > 0 )
			{
				priNormal.insert(pair<double, pair<CNode*, CPosition>>(metric, pair<CNode*, CPosition>(node, prediction)));
				reachable = true;
				break;
			}
		}
		if( !reachable )
			priUnreachable.insert(pair<double, pair<CNode*, CPosition>>(-metric, pair<CNode*, CPosition>(node, prediction)));
		//priUnreachable.insert(pair<double, pair<CNode*, CPosition>>(priUnreachable.size(), pair<CNode*, CPosition>(node, prediction)));
	}
	vector<pair<CNode*, CPosition>> normal;
	////least pri first
	//for( map<double, pair<CNode*, CPosition>>::iterator i = priNormal.begin(); i != priNormal.end(); ++i )
	//{
	//	normal.push_back(i->second);
	//}

	//largest pri first
	for( map<double, pair<CNode*, CPosition>>::reverse_iterator i = priNormal.rbegin(); i != priNormal.rend(); ++i )
	{
		normal.push_back(i->second);
	}

	vector<pair<CNode*, CPosition>> unreachable;
	////least pri first
	//for( map<double, pair<CNode*, CPosition>>::iterator i = priUnreachable.begin(); i != priUnreachable.end(); ++i )
	//{
	//	unreachable.push_back(i->second);
	//}

	//largest pri first
	for( map<double, pair<CNode*, CPosition>>::reverse_iterator i = priUnreachable.rbegin(); i != priUnreachable.rend(); ++i )
	{
		unreachable.push_back(i->second);
	}
	return pair<vector<pair<CNode*, CPosition>>, vector<pair<CNode*, CPosition>>>(normal, unreachable);
}

bool CPFerry::newMANode()
{
	if( allMAs.size() >= CMANode::MAX_NUM_MA )
		return false;
	else
	{
		CPFerryMANode* ma = new CPFerryMANode();
		allMAs.push_back(ma);
		freeMAs.push_back(ma);
		CPrintHelper::PrintBrief(ma->getName() + " is created. (" + STRING(allMAs.size()) + " in total)");
		return true;
	}
}

bool CPFerry::newMANode(int n)
{
	for( int i = 0; i < n; ++i )
	{
		if( !newMANode() )
			return false;
	}
	return true;
}

vector<CGeneralNode*> CPFerry::findNeighbors(CGeneralNode & src)
{
	vector<CGeneralNode*> neighbors = CMacProtocol::findNeighbors(src);

	/**************************************************** MA ***********************************************************/

	vector<CPFerryMANode*> MAs = busyMAs;
	for( vector<CPFerryMANode*>::iterator iMA = MAs.begin(); iMA != MAs.end(); ++iMA )
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

bool CPFerry::transmitFrame(CGeneralNode & src, CFrame * frame, int now)
{
	vector<CGeneralNode*> neighbors = findNeighbors(src);
	return CMacProtocol::transmitFrame(src, frame, now, findNeighbors, receivePackets);
}

vector<CPacket*> CPFerry::receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;

	if( typeid( gToNode ) == typeid( CSink ) )
	{
		CSink* toSink = dynamic_cast< CSink* >( &gToNode );

		/*********************************************** Sink <- MA *******************************************************/

		if( typeid( gFromNode ) == typeid( CPFerryMANode ) )
		{
			CPFerryMANode* fromMA = dynamic_cast<CPFerryMANode*>( &gFromNode );
			packetsToSend = receivePackets(toSink, fromMA, packets, now);
		}
	}

	else if( typeid( gToNode ) == typeid( CPFerryMANode ) )
	{
		CPFerryMANode* toMA = dynamic_cast<CPFerryMANode*>( &gToNode );

		/************************************************ MA <- sink *******************************************************/

		if( typeid( gFromNode ) == typeid( CSink ) )
		{
			CSink* fromSink = dynamic_cast< CSink* >( &gFromNode );
			packetsToSend = receivePackets(toMA, fromSink, packets, now);
		}

		/************************************************ MA <- node *******************************************************/

		else if( typeid( gFromNode ) == typeid( CNode ) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( &gFromNode );
			packetsToSend = receivePackets(toMA, fromNode, packets, now);
		}
	}

	else if( typeid( gToNode ) == typeid( CNode ) )
	{
		CNode* node = dynamic_cast<CNode*>( &gToNode );

		/************************************************ Node <- MA *******************************************************/

		if( typeid( gFromNode ) == typeid( CPFerryMANode ) )
		{
			CPFerryMANode* fromMA = dynamic_cast<CPFerryMANode*>( &gFromNode );
			packetsToSend = receivePackets(node, fromMA, packets, now);
		}
	}
	else
		throw string("CPFerry::receivePackets(): Unexpected condition.");

	// TODO: + comm : node <--> sink ?

	return packetsToSend;
}

vector<CPacket*> CPFerry::receivePackets(CSink * sink, CPFerryMANode * fromMA, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;

	for( vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{
		if( typeid( **ipacket ) == typeid( CCtrl ) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>( *ipacket );
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

		else if( typeid( **ipacket ) == typeid( CData ) )
		{
			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back(*dynamic_cast<CData*>( *ipacket ));
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

vector<CPacket*> CPFerry::receivePackets(CPFerryMANode * ma, CSink * fromSink, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	vector<CData> dataToSend;  //空vector代表代表缓存为空

	for( vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid( **ipacket ) == typeid( CCtrl ) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>( *ipacket );
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

				case CCtrl::_rts:

					if( !ma->hasData() )
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

					if( ma->isReturningToSink() )
						atMAReturn(ma, time);

					//收到空的ACK时，结束本次数据传输
					if( ctrl->getACK().empty() )
						return packetsToSend;
					//clear data with ack
					else
						ma->dropDataByAck(ctrl->getACK());

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
	if( !dataToSend.empty() )
	{
		for( auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata )
			packetsToSend.push_back(new CData(*idata));
	}

	return packetsToSend;

}

vector<CPacket*> CPFerry::receivePackets(CNode * node, CPFerryMANode * fromMA, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
	vector<CData> dataToSend;  //空vector代表代表缓存为空
	int capacity = INVALID;

	for( vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid( **ipacket ) == typeid( CCtrl ) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>( *ipacket );
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

				case CCtrl::_rts:

					if( !node->hasData() )
					{
						nodataToSend = new CCtrl(node->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_no_data);
						break;
					}
					//CTS
					ctrlToSend = new CCtrl(node->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);

					break;

				case CCtrl::_cts:

					break;

					/************************************* rcv capacity **********************************/

				case CCtrl::_capacity:

					capacity = ctrl->getCapacity();

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
						node->dropDataByAck(ctrl->getACK());

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
	if( nodataToSend != nullptr )
	{
		packetsToSend.push_back(nodataToSend);
	}
	if( !dataToSend.empty() )
	{
		for( auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata )
			packetsToSend.push_back(new CData(*idata));
	}

	return packetsToSend;

}

vector<CPacket*> CPFerry::receivePackets(CPFerryMANode * ma, CNode * fromNode, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;

	for( vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{
		if( typeid( **ipacket ) == typeid( CCtrl ) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>( *ipacket );
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

					atDataCollection(ma, fromNode, ma->getLocation(), time);

					break;

				default:

					break;
			}
			++ipacket;
		}

		else if( typeid( **ipacket ) == typeid( CData ) )
		{
			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back(*dynamic_cast<CData*>( *ipacket ));
				++ipacket;
			} while( ipacket != packets.end() );

			//accept data into buffer
			vector<CData> ack = ma->bufferData(time, datas);

			CPrintHelper::PrintCommunication(time, fromNode->getName(), ma->getName(), ack.size());
			atDataCollection(ma, fromNode, ma->getLocation(), time);

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

void CPFerry::CommunicateBetweenNeighbors(int now)
{
	if( now % getConfig<int>("log", "slot_log") == 0 )
	{
		if( now > 0 )
		{
			CPrintHelper::PrintPercentage("Task Met", CPFerryTask::getPercentTaskMet());
			CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
		}
		CPrintHelper::PrintNewLine();
		CPrintHelper::PrintHeading(now, "DATA DELIVERY");
	}

	// TODO: sink receive RTS / send by slot ?
	// pferry: sink => MAs
	CSink* sink = CSink::getSink();
	transmitFrame(*sink, sink->sendRTS(now), now);

	vector<CPFerryMANode*> MAs = allMAs;
	// pferry: MAs => nodes
	for( CPFerryMANode* pMA : MAs )
	{
		// skip discover if buffer is full && _selfish is used
		if( pMA->isBusy()
		   && pMA->getBufferVacancy() > 0 )
			transmitFrame(*pMA, pMA->sendRTSWithCapacity(now), now);
	}

	// pferry: no forward between nodes
}

//return false if skipped (no valid prediction for current time / no available nodes)

bool CPFerry::arrangeTask(vector<CNode*> nodes, int now)
{
	if( !freeMAs.empty() )
	{
		pair<vector<pair<CNode*, CPosition>>, vector<pair<CNode*, CPosition>>> res = sortByPriority(nodes, now);
		vector<pair<CNode*, CPosition>> sorted = res.first;
		vector<pair<CNode*, CPosition>> unreachableNodes = res.second;

		int i = 0;
		for( ; !freeMAs.empty() && i < sorted.size(); ++i )
		{
			CPFerryMANode* ma = freeMAs.front();
			CNode* node = sorted[i].first;
			CPosition pos = sorted[i].second;
			ma->assignTask(node, pos, minWaitingTime(), now);

			CPrintHelper::PrintBrief(now, ma->getName() + " is targetting " + node->getName()
									 + " around " + pos.toString()
									 + " at " + STRING(pos.getTime()) + "s.");

			RemoveFromList(freeMAs, ma);
			busyMAs.push_back(ma);
			RemoveFromList(candidateNodes, node);
			targetNodes.push_back(node);
		}

		dealWithUnreachableNodes(unreachableNodes, now);
		return i > 0;
	}
	else
		return false;
}

void CPFerry::ArrangeTask(int now)
{
	arrangeTask(candidateNodes, now);
}

void CPFerry::Init(int now)
{
	PATH_PREDICT = getConfig<string>("pferry", "path_predict");
	PRESTAGE_PROTOCOL = getConfig<config::EnumRoutingProtocolScheme>("pferry", "prestage_protocol");
	STARTTIME = getConfig<int>("pferry", "starttime");
	CPFerryMANode::RETURN_ONCE_MET = getConfig<bool>("pferry", "return_once_met");

	CPFerryTask::Init(CNode::getIdNodes(CNode::getAllNodes()));

	if( PRESTAGE_PROTOCOL == config::_xhar )
		HAR::Init(now);
}

void CPFerry::InitFromPrestageProtocol(int now)
{
	if( PRESTAGE_PROTOCOL == config::_xhar )
	{
		CPrintHelper::PrintHeading(now, "IMPORTING FROM HAR");
		initNodeInfo(now);
		initMANodes();
		CPrintHelper::PrintNewLine();
	}

}

bool CPFerry::Operate(int now)
{
	if( predictions.empty() )
		initPredictions();

	if( now >= STARTTIME
	   && now < STARTTIME + getConfig<int>("simulation", "slot") )
		InitFromPrestageProtocol(now);

	if( now < STARTTIME )
	{
		if( PRESTAGE_PROTOCOL == config::_xhar )
			HAR::Operate(now);
	}
	else
	{
		if( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac )
			CSMac::Prepare(now);
		else
			throw string("CPFerry::Operate(): Only SMac is allowed as MAC protocol for CPFerry.");

		//update node & ma positions
		if( !UpdateNodeStatus(now) )
			return false;
		updateMAStatus(now);

		//task management
		ArrangeTask(now);

		if( now < getConfig<int>("simulation", "runtime") )
			CommunicateBetweenNeighbors(now);
		//communicate
		//MA/sink - node: data collection
		//right node: return 
		//wrong node: wait until timeout
		//MA - sink: report task result
		//met: move node to candidate list, move ma to free list
		//miss: 
		//urgent node: redo
		//otherwise: move ma to free list, waiting for reassign
		ArrangeTask(now);

		PrintInfo(now);
	}
	return true;
}

void CPFerry::PrintInfo(int now)
{
	if( !( now % getConfig<int>("log", "slot_log") == 0
		  || now == getConfig<int>("simulation", "runtime") ) )
		return;


	/***************************************** 路由协议的通用输出 *********************************************/

	CRoutingProtocol::PrintInfo(CNode::getAllNodes(), now);


	/**************************************** 补充输出 *********************************************/

	//MA Buffer
	ofstream buffer_ma(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer_ma"), ios::app);
	if( now == 0 )
	{
		buffer_ma << endl << getConfig<string>("log", "info_log") << endl;
		buffer_ma << getConfig<string>("log", "info_buffer_ma") << endl;
	}
	buffer_ma << now << TAB;
	for( auto iMA = allMAs.begin(); iMA != allMAs.end(); ++iMA )
		buffer_ma << ( *iMA )->getBufferSize() << TAB;
	buffer_ma << endl;
	buffer_ma.close();

	//Task Met
	ofstream task(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_task"), ios::app);
	if( now == STARTTIME )
	{
		task << endl << getConfig<string>("log", "info_log") << endl;
		task << getConfig<string>("log", "info_task") << endl;
	}
	task << now << TAB << CPFerryTask::getPercentTaskMet() << TAB << CPFerryTask::getCountTaskMet()
		<< TAB << CPFerryTask::getCountTask() << endl;
	task.close();

	//Task Distribution Among Nodes
	ofstream task_node(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_task_node"), ios::app);
	if( STARTTIME <= now
	   && now < STARTTIME + getConfig<int>("log", "slot_log") )
	{
		task_node << endl << getConfig<string>("log", "info_log") << endl;
		task_node << getConfig<string>("log", "info_task_node") << TAB;
		for( pair<int, int> p : CPFerryTask::getCountTaskForNodes() )
			task_node << p.first << TAB;
		task_node << endl;
	}
	task_node << now << TAB;
	for( pair<int, int> p : CPFerryTask::getCountTaskForNodes() )
		task_node << p.second << TAB;
	task_node << endl;
	task_node.close();

}

void CPFerry::PrintFinal(int now)
{
	CRoutingProtocol::PrintFinal(now);
}

CTracePrediction::CPanSystem CTracePrediction::CPanSystem::readPanSystemFromFile(string filename)
{
	try
	{
		FILE *file;
		file = fopen(filename.c_str(), "rb");

		double temp_x = 0;
		double temp_y = 0;
		fscanf(file, "%lf %lf", &temp_x, &temp_y);
		fclose(file);

		CPanSystem pan;
		pan.setPan(CCoordinate(temp_x, temp_y));
		return pan;
	}
	catch( exception e )
	{
		throw string("CPanSystem::readPanSystemFromFile() : ") + e.what();
	}
}

void CTracePrediction::CPanSystem::CancelPanding(map<int, CCoordinate>& trace)
{
	for( map<int, CCoordinate>::iterator icoor = trace.begin(); icoor != trace.end(); ++icoor )
	{
		icoor->second = cancelPanding(icoor->second);
	}
}

//e.g. 31.full.trace

string CTracePrediction::filenamePrediction(string nodename)
{
	if( KEYWORD_PREDICT.empty() )
		KEYWORD_PREDICT = getConfig<string>("pferry", "keyword_predict");
	string filename = nodename + KEYWORD_PREDICT + getConfig<string>("trace", "extension_trace_file");
	return filename;
}

//e.g. 31.pan

string CTracePrediction::filenamePan(string nodename)
{
	if( EXTENSION_PAN_FILE.empty() )
		EXTENSION_PAN_FILE = getConfig<string>("pferry", "extension_pan_file");
	string filename = nodename + EXTENSION_PAN_FILE;
	return filename;
}

CTracePrediction::CTracePrediction(int nodeId, string identifier, string dir) : node(nodeId)
{
	string path = dir + '/' + filenamePrediction(identifier);
	if( !CFileHelper::IfExists(path) )
	{
		throw string("CTracePredict::CTracePredict(): Cannot find trace file \"" + path + "\".");
	}
	else
	{
		Trace trace = Trace::readTraceFromFile(path, false);
		*this = CTracePrediction(trace);

		static bool panning = true;

		if( panning )
		{
			string pathPan = dir + '/' + filenamePan(identifier);
			if( CFileHelper::IfExists(pathPan) )
			{
				CPanSystem pan = CPanSystem::readPanSystemFromFile(pathPan);
				pan.CancelPanding(this->trace);
			}
			else
			{
				if( CPrintHelper::Warn("CTracePrediction::CTracePrediction(): No pan file for trace predictions is found. "
									   "Confirm to proceed.") )
					panning = false;
			}
		}
	}
}

double CTracePrediction::calculateHitrate(Trace fact, Trace pred, int hitrange)
{
	map<int, CCoordinate> facts = fact.getTrace(), preds = pred.getTrace();
	int nHit = 0;
	for( pair<int, CCoordinate> pPred : preds )
	{
		if( CCoordinate::getDistance(pPred.second, facts[pPred.first]) <= hitrange )
			++nHit;
	}
	return double(nHit) / preds.size();
}

void CPFerryTask::Init(vector<int> idNodes)
{
	countTaskForNodes.clear();
	for( int id : idNodes )
		countTaskForNodes[id] = 0;
}

void CPFerryTask::recordTaskMet(int nodeId)
{
	++COUNT_TASK_MET;
	++COUNT_TASK;
	countTaskForNodes[nodeId]++;
}

void CPFerryTask::recordTaskMiss(int nodeId)
{
	++COUNT_TASK;
	countTaskForNodes[nodeId]++;
}

int CPFerryTask::getCountTaskMet()
{
	return COUNT_TASK_MET;
}

int CPFerryTask::getCountTask()
{
	return COUNT_TASK;
}

double CPFerryTask::getPercentTaskMet()
{
	return double(COUNT_TASK_MET) / COUNT_TASK;
}

map<int, int> CPFerryTask::getCountTaskForNodes()
{
	return countTaskForNodes;
}
