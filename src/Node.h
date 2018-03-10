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

	//TODO: make sure identifier is printed instead of ID in log files
	string nodeIdentifier;  //node identifier read from trace file
	CCTrace* trace;
	double dataRate;
	double dutyCycle;

	//  [ ----------SLEEP----------> | ----WAKE----> )

	int SLOT_SLEEP;  //由WORK_CYCLE和DC计算得到
	int SLOT_WAKE;  //由WORK_CYCLE和DC计算得到

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
	static vector<CNode*> allNodes;
	static vector<CNode*> aliveNodes;
	static vector<CNode*> deadNodes;
	static map<int, CNode*> mapAllNodes;
	//TODO: remove
	static vector<int> idNodes;

	static double DEFAULT_DATA_RATE;
	static int DEFAULT_CAPACITY_BUFFER;
	static int DEFAULT_CAPACITY_ENERGY;
	static int WORK_CYCLE;
	static double DEFAULT_DUTY_RATE;

	/***********************  Initialization  ************************/

	void init();

	CNode();

	CNode(double dataRate);

	void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}
	void setIdentifier(string nodeIdentifier)
	{
		this->nodeIdentifier = nodeIdentifier;
	}
	void loadTrace(string filepath)
	{
		this->trace = CCTrace::readTraceFromFile(filepath, getConfig<bool>("trace", "continuous"));
		string filename = CFileHelper::SplitPath(filepath).second;
		string nodename = CFileHelper::SplitFilename(filename).first;
		this->setIdentifier(nodename);
		this->setName("Node #" + nodename);
	}
	static vector<CNode*> loadNodesFromFile()
	{
		vector<CNode*> nodes;
		string path = getConfig<string>("trace", "path");
		vector<string> filenames = CFileHelper::ListDirectory(path);
		filenames = CFileHelper::FilterByExtension(filenames, getConfig<string>("trace", "extension_trace_file"));

		if(filenames.empty())
			throw string("CNode::loadNodesFromFile(): Cannot find any trace files under \"" + path + "\".");

		for(int i = 0; i < filenames.size(); ++i)
		{
			double dataRate = DEFAULT_DATA_RATE;
			if(i % 5 == 0)
				dataRate *= 5;
			CNode* pnode = new CNode();
			pnode->setDataByteRate(dataRate);
			pnode->generateID();
			pnode->loadTrace(filenames[i]);

			nodes.push_back(pnode);
			CPrintHelper::PrintBrief(pnode->getName() + " is loaded from trace file " + filenames[i] + ". (" 
									   + STRING(nodes.size()) + " in total)");
		}
		return nodes;
	}

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

	//vector<int> summaryVector;
	static int LIFETIME_COMMUNICATION_HISROTY;
	map<int, int> communicationHistory;  // (nodeId, lastCommunicationTime)


