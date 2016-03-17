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

	int SLOT_SLEEP;  //��SLOT_TOTAL��DC����õ�
	int SLOT_LISTEN;  //��SLOT_TOTAL��DC����õ�
	int SLOT_WAIT;  //���� RTS ֮ǰ�ȴ���ʱ��
	int state;  //ȡֵ��Χ��[ - SLOT_SLEEP, + SLOT_LISTEN )֮�䣬ֵ���ڵ���0������Listen״̬
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
	static int visiterOnRoute;;

	static int ID_COUNT;
	static vector<CNode *> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���
	static vector<CNode *> deadNodes;  //�����ľ��Ľڵ�
	static vector<CNode *> deletedNodes;  //�����ݴ�Node������̬�仯ʱ����ʱ�Ƴ��Ľڵ�

	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;

	/**************************************  Prophet  *************************************/

	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���

	void init();

	CNode();

	CNode(double generationRate);

	static void initNodes();

	void generateData(int currentTime);

	/*************************************  Epidemic  *************************************/

	//��ɾ�����ڵ���Ϣ������ʹ��TTLʱ��
	void dropOverdueData(int currentTime);

	//���й���
	//������(1)�������ڵ�-���ڵ㡱(2)����-�¡���˳���buffer�е����ݽ�������
	//����MAX_QUEUE_SIZEʱ��ǰ�˶������ݣ����ʱ����ǰ�˶�������
	//ע�⣺������dropOverdueData֮�����
	void dropDataIfOverflow();

	//ע�⣺��Ҫ��ÿ���յ�����֮��Ͷ������֮������������֮����ô˺���
	void updateBufferStatus(int currentTime);

	//����SV��HOP <= 1����Ϣ�������SV��
	//ע�⣺Ӧȷ������֮ǰbuffer״̬Ϊ���£�����Ҫ��ÿ�η���SV���յ�SV������requestList֮ǰ���ô˺���
	void updateSummaryVector();

	/**************************************  Prophet  *************************************/

	//������
	static void newNodes(int n);

	//������
	static void removeNodes(int n);

	void initDeliveryPreds();

	void decayDeliveryPreds(int currentTime);


public:

	static int NUM_NODE_MIN;
	static int NUM_NODE_MAX;
	static int NUM_NODE_INIT;

	static int SLOT_TOTAL;
	static double DEFAULT_DUTY_CYCLE;  //��ʹ��HDC������HDC�в����ȵ�������ʱ��ռ�ձ�
	static double HOTSPOT_DUTY_CYCLE;  //HDC���ȵ������ڵ�ռ�ձ�
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

	//��������͹���״̬�������Ƿ�ȫ���ڵ㶼�Դ��
	bool updateStatus(int currentTime);

	//ע�⣺���м���������Ӧ���ڵ��ô˺����ж�֮����У����ô˺���֮ǰ����ȷ����updateStatus
	bool isListening() const;

	//���ȵ㴦��� dc
	void raiseDutyCycle();

	//�ڷ��ȵ㴦���� dc
	void resetDutyCycle();

	//��� recycable Ϊ true��������ڶ�̬�ڵ��������ʱ���ָ�
	//ע�⣺�нڵ�����֮�󣬱������ClearDeadNodes()
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
	//�����յ� RTS �����нڵ�

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

	//�����Ѿ�ʧЧ�Ľڵ��ɾ���Ľڵ㣬����ID����
	static vector<CNode *> getAllNodes(bool sort);

	static vector<int>& getIdNodes();

	static bool finiteEnergy();

	static bool hasNodes(int currentTime);

	static void ClearDeadNodes();

	static bool ifNodeExists(int id);

	//�ýڵ㲻����ʱ�޷���ֵ���Խ��������Ա�����ÿ�ε���֮ǰ���ú���ifNodeExists()���м��
	static CNode* getNodeByID(int id);

	static double getSumEnergyConsumption();


	//����������HAR��ͳ�ƽڵ��MA������������ͳ�ƽڵ�����������
	//ע�⣺��ʱֻ֧��Prophet·�ɣ�����·����δ�����ش���
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

	//ע�⣺�˺��������ڵ��ù�encountAtHotspot()��encountOnRoute()֮���ٵ���
	static void encountActive() 
	{
		encounterActive++;
	}

	//���ʼ���������ͳ�ƽڵ�λ���ȵ��ڵİٷֱȣ�HAR·������δ��ӵ��ã�
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

	//����buffer״̬��¼���Ա����buffer status
	void recordBufferStatus();

	vector<CData> sendAllData(_SEND mode) override;

	vector<CData> sendData(int n);

	bool receiveData(int time, vector<CData> datas) override;


	/*************************************  Epidemic  *************************************/

	bool hasSpokenRecently(CNode* node, int currentTime);

	void addToSpokenCache(CNode* node, int t);

	//FIXME: ��������Դ����ӳ٣�����������send��receive������������봫���ӳٵļ���

	vector<int> sendSummaryVector();

	vector<int> receiveSummaryVector(vector<int> sv);

	//����Է��ڵ��SV�����㲢���������������ID�б�
	vector<int> sendRequestList(vector<int> sv);

	vector<int> receiveRequestList(vector<int> req);

	vector<CData> sendDataByRequestList(vector<int> requestList);


	/**************************************  Prophet  *************************************/

	void updateDeliveryPredsWith(int node, map<int, double> preds);

	void updateDeliveryPredsWithSink();

	map<int, double> sendDeliveryPreds();

	map<int, double> receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv);

	vector<CData> sendDataByPredsAndSV(map<int, double> preds, vector<int> &sv);

	//���޶���Χ�������ɾһ��������node
	static int ChangeNodeNumber();

};

