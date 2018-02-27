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
	CHotspot *atHotspot;

	int sumTimeAwake;
	int sumTimeAlive;
	//����ͳ������ڵ��buffer״̬��Ϣ
	int sumBufferRecord;
	int countBufferRecord;
	static int encounterAtWaypoint;
	//	static int encounterActiveAtHotspot;
	//	static int encounterActive;  //��Ч����
	static int encounter;
	static int visiterAtWaypoint;
	static int visiter;

	static int COUNT_ID;
	static vector<CNode *> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
								   // TODO: remove ?
	static vector<CNode *> deadNodes;  //�����ľ��Ľڵ�
	static vector<CNode *> deletedNodes;  //�����ݴ�Node������̬�仯ʱ����ʱ�Ƴ��Ľڵ�


	void init();

	CNode();

	CNode(double dataRate);

	~CNode();

	static void initNodes();

	void generateData(int currentTime);


	/****************************************  MAC  ***************************************/

	//������
	static void removeNodes(int n);
	//������
	static void restoreNodes(int n);

	//	CFrame sendCTSWithIndex(CNode* dst, int currentTime);
	//	CFrame sendDataWithIndex(CNode* dst, vector<CData> datas, int currentTime);
	//	vector<CData> bufferData(int time, vector<CData> datas) override;



	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;


	/**************************************  Prophet  *************************************/


	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���


public:

	CCTrace getTrace() const
	{
		return *(this->trace);
	}

	/****************************************  MAC  ***************************************/

	// TODO: trigger operations here ?
	// *TODO: move even between 2 locations ?
	// TODO: how to trigger RTS ?
	//��������node�����ꡢռ�ձȺ͹���״̬���������ݣ�����֮��Ӧ���� isAlive() ������
	void updateStatus(int currentTime);

	void updateTimerWake(int time);
	void updateTimerSleep(int time);

	//	void receiveRTS(CFrame frame);

	//	void receiveFrame(CFrame* frame, int currentTime) override;

	CFrame* sendRTSWithCapacityAndPred(int currentTime);

	bool hasSpokenRecently(CNode* node, int currentTime);

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

	void Overhear(int currentTime) override;

	/*************************** DC��� ***************************/

	void Wake() override;
	void Sleep() override;

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

	inline bool useDefaultDutyCycle()
	{
		return EQUAL(dutyCycle, getConfig<double>("mac", "duty_rate"));
	}
	inline bool useHotspotDutyCycle()
	{
		return EQUAL(dutyCycle, getConfig<double>("hdc", "hotspot_duty_rate"));
	}
	//���ȵ㴦��� dc
	void raiseDutyCycle();
	//�ڷ��ȵ㴦���� dc
	void resetDutyCycle();

	/*************************** ------- ***************************/

	/*************************** �ܺ���� ***************************/

	void consumeEnergy(double cons, int currentTime) override
	{
		this->energyConsumption += cons;
		if( getConfig<int>("node", "energy") > 0
		   && getConfig<int>("node", "energy") - energyConsumption <= 0 )
		{
			this->die(currentTime);
			CPrintHelper::PrintDetail(currentTime, this->getName() + " dies of energy exhaustion.");
		}
	}

	//����������ֵʱ��ʼ�շ���true
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
	bool isRecyclable(int currentTime) const
	{
		return this->trace->isValid(currentTime);
	}

	void die(int currentTime)
	{
		this->timeDeath = currentTime;
	}

	//�������ڵ������Ƴ��������Ƿ����µĽڵ�����
	static bool ClearDeadNodes(int currentTime);
	// TODO: change to the new implementation below
	static bool ClearDeadNodes(vector<CNode*> &aliveList, vector<CNode*> &deadList, int now);

	static bool finiteEnergy();

	static bool hasNodes(int currentTime);

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
	vector<CData> updateBufferStatus(int currentTime);

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


	static vector<CNode *>& getNodes();

	static int getNodeCount();

	//�����Ѿ�ʧЧ�Ľڵ��ɾ���Ľڵ㣬����ID����
	static vector<CNode *> getAllNodes(bool sort);

	static vector<int> getIdNodes();

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
	//�ȵ��������п��ܵ�������ֻ�����ݼ����ȵ�ѡȡ�йأ�
	static void encountAtWaypoint()
	{
		++encounterAtWaypoint;
	}
	//	//�ȵ������ڵ���Ч���������ھӽڵ㷢��ʱ���ϵĽڵ�ͼ���ʱ���ϵĽڵ㣨�����������ݴ��䣩������
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
	static int getEncounterAtWaypoint()
	{
		return encounterAtWaypoint;
	}
	//	static int getEncounterActiveAtHotspot() 
	//	{
	//		return encounterActiveAtHotspot;
	//	}
	//	//������Ч�����У��������ȵ�����ı���
	//	static double getPercentEncounterActiveAtHotspot() 
	//	{
	//		if(encounterActiveAtHotspot == 0)
	//			return 0.0;
	//		return double(encounterActiveAtHotspot) / double(encounterActive);
	//	}
	static double getPercentEncounterAtWaypoint()
	{
		if( encounterAtWaypoint == 0 )
			return 0.0;
		return double(encounterAtWaypoint) / double(encounter);
	}
	//	static double getPercentEncounterActive() 
	//	{
	//		if(encounterActive == 0)
	//			return 0.0;
	//		return double(encounterActive) / double(encounter);
	//	}

	//���ʼ���������ͳ�ƽڵ�λ���ȵ��ڵİٷֱȣ�HAR·������δ��ӵ��ã�
	static void visitAtHotspot()
	{
		++visiterAtWaypoint;
	}
	static void visit()
	{
		++visiter;
	}

	static double getPercentVisiterAtHotspot()
	{
		if( visiterAtWaypoint == 0 )
			return 0.0;
		return double(visiterAtWaypoint) / double(visiter);
	}
	static int getVisiter()
	{
		return visiter;
	}
	static int getVisiterAtHotspot()
	{
		return visiterAtWaypoint;
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

	CHotspot* getAtHotspot() const
	{
		return atHotspot;
	}

	void setAtWaypoint(CHotspot* atHotspot)
	{
		this->atHotspot = atHotspot;
	}

	bool isAtWaypoint() const
	{
		return atHotspot != nullptr;
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
