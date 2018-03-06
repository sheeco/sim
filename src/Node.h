#pragma once

#ifndef __NODE_H__
#define __NODE_H__

#include "Global.h"
#include "Configuration.h"
#include "GeneralNode.h"
#include "Frame.h"
#include "Trace.h"
#include "FileHelper.h"
#include "PrintHelper.h"

class CNode :
	virtual public CGeneralNode
{
	//protected:

	//	int ID;  //node编号
	//	CCoordinate location;  //node现在的y坐标
	//	int time;  //更新node坐标、工作状态（、Prophet中的衰减）的时间戳
	//	bool flag;


protected:

	string nodeIdentifier;  //node identifier read from trace file
	CCTrace* trace;
	double dataRate;
	double dutyCycle;

	//  [ ----------SLEEP----------> | ----WAKE----> )

	int SLOT_SLEEP;  //由getConfig<int>("mac", "cycle")和DC计算得到
	int SLOT_WAKE;  //由getConfig<int>("mac", "cycle")和DC计算得到

					//计时器：INVALID(-1) 表示当前不处于此状态；0 表示此状态即将结束
	int timerSleep;
	int timerWake;
	int timerCarrierSense;  //距离载波侦听结束、邻居节点发现开始的剩余时间
	bool discovering;  //用于标记是否正在进行邻居节点发现，本次发现完毕将置false

	int timeLastData;  //上一次数据生成的时间
	int timeDeath;  //节点失效时间，默认值为-1

	int sumTimeAwake;
	int sumTimeAlive;
	//用于统计输出节点的buffer状态信息
	int sumBufferRecord;
	int countBufferRecord;
	//TODO: move to proper class
	static int encounter;
	static int visiter;

	static int COUNT_ID;
	static vector<CNode *> allNodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
	static vector<int> idNodes;
								   // TODO: remove ?


	void init();

	CNode();

	CNode(double dataRate);

	static vector<CNode> loadNodesFromFile()
	{
		vector<CNode> allNodes;
		string path = getConfig<string>("trace", "path");
		vector<string> filenames = CFileHelper::ListDirectory(path);
		filenames = CFileHelper::FilterByExtension(filenames, getConfig<string>("trace", "extension_trace_file"));

		if(filenames.empty())
			throw string("CNode::loadNodesFromFile(): Cannot find any trace files under \"" + path + "\".");

		for(int i = 0; i < filenames.size(); ++i)
		{
			double dataRate = getConfig<double>("node", "default_data_rate");
			if(i % 5 == 0)
				dataRate *= 5;
			CNode node;
			node.setDataByteRate(dataRate);
			node.generateID();
			node.loadTrace(filenames[i]);

			allNodes.push_back(node);
		}
		return allNodes;
	}
	static bool setNodes(vector<CNode*> nodes);
	void generateData(int now);


	/****************************************  MAC  ***************************************/

	//待测试
	static vector<CNode*> removeNodes(int n);
	//待测试
	static vector<CNode*> restoreNodes(vector<CNode*> removedNodes, int n);

	//	CFrame sendCTSWithIndex(CNode* dst, int now);
	//	CFrame sendDataWithIndex(CNode* dst, vector<CData> datas, int now);
	//	vector<CData> bufferData(int time, vector<CData> datas) override;



	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;


	/**************************************  Prophet  *************************************/


	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >，sink节点ID为0将位于最前，便于查找


public:

	~CNode();

	CCTrace getTrace() const
	{
		return *(this->trace);
	}

	/****************************************  MAC  ***************************************/

	// TODO: trigger operations here ?
	// *TODO: move even between 2 locations ?
	// TODO: how to trigger RTS ?
	//更新所有node的坐标、占空比和工作状态，生成数据；调用之后应调用 isAlive() 检验存活
	void updateStatus(int now);

	void updateTimerWake(int time);
	void updateTimerSleep(int time);

	//	void receiveRTS(CFrame frame);

	//	void receiveFrame(CFrame* frame, int now) override;

	CFrame* sendRTSWithCapacityAndPred(int now);

	bool hasSpokenRecently(CNode* node, int now);

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

	void Overhear(int now) override;

	/*************************** DC相关 ***************************/

	void Wake() override;
	void Sleep() override;

	void setDutyCycle(double dutyCycle)
	{
		this->dutyCycle = dutyCycle;
	}
	double getDutyCycle() const
	{
		return this->dutyCycle;
	}
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
		this->timerCarrierSense = INVALID;
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

	//在热点处提高 dc
	void raiseDutyCycle(double newDutyCycle)
	{
		this->setDutyCycle(newDutyCycle);
		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(getConfig<int>("mac", "cycle") * newDutyCycle);
		SLOT_SLEEP = getConfig<int>("mac", "cycle") - SLOT_WAKE;
		//唤醒状态下，延长唤醒时间
		if(isAwake())
			timerWake += SLOT_WAKE - oldSlotWake;
		//休眠状态下，立即唤醒
		else
			Wake();
	}
	//在非热点处降低 dc
	void resetDutyCycle()
	{
		this->setDutyCycle( getConfig<double>("hdc", "hotspot_duty_rate") );
		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(getConfig<int>("mac", "cycle") * this->getDutyCycle());
		SLOT_SLEEP = getConfig<int>("mac", "cycle") - SLOT_WAKE;
	}

	/*************************** ------- ***************************/

	/*************************** 能耗相关 ***************************/

	void consumeEnergy(double cons, int now) override
	{
		this->energyConsumption += cons;
		if( getConfig<int>("node", "energy") > 0
		   && getConfig<int>("node", "energy") - energyConsumption <= 0 )
		{
			this->die(now);
			CPrintHelper::PrintDetail(now, this->getName() + " dies of energy exhaustion.");
		}
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
		return getConfig<int>("node", "energy") - energyConsumption;
	}

	//true if energy has ran out
	//false if trace information has ran out
	//the feature is not used for now
	bool isRecyclable(int now) const
	{
		return this->trace->isValid(now);
	}

	void die(int now)
	{
		this->timeDeath = now;
	}

	template <class T>
	static T* downcast(CNode* node)
	{
		return dynamic_cast< T* >( node );
	}
	template <class T>
	static CNode* upcast(T* node)
	{
		return dynamic_cast< CNode* >( node );
	}
	template <class T>
	static vector<T*> downcast(vector<CNode*> nodes)
	{
		vector<T*> res;
		for(CNode* node : nodes)
			res.push_back(downcast<T>(node));
		return res;
	}
	template <class T>
	static vector<CNode*> upcast(vector<T*> nodes)
	{
		vector<CNode*> res;
		for(T* node : nodes)
			res.push_back(upcast<T>(node));
		return res;
	}
	// TODO: change to the new implementation below
	//将死亡节点整理移出，返回是否有新的节点死亡
	template <class T>
	static bool ClearDeadNodes(vector<T*> &aliveList, vector<T*> &deadList, int now)
	{
		vector<CNode*> aliveNodes, deadNodes;
		aliveNodes = upcast<T>(aliveList);
		deadNodes = upcast<T>(deadList);
		bool death = false;
		for(vector<CNode *>::iterator ipNode = aliveNodes.begin(); ipNode != aliveNodes.end(); )
		{
			if(!( *ipNode )->isAlive())
			{
				CPrintHelper::PrintDetail(now, ( *ipNode )->getName() + " is dead.");
				death = true;
				aliveNodes.push_back(*ipNode);
				ipNode = aliveNodes.erase(ipNode);
			}
			else
				++ipNode;
		}

		if(death)
		{
			ofstream death(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_death"), ios::app);
			if(now == 0)
			{
				death << endl << getConfig<string>("log", "info_log") << endl;
				death << getConfig<string>("log", "info_death") << endl;
			}
			death << now << TAB << aliveNodes.size() << TAB << CData::getCountDelivery()
				<< TAB << CData::getDeliveryRatio() << endl;
			death.close();

			CPrintHelper::PrintAttribute("Node Count", aliveNodes.size());
		}

		aliveList = downcast<T>(aliveNodes);
		deadList = downcast<T>(deadNodes);
		return death;
	}

	static bool finiteEnergy();

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

	//将按照(1)“其他节点-本节点”(2)“旧-新”的顺序对buffer中的数据进行排序
	//超出MAX_DATA_RELAY时从前端丢弃数据，溢出时将从前端丢弃数据并返回
	//注意：必须在dropOverdueData之后调用
	vector<CData> dropDataIfOverflow();

	//注意：需要在每次收到数据之后、投递数据之后、生成新数据之后调用此函数
	vector<CData> updateBufferStatus(int now);

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


	static vector<CNode *>& getAllNodes();

	static int getNodeCount();

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
	static int getEncounter()
	{
		return encounter;
	}
	//	static int getEncounterActive()
	//	{
	//		return encounterActive;
	//	}
	static void visit()
	{
		++visiter;
	}

	static int getVisiter()
	{
		return visiter;
	}

	/*************************** ------- ***************************/

	void setDataByteRate(double dataRate)
	{
		this->dataRate = dataRate;
	}
	double getDataByteRate() const
	{
		return dataRate;
	}
	double getDataCountRate() const
	{
		return dataRate / getConfig<int>("data", "size_data");
	}

	void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}
	void setIdentifier(string nodeIdentifier)
	{
		this->nodeIdentifier = nodeIdentifier;
	}
	string getIdentifier()
	{
		return this->nodeIdentifier;
	}
	void loadTrace(string filepath)
	{
		this->trace = CCTrace::readTraceFromFile(filepath, getConfig<bool>("trace", "continuous"));
		string filename = CFileHelper::SplitPath(filepath).second;
		string nodename = CFileHelper::SplitFilename(filename).first;
		this->setIdentifier(nodename);
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


	/**************************************  Prophet  *************************************/

	//	map<int, double> sendDeliveryPreds();
	//
	//	map<int, double> receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv);

};

#endif // __NODE_H__
