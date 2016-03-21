#pragma once

#include <map>
#include "GeneralNode.h"
#include "Hotspot.h"
#include "Package.h"


class CNode :
	public CGeneralNode
{
//protected:

//	int ID;  //node���
//	CCoordinate location;  //node���ڵ�y����
//	int time;  //����node���ꡢ����״̬����Prophet�е�˥������ʱ���
//	bool flag;


private:

	double generationRate;
	double dutyCycle;

	//  [ ----------SLEEP----------> | ----LISTEN----> ]

	int state;  //ȡֵ��Χ��[ - SLOT_SLEEP, + SLOT_LISTEN )֮�䣬ֵ���ڵ���0������Listen״̬
	int SLOT_SLEEP;  //��SLOT_TOTAL��DC����õ�
	int SLOT_LISTEN;  //��SLOT_TOTAL��DC����õ�
	int SLOT_DISCOVER;  //���� RTS ֮ǰ�ȴ��ھӽڵ㷢�ֵ�ʱ��

	int timeData;  //��һ���������ɵ�ʱ��
	int timeDeath;  //�ڵ�ʧЧʱ�䣬Ĭ��ֵΪ-1
	bool recyclable;  //�ڽڵ�����֮��ָʾ�ڵ��Ƿ��Կɱ����գ�Ĭ��Ϊ true��ֱ�� trace ��Ϣ��ֹ�� false����δʹ�ã�����г����Ϊ��Ӧ�ö�ȡ�˲�����
	CHotspot *atHotspot;

	//����ͳ������ڵ��buffer״̬��Ϣ
	int bufferSizeSum;
	int bufferChangeCount;
	static int encounterAtHotspot;
	static int encounterActive;  //��Ч����
	static int encounter;
	static int visiterAtHotspot;
	static int visiterOnRoute;
	static int transmitSuccessful;  //�ɹ������ݴ���
	static int transmit;

	static int ID_COUNT;
	static vector<CNode *> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���
	static vector<CNode *> deadNodes;  //�����ľ��Ľڵ�
	static vector<CNode *> deletedNodes;  //�����ݴ�Node������̬�仯ʱ����ʱ�Ƴ��Ľڵ�


	void init();

	CNode();

	CNode(double generationRate);

	~CNode();

	static void initNodes();

	void generateData(int currentTime);


	/****************************************  MAC  ***************************************/

	//������
	static void newNodes(int n);
	//������
	static void removeNodes(int n);

//	CPackage sendCTSWithIndex(CNode* dst, int currentTime);
//	CPackage sendDataWithIndex(CNode* dst, vector<CData> datas, int currentTime);
//	vector<CData> bufferData(int time, vector<CData> datas) override;



	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;

//	vector<CData> getDataBySV(vector<int> sv);
//	vector<CData> getDataBySV(vector<int> sv, int max);


	/**************************************  Prophet  *************************************/


	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���

	void initDeliveryPreds();
	void decayDeliveryPreds(int currentTime);


public:

	static int NUM_NODE_MIN;
	static int NUM_NODE_MAX;
	static int NUM_NODE_INIT;

	static int SLOT_TOTAL;
	static double DEFAULT_DUTY_CYCLE;  //��ʹ��HDC������HDC�в����ȵ�������ʱ��ռ�ձ�
	static double HOTSPOT_DUTY_CYCLE;  //HDC���ȵ������ڵ�ռ�ձ�
	static int DEFAULT_SLOT_DISCOVER;

	static double DEFAULT_DATA_RATE;  //( package / s )
	static int DATA_SIZE;  //( Byte )

	static int BUFFER_CAPACITY;
	static int ENERGY;
	static int SPOKEN_MEMORY;  //�����ʱ���ڽ��������ݵĽڵ���ʱ���ٽ�������
	static _RECEIVE RECEIVE_MODE;
	static _SEND SEND_MODE;
	static _QUEUE QUEUE_MODE;

	/****************************************  MAC  ***************************************/

	//���޶���Χ�������ɾһ��������node
	static int ChangeNodeNumber();

	// TODO: trigger operations here ?
	// *TODO: move even between 2 locations ?
	// TODO: how to trigger RTS ?
	//��������͹���״̬�������Ƿ�ȫ���ڵ㶼�Դ��
	bool updateStatus(int currentTime);

//	void receiveRTS(CPackage package);

//	void receivePackage(CPackage* package, int currentTime) override;

	bool hasSpokenRecently(CNode* node, int currentTime);

	void addToSpokenCache(CNode* node, int t);

	//�ɸ�����ID�б���ȡ��Ӧ����
	vector<CData> getDataByRequestList(vector<int> requestList) const;

	//����SEND_MODE == _dumpʱ��ɾ�� ACK ������
	void checkDataByAck(vector<CData> ack);

	// TODO: skip sending RTS if node has received any RTS ? yes if *trans delay countable
	//������յ��������ڵ�� RTS (��δʹ��)
	bool skipRTS()
	{
		return false;
	}


	/*************************** DC��� ***************************/

	//�ж��Ƿ����ڼ���
	//ע�⣺���м���������Ӧ���ڵ��ô˺����ж�֮����У����ô˺���֮ǰ����ȷ����updateStatus
	bool isListening() const override;

	//�ж��Ƿ��������ھӽڵ㷢��ʱ��
	//ע�⣺���ô˺���֮ǰ����ȷ����updateStatus
	bool isDiscovering() const
	{
		return state == SLOT_DISCOVER;
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

	//�������ڵ������Ƴ�
	static void ClearDeadNodes();


	/*************************** ------- ***************************/


	/**************************************  Epidemic  *************************************/

	/*************************** ���й��� ***************************/

	bool isFull()
	{
		return buffer.size() == bufferCapacity;
	}

	//�ֶ�������ѹ�� buffer�������������κβ���
	//ע�⣺�����ڵ��ô˺���֮���ֶ����� updateBufferStatus() ������
	void pushIntoBuffer(vector<CData> datas)
	{
		AddToListUniquely( this->buffer, datas );
	}

	//��ɾ�����ڵ���Ϣ������ʹ��TTLʱ���������Ƴ������ݷ���
	vector<CData> dropOverdueData(int currentTime);

	//������(1)�������ڵ�-���ڵ㡱(2)����-�¡���˳���buffer�е����ݽ�������
	//����MAX_QUEUE_SIZEʱ��ǰ�˶������ݣ����ʱ����ǰ�˶������ݲ�����
	//ע�⣺������dropOverdueData֮�����
	vector<CData> dropDataIfOverflow();

	//ע�⣺��Ҫ��ÿ���յ�����֮��Ͷ������֮������������֮����ô˺���
	vector<CData> updateBufferStatus(int currentTime);

	/*************************** ------- ***************************/

	//��ȡ���� SV��HOP == 0 ����Ϣ�������SV��
	//ע�⣺Ӧȷ������֮ǰbuffer״̬Ϊ����
	vector<int> updateSummaryVector();


	/**************************************  Prophet  *************************************/

	static double INIT_DELIVERY_PRED;
	static double DECAY_RATIO;
	static double TRANS_RATIO;

	map<int, double> getDeliveryPreds() const
	{
		return deliveryPreds;
	}
	void updateDeliveryPredsWith(int node, map<int, double> preds);
	void updateDeliveryPredsWithSink();


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
	static void encountAtHotspot() 
	{
		++encounterAtHotspot;
	}
	static void encount() 
	{
		++encounter;
	}
	static void encountActive() 
	{
		++encounterActive;
	}

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

	//���ʼ���������ͳ�ƽڵ�λ���ȵ��ڵİٷֱȣ�HAR·������δ��ӵ��ã�
	static void visitAtHotspot() 
	{
		++visiterAtHotspot;
	}
	static void visitOnRoute() 
	{
		++visiterOnRoute;
	}

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

	//���ݴ������������ͳ�����ݴ���ɹ��İٷֱ�
	static void transmitTry()
	{
		++transmit;
	}
	static void transmitSucceed()
	{
		++transmitSuccessful;
	}

	static int getTransmit()
	{
		return transmit;
	}
	static int getTransmitSuccessful()
	{
		return transmitSuccessful;
	}
	static double getTransmitSuccessfulPercent()
	{
		if(transmitSuccessful == 0)
			return 0.0;
		return double(transmitSuccessful) / double(transmit);
	}

	/*************************** ------- ***************************/

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
		++ID_COUNT;
		this->ID = ID_COUNT;		
	}

	double getAverageBufferSize() const 
	{
		if(bufferSizeSum == 0)
			return 0;
		else
			return double(bufferSizeSum) / double(bufferChangeCount);
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

