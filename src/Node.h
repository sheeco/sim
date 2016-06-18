#pragma once

#ifndef __NODE_H__
#define __NODE_H__

#include <map>
#include "GeneralNode.h"
#include "Hotspot.h"
#include "Frame.h"
#include "Trace.h"

class CNode :
	virtual public CGeneralNode
{
//protected:

//	int ID;  //node���
//	CCoordinate location;  //node���ڵ�y����
//	int time;  //����node���ꡢ����״̬����Prophet�е�˥������ʱ���
//	bool flag;


private:

	CCTrace* trace;
	double dataRate;
	double dutyCycle;

	//  [ ----------SLEEP----------> | ----WAKE----> )

	int SLOT_SLEEP;  //��SLOT_TOTAL��DC����õ�
	int SLOT_WAKE;  //��SLOT_TOTAL��DC����õ�

	//��ʱ����UNVALID(-1) ��ʾ��ǰ�����ڴ�״̬��0 ��ʾ��״̬��������
	int timerSleep;
	int timerWake;
	int timerCarrierSense;  //�����ز������������ھӽڵ㷢�ֿ�ʼ��ʣ��ʱ��
	bool discovering;  //���ڱ���Ƿ����ڽ����ھӽڵ㷢�֣����η�����Ͻ���false

	int timeLastData;  //��һ���������ɵ�ʱ��
	int timeDeath;  //�ڵ�ʧЧʱ�䣬Ĭ��ֵΪ-1
	bool recyclable;  //�ڽڵ�����֮��ָʾ�ڵ��Ƿ��Կɱ����գ�Ĭ��Ϊ true��ֱ�� trace ��Ϣ��ֹ�� false����δʹ�ã�����г����Ϊ��Ӧ�ö�ȡ�˲�����
	CHotspot *atHotspot;

	int sumTimeAwake;
	int sumTimeAlive;
	//����ͳ������ڵ��buffer״̬��Ϣ
	int sumBufferRecord;
	int countBufferRecord;
	static int encounterAtHotspot;
//	static int encounterActiveAtHotspot;
//	static int encounterActive;  //��Ч����
	static int encounter;
	static int visiterAtHotspot;
	static int visiter;

	static int COUNT_ID;
	static vector<CNode *> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���
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
	static void newNodes(int n);
	//������
	static void removeNodes(int n);

//	CFrame sendCTSWithIndex(CNode* dst, int currentTime);
//	CFrame sendDataWithIndex(CNode* dst, vector<CData> datas, int currentTime);
//	vector<CData> bufferData(int time, vector<CData> datas) override;



	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;

//	vector<CData> getDataBySV(vector<int> sv);
//	vector<CData> getDataBySV(vector<int> sv, int max);


	/**************************************  Prophet  *************************************/


	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���


public:

	static int MIN_NUM_NODE;
	static int MAX_NUM_NODE;
	static int INIT_NUM_NODE;

	static int SLOT_TOTAL;
	static double DEFAULT_DUTY_CYCLE;  //��ʹ��HDC������HDC�в����ȵ�������ʱ��ռ�ձ�
	static double HOTSPOT_DUTY_CYCLE;  //HDC���ȵ������ڵ�ռ�ձ�
	static int DEFAULT_SLOT_CARRIER_SENSE;  //����RTS֮ǰ���ز�������ʱ��

	static double DEFAULT_DATA_RATE;  //( frame / s )
	static int SIZE_DATA;  //( Byte )

	static int CAPACITY_BUFFER;
	static int CAPACITY_ENERGY;
	// TODO:
	static int SPEED_TRANS;  // Byte / s
	static int LIFETIME_SPOKEN_CACHE;  //�����ʱ���ڽ��������ݵĽڵ���ʱ���ٽ�������
	static _RECEIVE MODE_RECEIVE;
	static _SEND MODE_SEND;
	static _QUEUE MODE_QUEUE;

	/****************************************  MAC  ***************************************/

	//���޶���Χ�������ɾһ��������node
	static int ChangeNodeNumber();

	// TODO: trigger operations here ?
	// *TODO: move even between 2 locations ?
	// TODO: how to trigger RTS ?
	//��������node�����ꡢռ�ձȺ͹���״̬���������ݣ�����֮��Ӧ���� isAlive() ������
	void updateStatus(int currentTime);

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
	bool skipRTS()
	{
		return false;
	}


	/*************************** DC��� ***************************/

	void Wake() override;
	void Sleep() override;

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
		this->timerCarrierSense = UNVALID;
	}

	//��Ǳ����ھӽڵ㷢���Ѿ����
	void finishDiscovering()
	{
		this->discovering = false;
	}

	inline bool useDefaultDutyCycle()
	{
		return EQUAL(dutyCycle, DEFAULT_DUTY_CYCLE);
	}
	inline bool useHotspotDutyCycle()
	{
		return EQUAL(dutyCycle, HOTSPOT_DUTY_CYCLE);
	}
	//���ȵ㴦��� dc
	void raiseDutyCycle();
	//�ڷ��ȵ㴦���� dc
	void resetDutyCycle();

	/*************************** ------- ***************************/

	/*************************** �ܺ���� ***************************/

	//����������ֵʱ��ʼ�շ���true
	bool isAlive() const 
	{
		if( CAPACITY_ENERGY == 0 )
			return true;
		else if( CAPACITY_ENERGY - energyConsumption <= 0 )
			return false;
		else
			return true;
	}

	double getEnergy() const 
	{
		if( ! isAlive() )
			return 0;
		return CAPACITY_ENERGY - energyConsumption;
	}

	bool isRecyclable() const 
	{
		return recyclable;
	}

	//��� recycable Ϊ true��������ڶ�̬�ڵ��������ʱ���ָ�
	//ע�⣺�нڵ�����֮�󣬱������ClearDeadNodes()
	void die(int currentTime, bool recyclable) 
	{
		this->timeDeath = currentTime;
		this->recyclable = recyclable;
	}

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

	//�������ڵ������Ƴ�
	static void ClearDeadNodes();


	/*************************** ------- ***************************/


	/**************************************  Epidemic  *************************************/

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
	static vector<CData> removeDataByCapacity(vector<CData> &datas, int capacity);

