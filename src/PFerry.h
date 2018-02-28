#pragma once

#ifndef __PFERRY_H__
#define __PFERRY_H__

#include "RoutingProtocol.h"
#include "MacProtocol.h"
#include "TracePrediction.h"
#include "MANode.h"
#include "SortHelper.h"
#include "PrintHelper.h"


class CPFerryNode : 
	virtual public CNode
{
private:
public:
	~CPFerryNode()
	{
	};

protected:
	friend class CPFerry;

	CPFerryNode()
	{
	};
};

class CPFerryTask: 
	virtual CEntity
{
private:

	CPFerryNode* target;
	CPosition location;  //when & where to expect the target node
	bool met;

	static int NUM_TASK_MET;
	static int NUM_TASK;

protected:
	friend class CPFerryMANode;
	friend class CPFerry;

	CPFerryTask(CPFerryNode* node, CPosition location)
	{
		this->target = node;
		this->location = location;
		this->met = false;
	}
	CPFerryNode* getTarget() const
	{
		return this->target;
	}
	CPosition getPosition() const
	{
		return this->location;
	}
	int getTime() const
	{
		return this->location.getTime();
	}
	static void recordTaskMet()
	{
		++NUM_TASK_MET;
		++NUM_TASK;
	}
	static void recordTaskMiss()
	{
		++NUM_TASK;
	}
	static int getCountTaskMet()
	{
		return NUM_TASK_MET;
	}
	static int getCountTask()
	{
		return NUM_TASK;
	}
	static double getPercentTaskMet()
	{
		return NDigitFloat(double(NUM_TASK_MET) / NUM_TASK, 2);
	}
	bool isMet()
	{
		return this->met;
	}
	void setMet(bool met)
	{
		this->met = met;
	}
};

