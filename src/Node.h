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

	//	int ID;  //node���
	//	CCoordinate location;  //node���ڵ�y����
	//	int time;  //����node���ꡢ����״̬����Prophet�е�˥������ʱ���
	//	bool flag;


protected:

	//TODO: make sure identifier is printed instead of ID in log files
	string nodeIdentifier;  //node identifier read from trace file
	CCTrace* trace;
	double dataRate;
	double dutyCycle;

	//  [ ----------SLEEP----------> | ----WAKE----> )

	int SLOT_SLEEP;  //��WORK_CYCLE��DC����õ�
	int SLOT_WAKE;  //��WORK_CYCLE��DC����õ�

					//��ʱ����INVALID(-1) ��ʾ��ǰ�����ڴ�״̬��0 ��ʾ��״̬��������
	int timerSleep;
	int timerWake;
	int timerCarrierSense;  //�����ز������������ھӽڵ㷢�ֿ�ʼ��ʣ��ʱ��
	bool discovering;  //���ڱ���Ƿ����ڽ����ھӽڵ㷢�֣����η�����Ͻ���false

	int timeLastData;  //��һ���������ɵ�ʱ��
	int timeDeath;  //�ڵ�ʧЧʱ�䣬Ĭ��ֵΪ-1

	int sumTimeAwake;
	int sumTimeAlive;
	//����ͳ������ڵ��buffer״̬��Ϣ
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

	//������
	static vector<CNode*> removeNodes(int n);
	//������
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
	//����������ֵʱ��ʼ�շ���true
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
	//��������node�����ꡢռ�ձȺ͹���״̬���������ݣ�����֮��Ӧ���� isAlive() ������
	void updateStatus(int now);

	//TODO: move into Prophet ?
	CFrame* sendRTSWithCapacityAndIndex(int now);

	bool hasCommunicatedRecently(int nodeId, int now);

	void updateCommunicationHistory(int nodeId, int now);

	//����MODE_SEND == _dumpʱ��ɾ�� ACK ������
	void dropDataByAck(vector<CData> ack);

	// TODO: skip sending RTS if node has received any RTS ? yes if *trans delay countable
	//������յ��������ڵ�� RTS (��δʹ��)
	//bool skipRTS()
	//{
	//	return false;
	//}

	void Overhear(int now) override;

	/*************************** DC��� ***************************/

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
		//�� timerOccupied ������Ҳ��Ϊ�ǻ��ѵ�
		return state == _awake /*|| timerOccupied == 0*/;
	}
	void updateTimerWake(int time);
	void updateTimerSleep(int time);


	//�ж��Ƿ����ھӽڵ㷢��״̬
	//ע�⣺���ô˺���֮ǰ����ȷ����updateStatus
	bool isDiscovering() const
	{
		return discovering == true;
	}

	//��ǿ�ʼ�ھӽڵ㷢��
	void startDiscovering()
	{
		this->discovering = true;
		this->timerCarrierSense = INVALID;
	}

	//��Ǳ����ھӽڵ㷢���Ѿ����
	void finishDiscovering()
	{
		this->discovering = false;
	}

	//���ֹ���֮���ӳ��ز�����
	void delayDiscovering(int timeDelay)
	{
		if( timeDelay <= 0 )
			return;

		this->timerCarrierSense += timeDelay;
		this->discovering = false;
		//�ӳٺ󳬳�����ʱ�ޣ��ӳ����ѣ��ӳ�����
		if( timerCarrierSense >= timerWake )
			delaySleep(timeDelay);
	}

	//���ʣ�໽��ʱ�䲻���������ӳ�����ʱ�䣬�ӳ�����
	void delaySleep(int timeDelay)
	{
		if( timeDelay <= 0 )
			return;
		if( state != _awake )
			return;
		if( timerWake < timeDelay )
			this->timerWake += timeDelay;
	}

	//���ȵ㴦��� dc
	void raiseDutyCycle(double newDutyCycle)
	{
		this->setDutyCycle(newDutyCycle);
		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(WORK_CYCLE * newDutyCycle);
		SLOT_SLEEP = WORK_CYCLE - SLOT_WAKE;
		//����״̬�£��ӳ�����ʱ��
		if(isAwake())
			timerWake += SLOT_WAKE - oldSlotWake;
		//����״̬�£���������
		else
			Wake();
	}
	//�ڷ��ȵ㴦���� dc
	void resetDutyCycle()
	{
		this->setDutyCycle( DEFAULT_DUTY_RATE );
		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(WORK_CYCLE * this->getDutyCycle());
		SLOT_SLEEP = WORK_CYCLE - SLOT_WAKE;
	}


	/*************************** �ܺ���� ***************************/

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
	//�������ڵ������Ƴ��������Ƿ����µĽڵ�����
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
			ASSERT(nAlive - nDead == aliveNodes.size());
			CPrintHelper::PrintAttribute("Node Count", STRING(nAlive) + " -> " + STRING(nAlive - nDead));
		}

		deadNodes.insert(deadNodes.end(), newlyDeadNodes.begin(), newlyDeadNodes.end());
		return death;
	}

	static bool finiteEnergy();

	/****************************  Statistics  *******************************/

	static double getSumEnergyConsumption();

	//����buffer״̬��¼���Ա����buffer status
	void recordBufferStatus();

	double getAverageSizeBuffer() const
	{
		if( sumBufferRecord == 0 )
			return 0;
		else
			return double(sumBufferRecord) / double(countBufferRecord);
	}

	//ͳ�ƻ���ʱ��
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

	//����������HAR��ͳ�ƽڵ��MA������������ͳ�ƽڵ�����������
	//ע�⣺��ʱֻ֧��Prophet·�ɣ�����·����δ�����ش���
	//���п��ܵ���������MAC��·��Э���޹أ�ֻ�����ݼ���ͳ��ʱ���йأ�
	static void encount()
	{
		++encounter;
	}
	//	//��Ч���������ھӽڵ㷢��ʱ���ϵĽڵ�ͼ���ʱ���ϵĽڵ㣨�����������ݴ��䣩������
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
