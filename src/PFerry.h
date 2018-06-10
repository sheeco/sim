#pragma once

#ifndef __PFERRY_H__
#define __PFERRY_H__

#include "RoutingProtocol.h"
#include "HAR.h"
#include "MacProtocol.h"
#include "Configuration.h"


//存储轨迹预测结果的类
class CTracePrediction :
	virtual public Trace
{
private:

	class CPanSystem :
		virtual public CGeoEntity
	{
	private:
		CCoordinate stride;

	public:
		inline void setPan(CCoordinate stride)
		{
			this->stride = stride;
		}
		static CPanSystem readPanSystemFromFile(string filename);
		CCoordinate applyPanding(CCoordinate coor)
		{
			return coor + stride;
		}
		inline CCoordinate cancelPanding(CCoordinate coor)
		{
			return coor - stride;
		}
		void CancelPanding(map<int, CCoordinate>& trace);
	};

	int node;

	static string KEYWORD_PREDICT;
	static string EXTENSION_PAN_FILE;

	//e.g. 31.full.trace
	static string filenamePrediction(string nodename);
	//e.g. 31.pan
	static string filenamePan(string nodename);

	CTracePrediction(Trace& trace, int nodeId) : Trace(trace), node(nodeId)
	{
	};

public:

	~CTracePrediction()
	{
	};

	CTracePrediction(int nodeId, string identifier, string dir);

	static double calculateHitrate(Trace fact, Trace pred, int hitrange);

	inline CPosition getPrediction(int time)
	{
		if( !hasEntry(time) )
			throw string("CTracePrediction::getPrediction(): Cannot find prediction at " + STRING(time) + "s.");
		return CPosition(this->node, this->getLocation(time), time);
	}
};


//LTPR(PFerry)中MA节点的数据收集任务类
class CPFerryTask: 
	virtual CEntity
{
private:

	CNode* target;
	CPosition location;  //when & where to expect the target node
	bool met;

	static int COUNT_TASK_MET;
	static int COUNT_TASK;
	static map<int, int> countTaskForNodes;

protected:
	friend class CPFerryMANode;
	friend class CPFerry;

	CPFerryTask(CNode* node, CPosition location)
	{
		this->target = node;
		this->location = location;
		this->met = false;
	}
	inline CNode* getTarget() const
	{
		return this->target;
	}
	inline CPosition getPosition() const
	{
		return this->location;
	}
	inline int getTime() const
	{
		return this->location.getTime();
	}
	inline bool isMet()
	{
		return this->met;
	}
	inline void setMet(bool met)
	{
		this->met = met;
	}

	static void Init(vector<int> idNodes);
	static void recordTaskMet(int nodeId);
	static void recordTaskMiss(int nodeId);
	static int getCountTaskMet();
	static int getCountTask();
	static double getPercentTaskMet();
	static map<int, int> getCountTaskForNodes();
};

//装载LTPR(PFerry)协议的MA节点
class CPFerryMANode : 
	virtual public CMANode
{
protected:
	friend class CPFerry;

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
	
	static bool RETURN_ONCE_MET;

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

	CPFerryMANode()
	{
		init();
	}
	CPFerryMANode(CHarMANode& harMA): CBasicEntity(harMA), CGeneralNode(harMA), CMANode(harMA)
	{
		init();
	}

	inline map<int, CCollectionRecord> getCollections() const
	{
		return collections;
	}
	inline bool hasTask() const
	{
		return !(this->tasks.empty());
	}
	inline map<int, CPFerryTask*> getTasks() const
	{
		return this->tasks;
	}
	inline CPFerryTask* findTask(int nodeId)
	{
		if( tasks.find(nodeId) == tasks.end() )
			return nullptr;
		else
			return tasks[nodeId];
	}
	void assignTask(CNode* node, CPosition location, int waitingTime, int now);
	void endTask();
	void recordCollection(int nodeId, CCoordinate location, int bufferVacancy, int time);
	void updateStatus(int now);
};

//LTPR(PFerry)路由
class CPFerry :
	virtual public CRoutingProtocol
{
private:

	static map<int, map<int, CTracePrediction*>> predictions;  //{stride: {node: prediction}}
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
	static int STARTTIME;
	static int WAITING_TIME;


	//init based on newly loaded CNode
	static void initNodeInfo(int now);
	static void initMANodes();
	static void initPredictions();

	static int getWaitingTime()
	{
		return WAITING_TIME;
	}

	static void updatecollectionRecords(int nodeId, int bufferVacancy, int time);
	
	// 60 -> 90; 50 -> 60
	static int nextInterval(int now, int stride);
	static bool hasPrediction(CNode* node, int time, int stride);
	static CPosition getNextPrediction(CNode* node, int time, int stride);

	static bool UpdateNodeStatus(int now);
	static bool updateMAStatus(int now);

	static void recordTargetMet(CNode* node);
	static void recordTargetMiss(CNode* node);
	static void reportTask(CPFerryMANode* ma, int now);

	static bool dealWithUnreachableNodes(vector<pair<CNode*, CPosition>> nodes, int now);

	static void atDataCollection(CPFerryMANode* ma, CNode* node, CCoordinate location, int time);

	static void turnFree(CPFerryMANode* ma);
	static void atMAReturn(CPFerryMANode* ma, int now);

	static double calculateMetric(CNode* node, CPosition prediction, int now);

	static pair<vector<pair<CNode*, CPosition>>, vector<pair<CNode*, CPosition>>> sortByPriority(vector<CNode*> nodes, int now);
	static bool newMANode();
	static bool newMANode(int n);


	CPFerry()
	{
	}
	~CPFerry()
	{
	}

	static vector<CGeneralNode*> findNeighbors(CGeneralNode& src);
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now);
	static vector<CPacket*> receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int now);

	static vector<CPacket*> receivePackets(CSink* sink, CPFerryMANode* fromMA, vector<CPacket*> packets, int time);

	static vector<CPacket*> receivePackets(CPFerryMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time);

	static vector<CPacket*> receivePackets(CNode* node, CPFerryMANode* fromMA, vector<CPacket*> packets, int time);

	static vector<CPacket*> receivePackets(CPFerryMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time);

	//TODO: add node->sink ?

	static void CommunicateBetweenNeighbors(int now);

	//return false if skipped (no valid prediction for current time / no available nodes)
	static bool arrangeTask(vector<CNode*> nodes, int now);
	static void ArrangeTask(int now);
	

public:

	static void Init(int now);
	static void InitFromPrestageProtocol(int now);
	static bool Operate(int now);
	
	static void PrintInfo(int now);

	static void PrintFinal(int now);
};

#endif // __PFERRY_H__