class CPFerryMANode : 
	virtual public CMANode
{
private:
	map<int, CPFerryTask*> tasks;
	vector<CPFerryTask*> taskHistory;
	map<CPFerryNode*, CPosition> collections;  //actual met nodes & latest collection time

	inline void init()
	{
		FreePointer(this->route);
		this->route = new CRoute(CSink::getSink());
	}

public:
	~CPFerryMANode()
	{
		tasks.clear();
		for( pair<int, CPFerryTask*> p: tasks )
			FreePointer( p.second );
		FreePointerVector(this->taskHistory);
	}

protected:
	friend class CPFerry;

	//TODO: check for proper initialization from base class
	CPFerryMANode()
	{
		init();
	}
	map<CPFerryNode*, CPosition> getCollections() const
	{
		return collections;
	}
	map<int, CPFerryTask*> getTasks() const
	{
		return this->tasks;
	}
	CPFerryTask* findTask(int nodeId)
	{
		if( tasks.find(nodeId) == tasks.end() )
			return nullptr;
		else
			return tasks[nodeId];
	}
	void assignTask(CPFerryNode* node, CPosition location, int waitingTime)
	{
		CPFerryTask* task = new CPFerryTask(node, location);
		this->tasks[node->getID()] = task;
		if( this->route == nullptr )
			this->route = new CRoute(CSink::getSink());
		this->route->AddWaypoint(new CPosition(location), waitingTime);
		if( this->route->getToPoint() == CSink::getSink() )
			this->route->updateToPoint();
	}
	void endTask()
	{
		for( pair<int, CPFerryTask*> p : tasks )
			this->taskHistory.push_back( p.second );
		this->tasks.clear();
		this->collections.clear();
		this->endRoute();
	}
	void recordCollection(CPFerryNode* node, CCoordinate location, int time)
	{
		this->collections[node] = CPosition(node->getID(), location, time);
		CPFerryTask* task = findTask(node->getID());
		if( task != nullptr )
		{
			task->setMet(true);
			this->setReturnAtOnce(true);
		}
	}
	//UNDONE: test
	vector<CData> bufferData(int time, vector<CData> datas)
	{
		vector<CData> ack = datas;
		RemoveFromList(datas, this->buffer);

		bool atPoint = isAtWaypoint();
		if( atPoint )
		{
			CData::deliverAtWaypoint(datas.size());
		}
		else
			CData::deliverOnRoute(datas.size());

		for( auto idata = datas.begin(); idata != datas.end(); ++idata )
		{
			////认为到达 MA 节点即到达 sink
			//idata->arriveSink(time);
			this->buffer.push_back(*idata);
		}

		return ack;
	}
	//UNDONE: test
	void updateStatus(int now)
	{
		if( this->time < 0 )
			this->time = now;
		int interval = now - this->time;

		//updateTimerOccupied(time);

		//target met/缓存已满，立即返回sink
		if( ifReturnAtOnce() ||
		   (getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_selfish
		   && buffer.size() >= capacityBuffer) )
		{
			waitingWindow = waitingState = 0;
			route->updateToPointWithSink();
		}

		//处于等待中
		if( waitingWindow > 0 )
		{
			//等待即将结束		
			if( ( waitingState + interval ) >= waitingWindow )
			{
				interval = waitingState + interval - waitingWindow;  //等待结束后，剩余的将用于移动的时间
				waitingWindow = waitingState = 0;  //等待结束，将时间窗重置
				route->updateToPoint();
				this->setReturnAtOnce(true);
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
			this->setTime(now);
			return;
		}

		//开始在路线上移动
		this->setTime(now - interval);  //将时间置于等待结束，移动即将开始的时间点
		atPoint = nullptr;  //离开路点

		CBasicEntity *toPoint = route->getToPoint();
		int waitingTime = route->getWaitingTime();
		int timeLeftAfterArrival = this->moveTo(*toPoint, interval, getConfig<int>("ma", "speed"));

		//如果已到达目的地
		if( timeLeftAfterArrival >= 0 )
		{

			//若目的地的类型是waypoint
			CPosition *ppos = nullptr;
			CSink *psink = nullptr;
			if( ( ppos = dynamic_cast< CPosition * >( toPoint ) ) != nullptr )
			{
				this->atPoint = toPoint;

#ifdef DEBUG
				CPrintHelper::PrintDetail(now, this->getName() + " arrives at task position " + ppos->format() + ".");
#endif // DEBUG

				CPFerryTask* task = this->findTask(ppos->getNode());
				if( task && task->isMet() )
					waitingWindow = 0;
				else if( task )
				{
					int timePred = task->getTime();
					waitingWindow = int(max(30, timePred + waitingTime - this->getTime()));
				}
				else
					throw string("CPFerryMANode::updateStatus(): Cannot find task for node id" + STRING( ppos->getNode() ) + ".");
				waitingState = 0;  //重新开始等待
			}
			//若目的地的类型是 sink
			else if( ( psink = dynamic_cast< CSink * >( toPoint ) ) != nullptr )
			{
				//route->updateToPoint();
			}
		}
		return;
	}
};

class CPFerry :
	virtual public CRoutingProtocol
{
private:


	static map<int, map<int, CTracePrediction*>> predictions;  //{stride: {node: prediction}}
	static int STARTTIME_PREDICTION;
	static vector<int> strides;

	static vector<CPFerryMANode*> allMAs;
	static vector<CPFerryMANode*> freeMAs;
	static vector<CPFerryMANode*> busyMAs;

	static int INIT_NUM_MA;
	static int MAX_NUM_MA;

	static vector<CPFerryNode*> allNodes;
	static vector<CPFerryNode*> aliveNodes;
	static vector<CPFerryNode*> deadNodes;
	static vector<CPFerryNode*> targetNodes;
	//static vector<CPFerryNode*> missNodes;
	static vector<CPFerryNode*> candidateNodes;  //untargetted nodes
	static vector<CPFerryNode*> urgentNodes;

	static map<CPFerryNode*, vector<CPosition>> collectionHistory;

	typedef struct CNodeRecord
	{
		int timeCollect;
		int bufferVacancy;  //after collection
	} CNodeRecord;
	static map<CPFerryNode*, CNodeRecord> collectionRecords;  //known latest collection time for nodes

	static void initNodes()
	{
		vector<string> filenames = CFileHelper::ListDirectory(getConfig<string>("trace", "path"));
		filenames = CFileHelper::FilterByExtension(filenames, getConfig<string>("trace", "extension_trace_file"));

		if( filenames.empty() )
			throw string("CPFerry::initNodes(): Cannot find any trace files under \"" + getConfig<string>("trace", "path")
						 + "\".");

		for( int i = 0; i < filenames.size(); ++i )
		{
			double dataRate = getConfig<double>("node", "default_data_rate");
			if( i % 5 == 0 )
				dataRate *= 5;
			CPFerryNode* node = new CPFerryNode();
			node->setDataByteRate(dataRate);
			node->generateID();
			node->loadTrace(filenames[i]);
			allNodes.push_back(node);

			updatecollectionRecords(node, node->getBufferVacancy(), 0);
			collectionHistory[node] = vector<CPosition>();
		}

		aliveNodes = allNodes;
		candidateNodes = allNodes;
	}
	static void initMANodes()
	{
		INIT_NUM_MA = getConfig<int>("pferry", "init_num_ma");
		MAX_NUM_MA = getConfig<int>("pferry", "max_num_ma");
		newMANode(INIT_NUM_MA);
		
		freeMAs = allMAs;
	}
	static void initPredictions()
	{
		vector<CPFerryNode *> nodes = allNodes;
		string dir = getConfig<string>("pferry", "path_predict"); //e.g. ../res/predict
		vector<string> subdirs = CFileHelper::ListDirectory(dir);

		if( subdirs.empty() )
			throw string("CPFerry::initPredictions(): Cannot find any trace files under \"" + dir + "\".");
		for( string subdir: subdirs )  //e.g. ../res/predict/1
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
				for( CPFerryNode *pnode : nodes )
				{
					predictions[stride][pnode->getID()] = new CTracePrediction(pnode, subdir);
				}
			}
		}
	}

	static CPFerryNode* cast(CNode* node)
	{
		return dynamic_cast< CPFerryNode* >( node );
	}
	static CNode* cast(CPFerryNode* node)
	{
		return dynamic_cast< CNode* >( node );
	}
	static vector<CPFerryNode*> cast(vector<CNode*> nodes)
	{
		vector<CPFerryNode*> res;
		for( CNode* node : nodes )
			res.push_back(cast(node));
		return res;
	}
	static vector<CNode*> cast(vector<CPFerryNode*> nodes)
	{
		vector<CNode*> res;
		for( CPFerryNode* node : nodes )
			res.push_back(cast(node));
		return res;
	}
	static bool ClearDeadNodes(int now)
	{
		vector<CNode*> tempAliveList = cast(aliveNodes);
		vector<CNode*> tempDeadList = cast(deadNodes);
		bool death = CNode::ClearDeadNodes(tempAliveList, tempDeadList, now);
		aliveNodes = cast(tempAliveList);
		deadNodes = cast(tempDeadList);
		return death;
	}

	static bool hasNodes(int now)
	{
		return !aliveNodes.empty();
	}

	static int minWaitingTime()
	{
		return CCTrace::getInterval();
	}

	static void updatecollectionRecords(CPFerryNode* node, int bufferVacancy, int time)
	{
		if(collectionRecords.find(node) == collectionRecords.end()
		   || collectionRecords[node].timeCollect < time)
			collectionRecords[node] = CNodeRecord{ time, bufferVacancy };
	}
	
	// 60 -> 90; 50 -> 60
	static int nextInterval(int now, int stride)
	{
		int interval = CCTrace::getInterval();
		return ( now / interval + stride ) * interval;
	}
	static bool hasPrediction(CPFerryNode* node, int time, int stride)
	{
		CTracePrediction* prediction = predictions[stride][node->getID()];
		return prediction->isValid( nextInterval(time, stride) );
	}
	static CPosition getNextPrediction(CPFerryNode* node, int time, int stride)
	{
		int timePred = nextInterval(time, stride);
		CTracePrediction* prediction = predictions[stride][node->getID()];
		try
		{
			return prediction->getPrediction(timePred);
		}
		catch( string error )
		{
			throw error;
		}
	}

	// TODO: ?
	static bool isUrgent(CPFerryNode* node, int now)
	{
		try
		{
			//CPosition prediction = getNextPrediction(node, now, strides[0]);
			double timePrediction = nextInterval(now, strides[0]);
			double timeOverflow = collectionRecords[node].timeCollect + collectionRecords[node].bufferVacancy / node->getDataCountRate();
			//double timeTravel = 2 * CBasicEntity::getDistance(prediction, *CSink::getSink()) / CPFerryMANode::getSpeed();
		
			return (timeOverflow < timePrediction);
		}
		catch( string error )
		{
			return false;
		}

	}
	static bool updateNodeStatus(int now)
	{
		//update basic status
		vector<CPFerryNode *> nodes = aliveNodes;
		for( CPFerryNode * node : nodes )
			node->updateStatus(now);

		ClearDeadNodes(now);

		if( !hasNodes(now) )
			return false;

		//update urgent list
		urgentNodes.clear();
		for( CPFerryNode* node : aliveNodes )
		{
			if( isUrgent(node, now) )
				urgentNodes.push_back(node);
		}

		return hasNodes(now);
	}
	static bool updateMAStatus(int now)
	{
		vector<CPFerryMANode*> mas = busyMAs;
		for( CPFerryMANode* ma : mas )
		{
			ma->updateStatus(now);
		}
		return true;
	}

	static void recordTargetMet(CPFerryNode* node)
	{
		RemoveFromList(targetNodes, node);
		//RemoveFromList(missNodes, node);
		AddToListUniquely(candidateNodes, node);
	}
	static void recordTargetMiss(CPFerryNode* node)
	{
		RemoveFromList(targetNodes, node);
		AddToListUniquely(candidateNodes, node);
	}
	static void reportTask(CPFerryMANode* ma, int now)
	{
		bool met = false;
		for( pair<int, CPFerryTask*> p : ma->getTasks() )
		{
			CPFerryTask* task = p.second;
			CPFerryNode* node = task->target;
			met = task->met;
			if( met )
			{
				recordTargetMet(node);
				CPFerryTask::recordTaskMet();
			}
			else
			{
				recordTargetMiss(node);
				CPFerryTask::recordTaskMiss();
			}
		}

		map<CPFerryNode*, CPosition> collections = ma->getCollections();
		for( pair<CPFerryNode*, CPosition> collection : collections )
		{
			CPFerryNode* node = collection.first;
			recordTargetMet(node);
		}

		string message = ma->getName() + " returns with target ";
		if( met )
			message += "met";
		else
			message += "miss";
		message += " & " + STRING(collections.size()) + " collections.";
		CPrintHelper::PrintDetail(now, message);
	}

	//FIXME:
	//static void dealWithMissNodes()
	//{
	//	AddToListUniquely(candidateNodes, missNodes);
	//}
	//FIXME:
	static bool dealWithUnreachableNodes(vector<pair<CPFerryNode*, CPosition>> nodes, int now)
	{
#ifdef DEBUG
		if(!nodes.empty() )
			CPrintHelper::PrintDetail(now, STRING(nodes.size()) + " nodes are unreachable.");
#endif // DEBUG

		int i = 0;
		for( ; !freeMAs.empty() && i < nodes.size(); ++i )
		{
			CPFerryMANode* ma = freeMAs.front();
			CPFerryNode* node = nodes[i].first;
			CPosition pos = nodes[i].second;
			ma->assignTask(node, pos, minWaitingTime());
			ma->setTime(now);

			CPrintHelper::PrintDetail(now, ma->getName() + " is targetting (unreachable) " + node->getName()
									  + " around " + pos.getLocation().format()
									  + " at " + STRING(pos.getTime()) + "s.");

			RemoveFromList(freeMAs, ma);
			busyMAs.push_back(ma);
			RemoveFromList(urgentNodes, node);
			RemoveFromList(candidateNodes, node);
			targetNodes.push_back(node);
			//missNodes.push_back(node);
		}

		return i > 0;
	}

	static void atDataCollection(CPFerryMANode* ma, CPFerryNode* node, CCoordinate location, int time)
	{
		ma->recordCollection(node, location, time);
	}

	static void turnFree(CPFerryMANode* ma)
	{
		AddToListUniquely(freeMAs, ma);
		RemoveFromList(busyMAs, ma);
	}
	static void recordCollection(CPFerryMANode* ma)
	{
		map<CPFerryNode*, CPosition> collections = ma->getCollections();
		for( pair<CPFerryNode*, CPosition> pCollection : collections )
			collectionHistory[pCollection.first].push_back(pCollection.second);
	}

	static void atMAReturn(CPFerryMANode* ma, int now)
	{
		reportTask(ma, now);
		ma->endTask();
		recordCollection(ma);
		turnFree(ma);
	}

	//FIXME:
	static double calculateMetric(CPFerryNode* node, CPosition prediction, int now)
	{
		double timePrediction = prediction.getTime();
		double timeOverflow = collectionRecords[node].timeCollect + collectionRecords[node].bufferVacancy / node->getDataCountRate() - now;
		double timeArrival = CBasicEntity::getDistance(prediction, *CSink::getSink()) / CPFerryMANode::getSpeed();

		if( timeArrival + now > timePrediction )
			return INVALID;  // TODO: what to do?
		else
		{
			if( timeOverflow >= 0 )
				return timeArrival / max(1, timeOverflow);
			else
				return -timeOverflow * node->getDataCountRate();
		}
	}

	static pair<vector<pair<CPFerryNode*, CPosition>>, vector<pair<CPFerryNode*, CPosition>>> sortByPriority(vector<CPFerryNode*> nodes, int now)
	{
		multimap<double, pair<CPFerryNode*, CPosition>> pri;
		vector<pair<CPFerryNode*, CPosition>> unreachableNodes;
		for( CPFerryNode* node : nodes )
		{
			CPosition prediction;
			bool unreachable = true;
			for( int stride : strides )
			{
				if( !hasPrediction(node, now, stride) )
				{
					unreachable = false;
					break;
				}
				prediction = getNextPrediction(node, now, stride);
				double metric = calculateMetric(node, prediction, now);
				if( metric > 0 )
				{
					pri.insert(pair<double, pair<CPFerryNode*, CPosition>>(metric, pair<CPFerryNode*, CPosition>(node, prediction)));
					unreachable = false;
					break;
				}
			}
			if( unreachable )
				unreachableNodes.push_back(pair<CPFerryNode*, CPosition>(node, prediction));
		}
		vector<pair<CPFerryNode*, CPosition>> res;
		for( pair<double, pair<CPFerryNode*, CPosition>> p : pri )
		{
			res.push_back(p.second);
		}
		return pair<vector<pair<CPFerryNode*, CPosition>>, vector<pair<CPFerryNode*, CPosition>>>(res, unreachableNodes);
	}
	static bool newMANode()
	{
		if( allMAs.size() >= MAX_NUM_MA )
			return false;
		else
		{
			CPFerryMANode* ma = new CPFerryMANode();
			allMAs.push_back(ma);
			freeMAs.push_back(ma);
			CPrintHelper::PrintDetail(ma->getName() + " is created. (" + STRING(allMAs.size()) + " in total)");
			return true;
		}
	}
	static bool newMANode(int n)
	{
		for( int i = 0; i < n; ++i )
		{
			if( !newMANode() )
				return false;
		}
		return true;
	}


	CPFerry();
	~CPFerry();

	static vector<CGeneralNode*> findNeighbors(CGeneralNode& src)
	{
		vector<CGeneralNode*> neighbors;

		/************************************************ Sensor Node *******************************************************/

		vector<CPFerryNode*> nodes = aliveNodes;
		for( vector<CPFerryNode*>::iterator idstNode = nodes.begin(); idstNode != nodes.end(); ++idstNode )
		{
			CNode* dstNode = *idstNode;
			//skip itself
			if( ( dstNode )->getID() == src.getID() )
				continue;

			if( CBasicEntity::withinRange(src, *dstNode, getConfig<int>("trans", "range_trans")) )
			{
				if( dstNode->isAwake() )
				{
					if( Bet(getConfig<double>("trans", "prob_trans")) )
						neighbors.push_back(dstNode);
				}
			}
		}

		/*************************************************** Sink **********************************************************/

		CSink* sink = CSink::getSink();
		if( CBasicEntity::withinRange(src, *sink, getConfig<int>("trans", "range_trans"))
		   && Bet(getConfig<double>("trans", "prob_trans"))
		   && sink->getID() != src.getID() )
		{
			neighbors.push_back(sink);
			CSink::encount();
			CSink::encountActive();
		}


		/**************************************************** MA ***********************************************************/

		vector<CPFerryMANode*> MAs = busyMAs;
		for( vector<CPFerryMANode*>::iterator iMA = MAs.begin(); iMA != MAs.end(); ++iMA )
		{
			//skip itself
			if( ( *iMA )->getID() == src.getID() )
				continue;

			if( CBasicEntity::withinRange(src, **iMA, getConfig<int>("trans", "range_trans"))
			   && Bet(getConfig<double>("trans", "prob_trans"))
			   && ( *iMA )->isAwake() )
			{
				neighbors.push_back(*iMA);
			}
		}
		return neighbors;
		// TODO: sort by distance with src node ?
	}
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now)
	{
		vector<CGeneralNode*> neighbors = findNeighbors(src);
		return CMacProtocol::transmitFrame(src, frame, now, findNeighbors, receivePackets);
	}
	//UNDONE:
	static vector<CPacket*> receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int now)
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

			else if( typeid( gFromNode ) == typeid( CPFerryNode ) )
			{
				CPFerryNode* fromNode = dynamic_cast<CPFerryNode*>( &gFromNode );
				packetsToSend = receivePackets(toMA, fromNode, packets, now);
			}
		}

		else if( typeid( gToNode ) == typeid( CPFerryNode ) )
		{
			CPFerryNode* node = dynamic_cast<CPFerryNode*>( &gToNode );

			/************************************************ Node <- MA *******************************************************/

			if( typeid( gFromNode ) == typeid( CPFerryMANode ) )
			{
				CPFerryMANode* fromMA = dynamic_cast<CPFerryMANode*>( &gFromNode );
				packetsToSend = receivePackets(node, fromMA, packets, now);
			}
		}

		// TODO: + comm : node <--> sink ?

		return packetsToSend;
	}

	static vector<CPacket*> receivePackets(CSink* sink, CPFerryMANode* fromMA, vector<CPacket*> packets, int time)
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

	static vector<CPacket*> receivePackets(CPFerryMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time)
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
							if( ma->ifReturnAtOnce() )
								atMAReturn(ma, time);
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
							if( ma->ifReturnAtOnce() )
								atMAReturn(ma, time);
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

						atMAReturn(ma, time);
						//收到空的ACK时，结束本次数据传输
						if( ctrl->getACK().empty() )
							return packetsToSend;
						//clear data with ack
						else
							ma->checkDataByAck(ctrl->getACK());

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

	static vector<CPacket*> receivePackets(CNode* node, CPFerryMANode* fromMA, vector<CPacket*> packets, int time)
	{
		vector<CPacket*> packetsToSend;
		CCtrl* ctrlToSend = nullptr;
		CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
		vector<CData> dataToSend;  //空vector代表代表缓存为空
		int capacity = -1;

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
							node->checkDataByAck(ctrl->getACK());

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

	static vector<CPacket*> receivePackets(CPFerryMANode* ma, CPFerryNode* fromNode, vector<CPacket*> packets, int time)
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
#ifdef DEBUG
						CPrintHelper::PrintCommunication(time, fromNode->getName(), ma->getName(), 0);
#endif // DEBUG

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
				atDataCollection(ma, fromNode, ma->getLocation(), time);

				CPrintHelper::PrintCommunication(time, fromNode->getName(), ma->getName(), ack.size());

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

	//TODO: add node->sink ?

	static void CommunicateWithNeighbor(int now)
	{
		static bool print = false;
		if( now == 0
		   || print )
		{
			CPrintHelper::PrintHeading(now, "DATA DELIVERY");
			print = false;
		}

		// TODO: sink receive RTS / send by slot ?
		// pferry: sink => MAs
		CSink* sink = CSink::getSink();
		transmitFrame(*sink, sink->sendRTS(now), now);

		vector<CPFerryNode*> nodes = aliveNodes;
		vector<CPFerryMANode*> MAs = busyMAs;


		// pferry: MAs => nodes
		for( vector<CPFerryMANode*>::iterator srcMA = MAs.begin(); srcMA != MAs.end(); ++srcMA )
		{
			// skip discover if buffer is full && _selfish is used
			if( ( *srcMA )->getCapacityForward() > 0 )
				transmitFrame(**srcMA, ( *srcMA )->sendRTSWithCapacity(now), now);
		}
		// pferry: no forward between nodes


		if( ( now + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0 )
		{
			CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
			CPrintHelper::PrintNewLine();
		print = true;
		}
	}

	//return false if skipped (no valid prediction for current time / no available nodes)
	static bool arrangeTask(vector<CPFerryNode*> nodes, int now)
	{
		if( !freeMAs.empty() )
		{
			pair<vector<pair<CPFerryNode*, CPosition>>, vector<pair<CPFerryNode*, CPosition>>> res = sortByPriority(nodes, now);
			vector<pair<CPFerryNode*, CPosition>> sorted = res.first;
			vector<pair<CPFerryNode*, CPosition>> unreachableNodes = res.second;
			
			int i = 0;
			for( ; !freeMAs.empty() && i < sorted.size(); ++i )
			{
				CPFerryMANode* ma = freeMAs.front();
				CPFerryNode* node = sorted[i].first;
				CPosition pos = sorted[i].second;
				ma->assignTask(node, pos, minWaitingTime());
				ma->setTime(now);
				
				CPrintHelper::PrintDetail(now, ma->getName() + " is targetting " + node->getName()
										  + " around " + pos.getLocation().format() 
										  + " at " + STRING(pos.getTime()) + "s.");

				RemoveFromList(freeMAs, ma);
				busyMAs.push_back(ma);
				RemoveFromList(urgentNodes, node);
				RemoveFromList(candidateNodes, node);
				targetNodes.push_back(node);
				//missNodes.push_back(node);
			}

			dealWithUnreachableNodes(unreachableNodes, now);
			return i > 0;
		}
		else
			return false;
	}
	static void ArrangeTask(int now)
	{
		if( !freeMAs.empty() )
		{
			arrangeTask(candidateNodes, now);
		}
		if( !urgentNodes.empty() &&
				allMAs.size() < MAX_NUM_MA )
		{
			int n = urgentNodes.size();
			if( newMANode(n) )
			{
				CPrintHelper::PrintDetail(now, "Another " + STRING(n) + " MAs are created. (" + STRING(allMAs.size()) + " in total)");
				arrangeTask(urgentNodes, now);
			}
		}
		else if( !urgentNodes.empty() )
		{
#ifdef DEBUG
			CPrintHelper::PrintDetail(now, STRING(urgentNodes.size()) + " nodes are short of MAs.");
#endif // DEBUG
		}
	}
	

public:

	static bool Init()
	{	// UNDONE:
		initNodes();

		initMANodes();
		
		initPredictions();
		return true;
	}
	static bool Operate(int now)
	{
		//update node & ma positions
		if( !updateNodeStatus(now) )
			return false;
		CSink::getSink()->updateStatus(now);
		updateMAStatus(now);
		
		//task management
		ArrangeTask(now);

		CommunicateWithNeighbor(now);
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
		return true;
	}
	
	static void PrintInfo(int now)
	{
		if( !( now % getConfig<int>("log", "slot_log") == 0
			  || now == getConfig<int>("simulation", "runtime") ) )
			return;


		/***************************************** 路由协议的通用输出 *********************************************/

		CRoutingProtocol::PrintInfo(cast(allNodes), now);

		/**************************************** 补充输出 *********************************************/



		if( now % getConfig<int>("log", "slot_log") == 0
		   || now == getConfig<int>("simulation", "runtime") )
		{
			//Task Met
			ofstream task(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_task"), ios::app);
			if( now == 0 )
			{
				task << endl << getConfig<string>("log", "info_log") << endl;
				task << getConfig<string>("log", "info_task") << endl;
			}
			task << now << TAB << CPFerryTask::getPercentTaskMet() << TAB << CPFerryTask::getCountTaskMet() 
				<< TAB << CPFerryTask::getCountTask() << endl;
			task.close();

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

		}

	}

	static void PrintFinal(int now)
	{
		CRoutingProtocol::PrintFinal(now);

		//最终final输出（补充）
		ofstream final(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_final"), ios::app);
		final << CData::getPercentDeliveryAtWaypoint() << TAB;
		final.close();
	}
};

#endif // __PFERRY_H__