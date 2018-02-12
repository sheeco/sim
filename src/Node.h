#pragma once

#ifndef __NODE_H__
#define __NODE_H__

#include "Global.h"
#include "Configuration.h"
#include "GeneralNode.h"
#include "Hotspot.h"
#include "Frame.h"
#include "Trace.h"
#include "FileHelper.h"

class CNode :
	virtual public CGeneralNode
{
	//protected:

	//	int ID;  //node编号
	//	CCoordinate location;  //node现在的y坐标
	//	int time;  //更新node坐标、工作状态（、Prophet中的衰减）的时间戳
	//	bool flag;


private:

	CCTrace* trace;
	double dataRate;
	double dutyCycle;

	//  [ ----------SLEEP----------> | ----WAKE----> )

	int SLOT_SLEEP;  //由configs.mac.CYCLE_TOTAL和DC计算得到
	int SLOT_WAKE;  //由configs.mac.CYCLE_TOTAL和DC计算得到

					//计时器：UNVALID(-1) 表示当前不处于此状态；0 表示此状态即将结束
	int timerSleep;
	int timerWake;
	int timerCarrierSense;  //距离载波侦听结束、邻居节点发现开始的剩余时间
	bool discovering;  //用于标记是否正在进行邻居节点发现，本次发现完毕将置false

	int timeLastData;  //上一次数据生成的时间
	int timeDeath;  //节点失效时间，默认值为-1
	CHotspot *atHotspot;

	int sumTimeAwake;
	int sumTimeAlive;
	//用于统计输出节点的buffer状态信息
	int sumBufferRecord;
	int countBufferRecord;
	static int encounterAtHotspot;
	//	static int encounterActiveAtHotspot;
	//	static int encounterActive;  //有效相遇
	static int encounter;
	static int visiterAtHotspot;
	static int visiter;

	static int COUNT_ID;
	static vector<CNode *> nodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
								   // TODO: remove ?
	static vector<CNode *> deadNodes;  //能量耗尽的节点
	static vector<CNode *> deletedNodes;  //用于暂存Node个数动态变化时被暂时移出的节点


	void init();

	CNode();

	CNode(double dataRate);

	~CNode();

	static void initNodes();

	void generateData(int currentTime);


	/****************************************  MAC  ***************************************/

	//将死亡节点整理移出，返回是否有新的节点死亡
	static bool ClearDeadNodes(int currentTime);

	//待测试
	static void removeNodes(int n);
	//待测试
	static void restoreNodes(int n);

	//	CFrame sendCTSWithIndex(CNode* dst, int currentTime);
	//	CFrame sendDataWithIndex(CNode* dst, vector<CData> datas, int currentTime);
	//	vector<CData> bufferData(int time, vector<CData> datas) override;



	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;

	//	vector<CData> getDataBySV(vector<int> sv);
	//	vector<CData> getDataBySV(vector<int> sv, int max);


	/**************************************  Prophet  *************************************/


	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >，sink节点ID为0将位于最前，便于查找


public:

	/****************************************  MAC  ***************************************/

	// TODO: trigger operations here ?
	// *TODO: move even between 2 locations ?
	// TODO: how to trigger RTS ?
	//更新所有node的坐标、占空比和工作状态，生成数据；调用之后应调用 isAlive() 检验存活
	void updateStatus(int currentTime);

	void updateTimerWake(int time);
	void updateTimerSleep(int time);

	//	void receiveRTS(CFrame frame);

	//	void receiveFrame(CFrame* frame, int currentTime) override;

	CFrame* sendRTSWithCapacityAndPred(int currentTime);

	bool hasSpokenRecently(CNode* node, int currentTime);

	void addToSpokenCache(CNode* node, int t);

	//由给出的ID列表提取对应数据
	vector<CData> getDataByRequestList(vector<int> requestList) const;

	//（仅MODE_SEND == _dump时）删除 ACK 的数据
	void checkDataByAck(vector<CData> ack);

	// TODO: skip sending RTS if node has received any RTS ? yes if *trans delay countable
	//标记已收到过其他节点的 RTS (暂未使用)
	//bool skipRTS()
	//{
	//	return false;
	//}

	void Overhear(int currentTime) override;

	/*************************** DC相关 ***************************/

	void Wake() override;
	void Sleep() override;

	bool isAwake() const override
	{
		//将 timerOccupied 的下沿也认为是唤醒的
		return state == _awake /*|| timerOccupied == 0*/;
	}

	//判断是否处于邻居节点发现状态
	//注意：调用此函数之前必须确保已updateStatus
	bool isDiscovering() const
	{
		return discovering == true;
	}

	//标记开始邻居节点发现
	void startDiscovering()
	{
		this->discovering = true;
		this->timerCarrierSense = UNVALID;
	}

	//标记本次邻居节点发现已经完成
	void finishDiscovering()
	{
		this->discovering = false;
	}

	//发现过听之后，延迟载波侦听
	void delayDiscovering(int timeDelay)
	{
		if( timeDelay <= 0 )
			return;

		this->timerCarrierSense += timeDelay;
		this->discovering = false;
		//延迟后超出唤醒时限，则立即休眠
		if( timerCarrierSense >= timerWake )
			Sleep();
	}

	void delaySleep(int timeDelay)
	{
		if( timeDelay <= 0 )
			return;
		if( state != _awake )
			return;

		this->timerWake += timeDelay;
	}

	inline bool useDefaultDutyCycle()
	{
		return EQUAL(dutyCycle, configs.mac.DUTY_RATE);
	}
	inline bool useHotspotDutyCycle()
	{
		return EQUAL(dutyCycle, configs.hdc.HOTSPOT_DUTY_RATE);
	}
	//在热点处提高 dc
	void raiseDutyCycle();
	//在非热点处降低 dc
	void resetDutyCycle();

	/*************************** ------- ***************************/

	/*************************** 能耗相关 ***************************/

	void consumeEnergy(double cons, int currentTime) override
	{
		this->energyConsumption += cons;
		if( configs.node.CAPACITY_ENERGY > 0
		   && configs.node.CAPACITY_ENERGY - energyConsumption <= 0 )
			this->die(currentTime);
	}

	//不设置能量值时，始终返回true
	bool isAlive() const
	{
		return this->timeDeath >= 0;
	}

	double getEnergy() const
	{
		if( !isAlive() )
			return 0;
		return configs.node.CAPACITY_ENERGY - energyConsumption;
	}

	//true if energy has ran out
	//false if trace information has ran out
	//the feature is not used for now
	bool isRecyclable(int currentTime) const
	{
		return this->trace->isValid(currentTime);
	}

	void die(int currentTime)
	{
		this->timeDeath = currentTime;
	}

	static bool finiteEnergy();

	static bool hasNodes(int currentTime);

	static double getSumEnergyConsumption();

	//统计唤醒时间
	int getSumTimeAwake() const
	{
		return sumTimeAwake;
	}
	double getPercentTimeAwake() const
	{
		if( sumTimeAlive == 0 )
			return 0;
		return double(sumTimeAwake) / double(sumTimeAlive);
	}

	/*************************** ------- ***************************/


	/**************************************  Epidemic  *************************************/

	/*************************** 队列管理 ***************************/

	bool isFull()
	{
		return buffer.size() == capacityBuffer;
	}

	//手动将数据压入 buffer，不伴随其他任何操作
	//注意：必须在调用此函数之后手动调用 updateBufferStatus() 检查溢出
	void pushIntoBuffer(vector<CData> datas);

	//返回溢出的数据
	//注意：调用之前应该确保数据已排序
	static vector<CData> removeDataByCapacity(vector<CData> &datas, int capacity, bool fromLeft);

	//	//将删除过期的消息（仅当使用TTL时），返回移出的数据分组
	//	vector<CData> dropOverdueData(int currentTime);

	//将按照(1)“其他节点-本节点”(2)“旧-新”的顺序对buffer中的数据进行排序
	//超出MAX_DATA_RELAY时从前端丢弃数据，溢出时将从前端丢弃数据并返回
	//注意：必须在dropOverdueData之后调用
	vector<CData> dropDataIfOverflow();

	//注意：需要在每次收到数据之后、投递数据之后、生成新数据之后调用此函数
	vector<CData> updateBufferStatus(int currentTime);

	/*************************** ------- ***************************/

	//获取最新 SV（HOP == 0 的消息不会放入SV）
	//注意：应确保调用之前buffer状态为最新
	vector<int> updateSummaryVector();


	/**************************************  Prophet  *************************************/

	map<int, double> getDeliveryPreds() const
	{
		return deliveryPreds;
	}

	void setDeliveryPreds(map<int, double> deliveryPreds)
	{
		this->deliveryPreds = deliveryPreds;
	}

	//返回节点允许接收的最大数据数
	int getCapacityForward();


	static vector<CNode *>& getNodes();

	static int getNodeCount();

	//包括已经失效的节点和删除的节点，按照ID排序
	static vector<CNode *> getAllNodes(bool sort);

	static vector<int> getIdNodes();

	static bool ifNodeExists(int id);

	//该节点不存在时无返回值所以将出错，所以必须在每次调用之前调用函数ifNodeExists()进行检查
	static CNode* getNodeByID(int id);


	/*************************** 相遇 & 访问 & 传输计数 ***************************/

	//相遇计数：HAR中统计节点和MA的相遇，否则统计节点间的相遇计数
	//注意：暂时只支持Prophet路由，其他路由尚未添加相关代码
	//所有可能的相遇（与MAC和路由协议无关，只与数据集和统计时槽有关）
	static void encount()
	{
		++encounter;
	}
	//	//有效相遇，即邻居节点发现时槽上的节点和监听时槽上的节点（即将触发数据传输）的相遇
	//	static void encountActive() 
	//	{
	//		++encounterActive;
	//	}
	//热点区域所有可能的相遇（只与数据集和热点选取有关）
	static void encountAtHotspot()
	{
		++encounterAtHotspot;
	}
	//	//热点区域内的有效相遇，即邻居节点发现时槽上的节点和监听时槽上的节点（即将触发数据传输）的相遇
	//	static void encountActiveAtHotspot() 
	//	{
	//		++encounterActiveAtHotspot;
	//	}

	static int getEncounter()
	{
		return encounter;
	}
	//	static int getEncounterActive()
	//	{
	//		return encounterActive;
	//	}
	static int getEncounterAtHotspot()
	{
		return encounterAtHotspot;
	}
	//	static int getEncounterActiveAtHotspot() 
	//	{
	//		return encounterActiveAtHotspot;
	//	}
	//	//所有有效相遇中，发生在热点区域的比例
	//	static double getPercentEncounterActiveAtHotspot() 
	//	{
	//		if(encounterActiveAtHotspot == 0)
	//			return 0.0;
	//		return double(encounterActiveAtHotspot) / double(encounterActive);
	//	}
	static double getPercentEncounterAtHotspot()
	{
		if( encounterAtHotspot == 0 )
			return 0.0;
		return double(encounterAtHotspot) / double(encounter);
	}
	//	static double getPercentEncounterActive() 
	//	{
	//		if(encounterActive == 0)
	//			return 0.0;
	//		return double(encounterActive) / double(encounter);
	//	}

	//访问计数：用于统计节点位于热点内的百分比（HAR路由中尚未添加调用）
	static void visitAtHotspot()
	{
		++visiterAtHotspot;
	}
	static void visit()
	{
		++visiter;
	}

	static double getPercentVisiterAtHotspot()
	{
		if( visiterAtHotspot == 0 )
			return 0.0;
		return double(visiterAtHotspot) / double(visiter);
	}
	static int getVisiter()
	{
		return visiter;
	}
	static int getVisiterAtHotspot()
	{
		return visiterAtHotspot;
	}

	/*************************** ------- ***************************/

	double getDataByteRate() const
	{
		return dataRate;
	}

	double getDataCountRate() const
	{
		return dataRate / configs.data.SIZE_DATA;
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
		return atHotspot != nullptr;
	}

	void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}
	void loadTrace(string filepath)
	{
		this->trace = CCTrace::readTraceFromFile(filepath);
		string filename = CFileHelper::SplitPath(filepath).second;
		string nodename = CFileHelper::SplitFilename(filename).first;
		this->setName("Node #" + nodename);
	}

	double getAverageSizeBuffer() const
	{
		if( sumBufferRecord == 0 )
			return 0;
		else
			return double(sumBufferRecord) / double(countBufferRecord);
	}

	//更新buffer状态记录，以便计算buffer status
	void recordBufferStatus();

	//	vector<CData> sendAllData(_SEND mode) override;

	//	vector<CData> sendData(int n);

	//	bool receiveData(int time, vector<CData> datas) override;

	/*************************************  Epidemic  *************************************/

	//FIXME: 如果不忽略传输延迟，则以下所有send和receive函数都必须加入传输延迟的计算

	//	vector<int> sendSummaryVector();

	//	vector<int> receiveSummaryVector(vector<int> sv);

	//	//传入对方节点的SV，计算并返回请求传输的数据ID列表
	//	vector<int> sendRequestList(vector<int> sv);

	//	vector<int> receiveRequestList(vector<int> req);


	/**************************************  Prophet  *************************************/

	//	map<int, double> sendDeliveryPreds();
	//
	//	map<int, double> receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv);

};

#endif // __NODE_H__