//	//��ɾ�����ڵ���Ϣ������ʹ��TTLʱ���������Ƴ������ݷ���
//	vector<CData> dropOverdueData(int currentTime);

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

	static int getNNodes();

	//�����Ѿ�ʧЧ�Ľڵ��ɾ���Ľڵ㣬����ID����
	static vector<CNode *> getAllNodes(bool sort);

	static vector<int>& getIdNodes();

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
	static void encountAtHotspot() 
	{
		++encounterAtHotspot;
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
	static int getEncounterAtHotspot() 
	{
		return encounterAtHotspot;
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
	static double getPercentEncounterAtHotspot() 
	{
		if(encounterAtHotspot == 0)
			return 0.0;
		return double(encounterAtHotspot) / double(encounter);
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
		++visiterAtHotspot;
	}
	static void visit() 
	{
		++visiter;
	}

	static double getPercentVisiterAtHotspot() 
	{
		if(visiterAtHotspot == 0)
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

	double getDataRate() const 
	{
		return dataRate;
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

	void loadTrace()
	{
		trace = CCTrace::getTraceFromFile(CCTrace::getTraceFilename(this->ID));
	}

	double getAverageSizeBuffer() const 
	{
		if(sumBufferRecord == 0)
			return 0;
		else
			return double(sumBufferRecord) / double(countBufferRecord);
	}

	//����buffer״̬��¼���Ա����buffer status
	void recordBufferStatus();

//	vector<CData> sendAllData(_SEND mode) override;

//	vector<CData> sendData(int n);

//	bool receiveData(int time, vector<CData> datas) override;

	/*************************************  Epidemic  *************************************/

	//FIXME: ��������Դ����ӳ٣�����������send��receive������������봫���ӳٵļ���

//	vector<int> sendSummaryVector();

//	vector<int> receiveSummaryVector(vector<int> sv);

//	//����Է��ڵ��SV�����㲢���������������ID�б�
//	vector<int> sendRequestList(vector<int> sv);

//	vector<int> receiveRequestList(vector<int> req);


	/**************************************  Prophet  *************************************/

//	map<int, double> sendDeliveryPreds();
//
//	map<int, double> receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv);

};

#endif // __NODE_H__
