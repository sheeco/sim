#pragma once

#ifndef __PFERRY_H__
#define __PFERRY_H__

#include "Configuration.h"
#include "RoutingProtocol.h"
#include "HAR.h"
#include "MacProtocol.h"
#include "TracePrediction.h"
#include "MANode.h"
#include "SortHelper.h"
#include "PrintHelper.h"


class CPFerryTask: 
	virtual CEntity
{
private:

	CNode* target;
	CPosition location;  //when & where to expect the target node
	bool met;

	static int NUM_TASK_MET;
	static int NUM_TASK;

protected:
	friend class CPFerryMANode;
	friend class CPFerry;

	CPFerryTask(CNode* node, CPosition location)
	{
		this->target = node;
		this->location = location;
		this->met = false;
	}
	CNode* getTarget() const
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
protected:

	typedef struct CCollectionRecord
	{
		int time = 0;
		CCoordinate location;
		int bufferVacancy = INVALID;

		CCollectionRecord(){};

		CCollectionRecord(int time, CCoordinate location, int bufferVacancy):
			time(time), location(location), bufferVacancy(bufferVacancy)
		{
		};
	} CCollectionRecord;

private:
	map<int, CPFerryTask*> tasks;
	vector<CPFerryTask*> taskHistory;
	map<int, CCollectionRecord> collections;  //actual met nodes & (latest collection time, bufferVacancy)

	inline void init()
	{
	}

public:
	~CPFerryMANode();

protected:
	friend class CPFerry;

	CPFerryMANode()
	{
		init();
	}
	CPFerryMANode(CHarMANode& harMA): CBasicEntity(harMA), CGeneralNode(harMA), CMANode(harMA)
	{
		init();
	}

	map<int, CCollectionRecord> getCollections() const
	{
		return collections;
	}
	inline bool hasTask() const
	{
		return !(this->tasks.empty());
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
	void assignTask(CNode* node, CPosition location, int waitingTime, int now)
	{
		CPFerryTask* task = new CPFerryTask(node, location);
		this->tasks[node->getID()] = task;
		if( !this->hasRoute() )
			this->route = new CRoute(CSink::getSink());
		this->route->AddWaypoint(new CPosition(location), waitingTime);
		this->route->initToPoint();
		this->setTime(now);
	}
	void endTask()
	{
		for( pair<int, CPFerryTask*> p : tasks )
			this->taskHistory.push_back( p.second );
		this->tasks.clear();
		this->collections.clear();
		this->endRoute();
	}
	void recordCollection(int nodeId, CCoordinate location, int bufferVacancy, int time)
	{
		this->collections[nodeId] = CCollectionRecord(time, location, bufferVacancy);
		CPFerryTask* task = findTask(nodeId);
		if( task != nullptr )
		{
			task->setMet(true);
			CPrintHelper::PrintDetail(time, this->getName() + " is returning to Sink due to task met.", 2);

			CBasicEntity* waypoint = nullptr;
			if( (waypoint = getAtWaypoint())
			    && waitingState >= 0 )
				CPrintHelper::PrintDetail(time, this->getName() + " has waited at " + waypoint->format() + " for " 
										  + STRING(waitingState) + "s.", 3);
			this->setReturningToSink();
		}
	}
	//UNDONE: test
	void updateStatus(int now);
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

	static vector<CNode*> targetNodes;
	//static vector<CNode*> missNodes;
	static vector<CNode*> candidateNodes;  //untargetted nodes

	typedef struct CNodeRecord
	{
		int timeCollect;
		int bufferVacancy;  //after collection
	} CNodeRecord;
	static map<int, CNodeRecord> collectionRecords;  //known latest collection time for nodes


	static string PATH_PREDICT;
	static config::EnumRoutingProtocolScheme PRESTAGE_PROTOCOL;


	//init based on newly loaded CNode
	static void initNodeInfo(int now)
	{
		vector<CNode*> nodes = CNode::getAliveNodes();
		for(CNode* pnode : nodes)
		{
			updatecollectionRecords(pnode->getID(), pnode->getBufferVacancy(), now);

		}
		candidateNodes = nodes;
	}
	static void initMANodes()
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
	static void initPredictions()
	{
		strides.clear();
		predictions.clear();

		vector<CNode *> nodes = CNode::getAllNodes();
		string dir = PATH_PREDICT; //e.g. ../res/predict
		vector<string> subdirs = CFileHelper::ListDirectory(dir);
		int minStartTime = INVALID;

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
				for( CNode *pnode : nodes )
				{
					CTracePrediction *pPred = new CTracePrediction(pnode, subdir);
					int starttime = pPred->getStartTime();
					if(minStartTime < 0
					   || starttime < minStartTime)
						minStartTime = starttime;
					predictions[stride][pnode->getID()] = pPred;
				}
			}
		}
		STARTTIME_PREDICTION = minStartTime;
	}

	static int minWaitingTime()
	{
		return CCTrace::getInterval();
	}

	static void updatecollectionRecords(int nodeId, int bufferVacancy, int time)
	{
		if(collectionRecords.find(nodeId) == collectionRecords.end()
		   || collectionRecords[nodeId].timeCollect < time)
			collectionRecords[nodeId] = CNodeRecord{ time, bufferVacancy };
	}
	
	// 60 -> 90; 50 -> 60
	static int nextInterval(int now, int stride)
	{
		int interval = CCTrace::getInterval();
		return ( now / interval + stride ) * interval;
	}
	static bool hasPrediction(CNode* node, int time, int stride)
	{
		CTracePrediction* prediction = predictions[stride][node->getID()];
		return prediction->isValid( nextInterval(time, stride) );
	}
	static CPosition getNextPrediction(CNode* node, int time, int stride)
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

	static bool UpdateNodeStatus(int now)
	{
		if( !CNode::UpdateNodeStatus(now) )
			return false;

		return true;
	}
	static bool updateMAStatus(int now)
	{
		vector<CPFerryMANode*> mas = allMAs;
		for( CPFerryMANode* ma : mas )
		{
			ma->updateStatus(now);
		}
		return true;
	}

	static void recordTargetMet(CNode* node)
	{
		RemoveFromList(targetNodes, node);
		AddToListUniquely(candidateNodes, node);
	}
	static void recordTargetMiss(CNode* node)
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
			CNode* node = task->target;
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

		map<int, CPFerryMANode::CCollectionRecord> collections = ma->getCollections();
		for( pair<int, CPFerryMANode::CCollectionRecord> collection : collections )
		{
			recordTargetMet( CNode::findNodeByID( collection.first ) );
		}

		string message = ma->getName() + " returns with target ";
		if( met )
			message += "met";
		else
			message += "miss";
		message += " & " + STRING(collections.size()) + " collections.";
		CPrintHelper::PrintBrief(now, message);
	}

	//FIXME:
	//static void dealWithMissNodes()
	//{
	//	AddToListUniquely(candidateNodes, missNodes);
	//}
	//FIXME:
	static bool dealWithUnreachableNodes(vector<pair<CNode*, CPosition>> nodes, int now)
	{
		if(!nodes.empty() )
			CPrintHelper::PrintDetail(now, STRING(nodes.size()) + " nodes are unreachable.", 4);

		int i = 0;
		for( ; !freeMAs.empty() && i < nodes.size(); ++i )
		{
			CPFerryMANode* ma = freeMAs.front();
			CNode* node = nodes[i].first;
			CPosition pos = nodes[i].second;
			ma->assignTask(node, pos, minWaitingTime(), now);

			CPrintHelper::PrintBrief(now, ma->getName() + " is targetting (unreachable) " + node->getName()
									  + " around " + pos.format()
									  + " at " + STRING(pos.getTime()) + "s.");

			RemoveFromList(freeMAs, ma);
			busyMAs.push_back(ma);
			RemoveFromList(candidateNodes, node);
			targetNodes.push_back(node);
			//missNodes.push_back(node);
		}

		return i > 0;
	}

	static void atDataCollection(CPFerryMANode* ma, CNode* node, CCoordinate location, int time)
	{
		ma->recordCollection(node->getID(), location, node->getBufferVacancy(), time);
	}

	static void turnFree(CPFerryMANode* ma)
	{
		AddToListUniquely(freeMAs, ma);
		RemoveFromList(busyMAs, ma);
	}
	static void atMAReturn(CPFerryMANode* ma, int now)
	{
		if( ma->hasTask() )
		{
			reportTask(ma, now);
			ma->endTask();
		}
		turnFree(ma);
	}

	//FIXME:
	static double calculateMetric(CNode* node, CPosition prediction, int now)
	{
		int datatime = getConfig<int>("simulation", "datatime");
		int timeLastCollectoin = collectionRecords[node->getID()].timeCollect;
		int bufferVacancy = collectionRecords[node->getID()].bufferVacancy;
		int bufferCapacity = node->getCapacityBuffer();
		double dataRate = node->getDataCountRate();
		double timePrediction = prediction.getTime();

		int bufferOccupancy = int( ( min(timePrediction, datatime) - timeLastCollectoin ) * dataRate + bufferCapacity - bufferVacancy );
		int dataLoss = max(0, bufferOccupancy - bufferCapacity);
		bufferOccupancy = min(bufferOccupancy, bufferCapacity);
		//double timeOverflow = timeLastCollectoin + bufferVacancy / dataRate;
		double timeArrival = CBasicEntity::getDistance(prediction, *CSink::getSink()) / CMANode::getMASpeed();

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

		if( timeArrival + now > timePrediction )
			return -( bufferOccupancy + dataLoss ) / timeArrival;
		else
		{
			if( dataLoss == 0 )
				return bufferOccupancy / timeArrival;
			else
				return dataLoss;
		}

	}

	static pair<vector<pair<CNode*, CPosition>>, vector<pair<CNode*, CPosition>>> sortByPriority(vector<CNode*> nodes, int now)
	{
		multimap<double, pair<CNode*, CPosition>> priNormal;
		multimap<double, pair<CNode*, CPosition>> priUnreachable;
		for( CNode* node : nodes )
		{
			CPosition prediction;
			bool unreachable = true;
			double metric;
			for( int stride : strides )
			{
				if( !hasPrediction(node, now, stride) )
				{
					unreachable = false;
					break;
				}
				prediction = getNextPrediction(node, now, stride);
				metric = calculateMetric(node, prediction, now);
				if( metric > 0 )
				{
					priNormal.insert(pair<double, pair<CNode*, CPosition>>(metric, pair<CNode*, CPosition>(node, prediction)));
					unreachable = false;
					break;
				}
			}
			if( unreachable )
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
	static bool newMANode()
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

	static vector<CPacket*> receivePackets(CNode* node, CPFerryMANode* fromMA, vector<CPacket*> packets, int time)
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

	static vector<CPacket*> receivePackets(CPFerryMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time)
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

	static void CommunicateBetweenNeighbors(int now)
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


		if( ( now + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0 )
		{
			CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
			CPrintHelper::PrintNewLine();
		print = true;
		}
	}

	//return false if skipped (no valid prediction for current time / no available nodes)
	static bool arrangeTask(vector<CNode*> nodes, int now)
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
										  + " around " + pos.format() 
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
	static void ArrangeTask(int now)
	{
		arrangeTask(candidateNodes, now);
	}
	

public:

	static void Init(int now)
	{	
		PATH_PREDICT = getConfig<string>("pferry", "path_predict");
		PRESTAGE_PROTOCOL = getConfig<config::EnumRoutingProtocolScheme>("pferry", "prestage_protocol");

		if( PRESTAGE_PROTOCOL == config::_xhar )
			HAR::Init(now);
	}
	static void InitFromPrestageProtocol(int now)
	{
		if( PRESTAGE_PROTOCOL == config::_xhar )
		{
			CPrintHelper::PrintHeading(now, "IMPORTING FROM HAR");
			initNodeInfo(now);
			initMANodes();
			CPrintHelper::PrintNewLine();
		}

	}
	static bool Operate(int now)
	{
		if( STARTTIME_PREDICTION == INVALID )
			initPredictions();

		if(now >= STARTTIME_PREDICTION
				&& now < STARTTIME_PREDICTION + getConfig<int>("simulation", "slot"))
			InitFromPrestageProtocol(now);

		if(now < STARTTIME_PREDICTION)
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
			if(!UpdateNodeStatus(now))
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
	
	static void PrintInfo(int now)
	{
		if( !( now % getConfig<int>("log", "slot_log") == 0
			  || now == getConfig<int>("simulation", "runtime") ) )
			return;


		/***************************************** 路由协议的通用输出 *********************************************/

		CRoutingProtocol::PrintInfo(CNode::getAllNodes(), now);

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
	}
};

#endif // __PFERRY_H__