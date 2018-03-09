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

	int SLOT_SLEEP;  //��getConfig<int>("mac", "cycle")��DC����õ�
	int SLOT_WAKE;  //��getConfig<int>("mac", "cycle")��DC����õ�

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
	static vector<CNode *> allNodes;  //���ڴ������д������ڵ�Ļ�����ָ�뿽��
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
	//TODO: check all the static methods for CNode
	//TODO: is it necessary to create inhertied node type for each protocol ?
	static void setNodes(vector<CNode*> nodes);
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

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;


	/**************************************  Prophet  *************************************/


	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���


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
	//��������node�����ꡢռ�ձȺ͹���״̬���������ݣ�����֮��Ӧ���� isAlive() ������
	void updateStatus(int now);

	void updateTimerWake(int time);
	void updateTimerSleep(int time);

	//	void receiveRTS(CFrame frame);

	//	void receiveFrame(CFrame* frame, int now) override;

	CFrame* sendRTSWithCapacityAndPred(int now);

	bool hasSpokenRecently(CNode* node, int now);

	void addToSpokenCache(CNode* node, int t);

	//�ɸ�����ID�б���ȡ��Ӧ����
	vector<CData> getDataByRequestList(vector<int> requestList) const;

	//����MODE_SEND == _dumpʱ��ɾ�� ACK ������
	void checkDataByAck(vector<CData> ack);

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
		//�ӳٺ󳬳�����ʱ�ޣ�����������
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

	//���ȵ㴦��� dc
	void raiseDutyCycle(double newDutyCycle)
	{
		this->setDutyCycle(newDutyCycle);
		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(getConfig<int>("mac", "cycle") * newDutyCycle);
		SLOT_SLEEP = getConfig<int>("mac", "cycle") - SLOT_WAKE;
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
		this->setDutyCycle( getConfig<double>("hdc", "hotspot_duty_rate") );
		int oldSlotWake = SLOT_WAKE;
		SLOT_WAKE = int(getConfig<int>("mac", "cycle") * this->getDutyCycle());
		SLOT_SLEEP = getConfig<int>("mac", "cycle") - SLOT_WAKE;
	}

	/*************************** ------- ***************************/

	/*************************** �ܺ���� ***************************/

	void consumeEnergy(double cons, int now) override
	{
		this->energyConsumption += cons;
		if( getConfig<int>("node", "energy") > 0
		   && getConfig<int>("node", "energy") - energyConsumption <= 0 )
		{
			this->die(now);
			CPrintHelper::PrintContent(now, this->getName() + " dies of energy exhaustion.");
		}
	}

	//����������ֵʱ��ʼ�շ���true
	bool isAlive() const
	{
		return ! (this->timeDeath >= 0);
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
		this->setTime(now);
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
	//�������ڵ������Ƴ��������Ƿ����µĽڵ�����
	template <class T>
	static bool ClearDeadNodes(vector<T*> &aliveList, vector<T*> &deadList, int now)
	{
		vector<CNode*> aliveNodes, newlyDeadNodes;
		aliveNodes = upcast<T>(aliveList);
		bool death = false;
		for(vector<CNode *>::iterator ipNode = aliveNodes.begin(); ipNode != aliveNodes.end(); )
		{
			if(!( *ipNode )->isAlive())
			{
				death = true;
				newlyDeadNodes.push_back(*ipNode);
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
			int nAlive = aliveList.size();
			for(CNode* pdead : newlyDeadNodes )
				death << now << TAB << pdead->getIdentifier() << TAB << --nAlive << TAB << CData::getCountDelivery() 
					<< TAB << CData::getDeliveryRatio() << endl;
			death.close();
			ASSERT(nAlive == aliveNodes.size());
			CPrintHelper::PrintAttribute("Node Count", nAlive);
		}

		aliveList = downcast<T>(aliveNodes);
		vector<T*> newlyDeadList = downcast<T>(newlyDeadNodes);
		deadList.insert(deadList.end(), newlyDeadList.begin(), newlyDeadList.end());
		return death;
	}

	static bool finiteEnergy();

	static double getSumEnergyConsumption();

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

	/*************************** ------- ***************************/


	/*************************** ���й��� ***************************/

	bool isFull()
	{
		return buffer.size() == capacityBuffer;
	}

	//�ֶ�������ѹ�� buffer�������������κβ���
	//ע�⣺�����ڵ��ô˺���֮���ֶ����� updateBufferStatus() ������
	void pushIntoBuffer(vector<CData> datas);

	//�������������
	//ע�⣺����֮ǰӦ��ȷ������������
	static vector<CData> removeDataByCapacity(vector<CData> &datas, int capacity, bool fromLeft);

	//������(1)�������ڵ�-���ڵ㡱(2)����-�¡���˳���buffer�е����ݽ�������
	//����MAX_DATA_RELAYʱ��ǰ�˶������ݣ����ʱ����ǰ�˶������ݲ�����
	//ע�⣺������dropOverdueData֮�����
	vector<CData> dropDataIfOverflow();

	//ע�⣺��Ҫ��ÿ���յ�����֮��Ͷ������֮������������֮����ô˺���
	vector<CData> updateBufferStatus(int now);

	/*************************** ------- ***************************/

	//��ȡ���� SV��HOP == 0 ����Ϣ�������SV��
	//ע�⣺Ӧȷ������֮ǰbuffer״̬Ϊ����
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

	//���ؽڵ�������յ����������
	int getCapacityForward();


	static vector<CNode *>& getAllNodes();

	static int getNodeCount();

	static vector<int> getIdNodes();
	static vector<int> getIdNodes(vector<CNode*> nodes)
	{
		vector<int> ids;
		for(CNode * inode : nodes)
			ids.push_back(inode->getID());
		return ids;
	}

	static bool ifNodeExists(int id);

	//�ýڵ㲻����ʱ�޷���ֵ���Խ��������Ա�����ÿ�ε���֮ǰ���ú���ifNodeExists()���м��
	static CNode* getNodeByID(int id);


	/*************************** ���� & ���� & ������� ***************************/

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

	//����buffer״̬��¼���Ա����buffer status
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