public:

	~CNode();

	/***********************  Setters & Getters  ************************/

	string getIdentifier()
	{
		return this->nodeIdentifier;
	}

	//返回节点允许接收的最大数据数
	int getCapacityForward();

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
	CCTrace getTrace() const
	{
		return *(this->trace);
	}


	/***********************  Alive / Dead  ************************/

	void die(int now)
	{
		this->timeDeath = now;
		this->setTime(now);
	}
	//不设置能量值时，始终返回true
	bool isAlive() const
	{
		return !( this->timeDeath >= 0 );
	}
	//true if energy has ran out
	//false if trace information has ran out
	//the feature is not used for now
	bool isRecyclable(int now) const
	{
		return this->trace->isValid(now);
	}

	/****************************************  MAC  ***************************************/

	// TODO: trigger operations here ?
	// TODO: move even between 2 locations ?
	// TODO: how to trigger RTS ?
	//更新所有node的坐标、占空比和工作状态，生成数据；调用之后应调用 isAlive() 检验存活
	void updateStatus(int now);

	CFrame* sendRTSWithCapacityAndIndex(int now);

	bool hasCommunicatedRecently(int nodeId, int now);

	void updateCommunicationHistory(int nodeId, int now);

	//（仅MODE_SEND == _dump时）删除 ACK 的数据
	void dropDataByAck(vector<CData> ack);

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
	void updateTimerWake(int time);
	void updateTimerSleep(int time);


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
		SLOT_WAKE = int(WORK_CYCLE * newDutyCycle);
		SLOT_SLEEP = WORK_CYCLE - SLOT_WAKE;
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
		this->setDutyCycle( DEFAULT_DUTY_RATE );
		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(WORK_CYCLE * this->getDutyCycle());
		SLOT_SLEEP = WORK_CYCLE - SLOT_WAKE;
	}


	/*************************** 能耗相关 ***************************/

	void consumeEnergy(double cons, int now) override
	{
		CGeneralNode::consumeEnergy(cons, now);

		if( !hasEnergyLeft() )
		{
			this->die(now);
			CPrintHelper::PrintBrief(now, this->getName() + " dies of energy exhaustion.");
		}
	}


	/****************************  Static Methods  *******************************/

	static int getCountAliveNodes();

	static vector<int> getIdNodes(vector<CNode*> nodes)
	{
		vector<int> ids;
		for(CNode * inode : nodes)
			ids.push_back(inode->getID());
		return ids;
	}

	static void Init(int now)
	{
		DEFAULT_DATA_RATE = getConfig<double>("node", "default_data_rate");
		DEFAULT_CAPACITY_BUFFER = getConfig<int>("node", "buffer");
		DEFAULT_CAPACITY_ENERGY = getConfig<int>("node", "energy");
		WORK_CYCLE = getConfig<int>("mac", "cycle");
		DEFAULT_DUTY_RATE = getConfig<double>("mac", "duty_rate");

		LIFETIME_COMMUNICATION_HISROTY = getConfig<int>("node", "lifetime_communication_history");

		allNodes = CNode::loadNodesFromFile();
		aliveNodes = allNodes;
		mapAllNodes.clear();
		for( CNode* pnode : allNodes )
			mapAllNodes[pnode->getID()] = pnode;
		deadNodes.clear();
	}
	//TODO: check all the static methods for CNode
	static bool hasNodes()
	{
		return !aliveNodes.empty();
	}
	static vector<CNode*> getAllNodes()
	{
		return allNodes;
	}
	static vector<CNode*> getAliveNodes()
	{
		return aliveNodes;
	}
	static vector<CNode*> getDeadNodes()
	{
		return deadNodes;
	}
	static CNode* findNodeByID(int id)
	{
		map<int, CNode*>::iterator imap = mapAllNodes.find(id);
		if( imap == mapAllNodes.end() )
			return nullptr;
		else
			return imap->second;
	}
	static bool UpdateNodeStatus(int now)
	{
		//update basic status
		vector<CNode *> nodes = aliveNodes;
		for( CNode * pnode : aliveNodes )
			pnode->updateStatus(now);

		CNode::ClearDeadNodes(now);

		return hasNodes();
	}
	//将死亡节点整理移出，返回是否有新的节点死亡
	static bool ClearDeadNodes(int now)
	{
		int nAlive = aliveNodes.size();
		vector<CNode*> newlyDeadNodes;
		bool death = false;
		for( vector<CNode *>::iterator ipNode = aliveNodes.begin(); ipNode != aliveNodes.end(); )
		{
			if( !( *ipNode )->isAlive() )
			{
				death = true;
				newlyDeadNodes.push_back(*ipNode);
				ipNode = aliveNodes.erase(ipNode);
			}
			else
				++ipNode;
		}

		if( death )
		{
			int nDead = 0;
			ofstream death(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_death"), ios::app);
			if( now == 0 )
			{
				death << endl << getConfig<string>("log", "info_log") << endl;
				death << getConfig<string>("log", "info_death") << endl;
			}
			for( CNode* pdead : newlyDeadNodes )
				death << now << TAB << pdead->getIdentifier() << TAB << nAlive - (++nDead) << TAB << CData::getCountDelivery()
				<< TAB << CData::getDeliveryRatio() << endl;
			death.close();
			ASSERT(nAlive == aliveNodes.size());
			CPrintHelper::PrintAttribute("Node Count", STRING(nAlive) + " -> " + STRING(nAlive - nDead));
		}

		deadNodes.insert(deadNodes.end(), newlyDeadNodes.begin(), newlyDeadNodes.end());
		return death;
	}

	static bool finiteEnergy();

	/****************************  Statistics  *******************************/

	static double getSumEnergyConsumption();

	//更新buffer状态记录，以便计算buffer status
	void recordBufferStatus();

	double getAverageSizeBuffer() const
	{
		if( sumBufferRecord == 0 )
			return 0;
		else
			return double(sumBufferRecord) / double(countBufferRecord);
	}

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

};

#endif // __NODE_H__
