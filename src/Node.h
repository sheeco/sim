#pragma once

#include <map>
#include "GeneralNode.h"
#include "Hotspot.h"
#include "Package.h"


class CNode :
	public CGeneralNode
{
//protected:

//	int ID;  //node编号
//	CCoordinate location;  //node现在的y坐标
//	int time;  //更新node坐标、工作状态（、Prophet中的衰减）的时间戳
//	bool flag;


private:

	double generationRate;
	double dutyCycle;

	int SLOT_SLEEP;  //由SLOT_TOTAL和DC计算得到
	int SLOT_LISTEN;  //由SLOT_TOTAL和DC计算得到
	int SLOT_WAIT;  //发送 RTS 之前等待的时间
	int state;  //取值范围在[ - SLOT_SLEEP, + SLOT_LISTEN )之间，值大于等于0即代表Listen状态
	int timeData;  //上一次数据生成的时间
	int timeDeath;  //节点失效时间，默认值为-1
	bool recyclable;  //在节点死亡之后，指示节点是否仍可被回收，默认为 true，直到 trace 信息终止赋 false（暂未使用，如果有充电行为则应该读取此参数）
	CHotspot *atHotspot;

	//用于统计输出节点的buffer状态信息
	int bufferSizeSum;
	int bufferChangeCount;
	static int encounterAtHotspot;
	static int encounterActive;  //有效相遇
	static int encounter;
	static int visiterAtHotspot;
	static int visiterOnRoute;;

	static int ID_COUNT;
	static vector<CNode *> nodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
	static vector<int> idNodes;  //用于储存所有传感器节点的ID，便于处理
	static vector<CNode *> deadNodes;  //能量耗尽的节点
	static vector<CNode *> deletedNodes;  //用于暂存Node个数动态变化时被暂时移出的节点

	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;

	/**************************************  Prophet  *************************************/

	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >，sink节点ID为0将位于最前，便于查找

	void init();

	CNode();

	CNode(double generationRate);

	static void initNodes();

	void generateData(int currentTime);

	/*************************************  Epidemic  *************************************/

	//将删除过期的消息（仅当使用TTL时）
	void dropOverdueData(int currentTime);

	//队列管理
	//将按照(1)“其他节点-本节点”(2)“旧-新”的顺序对buffer中的数据进行排序
	//超出MAX_QUEUE_SIZE时从前端丢弃数据，溢出时将从前端丢弃数据
	//注意：必须在dropOverdueData之后调用
	void dropDataIfOverflow();

	//注意：需要在每次收到数据之后、投递数据之后、生成新数据之后调用此函数
	void updateBufferStatus(int currentTime);

	//更新SV（HOP <= 1的消息不会放入SV）
	//注意：应确保调用之前buffer状态为最新，且需要在每次发送SV、收到SV并计算requestList之前调用此函数
	void updateSummaryVector();

	/**************************************  Prophet  *************************************/

	//待测试
	static void newNodes(int n);

	//待测试
	static void removeNodes(int n);

	void initDeliveryPreds();

	void decayDeliveryPreds(int currentTime);


public:

	static int NUM_NODE_MIN;
	static int NUM_NODE_MAX;
	static int NUM_NODE_INIT;

	static int SLOT_TOTAL;
	static double DEFAULT_DUTY_CYCLE;  //不使用HDC，或者HDC中不在热点区域内时的占空比
	static double HOTSPOT_DUTY_CYCLE;  //HDC中热点区域内的占空比
	static int DEFAULT_SLOT_WAIT;

	static double DEFAULT_DATA_RATE;  //( package / s )
	static int DATA_SIZE;  //( Byte )
	static int CTRL_SIZE;

	static int BUFFER_CAPACITY;
	static int ENERGY;
	static _RECEIVE RECEIVE_MODE;
	static _SEND SEND_MODE;
	static _QUEUE QUEUE_MODE;

	/****************************************  MAC  ***************************************/

	//更新坐标和工作状态，返回是否全部节点都仍存活
	bool updateStatus(int currentTime);

	//注意：所有监听动作都应该在调用此函数判断之后进行，调用此函数之前必须确保已updateStatus
	bool isListening() const;

	//在热点处提高 dc
	void raiseDutyCycle();

	//在非热点处降低 dc
	void resetDutyCycle();

	//如果 recycable 为 true，则可以在动态节点个数测试时被恢复
	//注意：有节点死亡之后，必须调用ClearDeadNodes()
	void die(int currentTime, bool recyclable) 
	{
		this->timeDeath = currentTime;
		this->recyclable = recyclable;
	}

	// TODO: don't send RTS if has received any RTS ?
	bool hasRTS()
	{
		return false;
	}

	CPackage sendRTS(int currentTime);

	void receiveRTS(CPackage package);
	//返回收到 RTS 的所有节点

	CPackage sendCTS(CNode* dst, int currentTime);

	// TODO: 
	void receivePackage(CPackage package);

	static vector<CNode*> broadcastPackage(CPackage package);

	// TODO: 
	static void transmitPackage(CPackage package);

	/**************************************  Prophet  *************************************/

	static double INIT_DELIVERY_PRED;
	static double DECAY_RATIO;
	static double TRANS_RATIO;



	~CNode(){};

	static vector<CNode *>& getNodes();

	static int getNNodes();

	//包括已经失效的节点和删除的节点，按照ID排序
	static vector<CNode *> getAllNodes(bool sort);

	static vector<int>& getIdNodes();

	static bool finiteEnergy();

	static bool hasNodes(int currentTime);

	static void ClearDeadNodes();

	static bool ifNodeExists(int id);

	//该节点不存在时无返回值所以将出错，所以必须在每次调用之前调用函数ifNodeExists()进行检查
	static CNode* getNodeByID(int id);

	static double getSumEnergyConsumption();


	//相遇计数：HAR中统计节点和MA的相遇，否则统计节点间的相遇计数
	//注意：暂时只支持Prophet路由，其他路由尚未添加相关代码
	static int getEncounter() 
	{
		return encounter;
	}

	static int getEncounterAtHotspot() 
	{
		return encounterAtHotspot;
	}

	static int getEncounterActive()
	{
		return encounterActive;
	}

	static double getEncounterAtHotspotPercent() 
	{
		if(encounterAtHotspot == 0)
			return 0.0;
		return double(encounterAtHotspot) / double(encounter);
	}

	static double getEncounterActivePercent() 
	{
		if(encounterActive == 0)
			return 0.0;
		return double(encounterActive) / double(encounter);
	}

	static void encountAtHotspot() 
	{
		encounterAtHotspot++;
		encounter++;
	}

	static void encountOnRoute() 
	{
		encounter++;
	}

	//注意：此函数必须在调用过encountAtHotspot()或encountOnRoute()之后再调用
	static void encountActive() 
	{
		encounterActive++;
	}

	//访问计数：用于统计节点位于热点内的百分比（HAR路由中尚未添加调用）
	static double getVisiterAtHotspotPercent() 
	{
		if(visiterAtHotspot == 0)
			return 0.0;
		return double(visiterAtHotspot) / double(visiterAtHotspot + visiterOnRoute);
	}

	static int getVisiter() 
	{
		return visiterAtHotspot + visiterOnRoute;
	}

	static int getVisiterAtHotspot() 
	{
		return visiterAtHotspot;
	}

	static void visitAtHotspot() 
	{
		visiterAtHotspot++;
	}

	static void visitOnRoute() 
	{
		visiterOnRoute++;
	}

	//不设置能量值时，始终返回true
	bool isAlive() const 
	{
		if( energy == 0 )
			return true;
		else if( energy - energyConsumption <= 0 )
			return false;
		else
			return true;
	}

	double getEnergy() const 
	{
		if( ! isAlive() )
			return 0;
		return energy - energyConsumption;
	}

	bool isRecyclable() const 
	{
		return recyclable;
	}

	double getGenerationRate() const 
	{
		return generationRate;
	}

	CHotspot* getAtHotspot() const 
	{
		return atHotspot;
	}

	void setAtHotspot(CHotspot* atHotspot) 
	{
		this->atHotspot = atHotspot;
	}

	bool isAtHotspot() const 
	{
		if( MAC_PROTOCOL == _hdc )
			return EQUAL( dutyCycle, HOTSPOT_DUTY_CYCLE );
		else if( ROUTING_PROTOCOL == _har )
			return atHotspot != nullptr;
		else
			return false;
	}

	void generateID() 
	{
		ID_COUNT++;
		this->ID = ID_COUNT;		
	}

	double getAverageBufferSize() const 
	{
		if(bufferSizeSum == 0)
			return 0;
		else
			return double(bufferSizeSum) / double(bufferChangeCount);
	}

	//更新buffer状态记录，以便计算buffer status
	void recordBufferStatus();

	vector<CData> sendAllData(_SEND mode) override;

	vector<CData> sendData(int n);

	bool receiveData(int time, vector<CData> datas) override;


	/*************************************  Epidemic  *************************************/

	bool hasSpokenRecently(CNode* node, int currentTime);

	void addToSpokenCache(CNode* node, int t);

	//FIXME: 如果不忽略传输延迟，则以下所有send和receive函数都必须加入传输延迟的计算

	vector<int> sendSummaryVector();

	vector<int> receiveSummaryVector(vector<int> sv);

	//传入对方节点的SV，计算并返回请求传输的数据ID列表
	vector<int> sendRequestList(vector<int> sv);

	vector<int> receiveRequestList(vector<int> req);

	vector<CData> sendDataByRequestList(vector<int> requestList);


	/**************************************  Prophet  *************************************/

	void updateDeliveryPredsWith(int node, map<int, double> preds);

	void updateDeliveryPredsWithSink();

	map<int, double> sendDeliveryPreds();

	map<int, double> receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv);

	vector<CData> sendDataByPredsAndSV(map<int, double> preds, vector<int> &sv);

	//在限定范围内随机增删一定数量的node
	static int ChangeNodeNumber();

};

