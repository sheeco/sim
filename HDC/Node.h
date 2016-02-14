#pragma once

#include "BasicEntity.h"
#include "GeneralNode.h"
#include "Data.h"
#include <map>
#include "Epidemic.h"
#include "Hotspot.h"

using namespace std;

extern int NUM_NODE;
extern double PROB_DATA_FORWARD;
extern MacProtocol MAC_PROTOCOL;
extern RoutingProtocol ROUTING_PROTOCOL;

class CNode :
	public CGeneralNode
{
//protected:
//	int ID;  //node���
//	double x;  //node���ڵ�x����
//	double y;  //node���ڵ�y����
//	int time;  //����node���ꡢ����״̬����Prophet�е�˥������ʱ���
//	bool flag;

private:

	double generationRate;
	double dutyCycle;

	int SLOT_LISTEN;  //��SLOT_TOTAL��DC����õ�
	int SLOT_SLEEP;  //��SLOT_TOTAL��DC����õ�
	int state;  //ȡֵ��Χ��[ - SLOT_TOTAL, + SLOT_LISTEN )֮�䣬ֵ���ڵ���0������Listen״̬
	int timeDeath;  //�ڵ�ʧЧʱ�䣬Ĭ��ֵΪ-1
	CHotspot *atHotspot;

	//����ͳ������ڵ��buffer״̬��Ϣ
	int bufferSizeSum;
	int bufferChangeCount;
	static int encounter;
	static int encounterAtHotspot;
	static int encounterOnRoute;

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

	void init()
	{
		generationRate = 0;
		atHotspot = nullptr;
		dutyCycle = DEFAULT_DUTY_CYCLE;
		SLOT_LISTEN = 0;
		SLOT_SLEEP = 0;
		state = 0;
		timeDeath = 0;
		bufferSizeSum = 0;
		bufferChangeCount = 0;
	}

	CNode(void)
	{
		init();
	}

	CNode(double generationRate, int bufferCapacity)
	{
		init();
		this->generationRate = generationRate;
		this->bufferCapacity = bufferCapacity;
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

	/*************************************  Epidemic  *************************************/

	//��ɾ�����ڵ���Ϣ������ʹ��TTLʱ��
	void dropOverdueData(int currentTime)
	{
		if( buffer.empty() )
			return;
		if( CData::useHOP() )
			return;

		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
		{
			idata->updateStatus(currentTime);
			//���TTL���ڣ�����
			if( idata->isOverdue() )
				idata = buffer.erase( idata );
			else
				++idata;
		}
	}	
	
	//���й���
	//������(1)�������ڵ�-���ڵ㡱(2)����-�¡���˳���buffer�е����ݽ�������
	//����MAX_QUEUE_SIZEʱ��ǰ�˶������ݣ����ʱ���Ӵ�ǰ�˶�������
	//ע�⣺������dropOverdueData֮�����
	void dropDataIfOverflow(int currentTime);

	//ע�⣺��Ҫ��ÿ���յ�����֮��Ͷ������֮������������֮����ô˺���
	void updateBufferStatus(int currentTime)
	{
		dropOverdueData(currentTime);
		dropDataIfOverflow(currentTime);
	}

	//����SV��HOP <= 1����Ϣ�������SV��
	//ע�⣺Ӧȷ������֮ǰbuffer״̬Ϊ���£�����Ҫ��ÿ�η���SV���յ�SV������requestList֮ǰ���ô˺���
	void updateSummaryVector()
	{
		if( buffer.size() > BUFFER_CAPACITY )
		{
			cout << "Error @ CNode::calculateSummaryVector() : Buffer isn't clean !" << endl;
			_PAUSE;
		}

		summaryVector.clear();
		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); ++idata)
		{

			if( CData::useHOP() && ( ! idata->allowForward() ) )
				continue;
			else
				summaryVector.push_back( idata->getID() );
		}
	}

	/**************************************  Prophet  *************************************/

	void initDeliveryPreds()
	{
		if( ! deliveryPreds.empty() )
			return;

		for(int id = 0; id <= NUM_NODE; id++)
		{
			if( id != ID )
				deliveryPreds[id] = INIT_DELIVERY_PRED;
		}
	}

	void decayDeliveryPreds(int currentTime)
	{
		for(map<int, double>::iterator imap = deliveryPreds.begin(); imap != deliveryPreds.end(); ++imap)
			deliveryPreds[ imap->first ] = imap->second * pow( DECAY_RATIO, ( currentTime - time ) / SLOT_MOBILITYMODEL );
	}


public:

	static double DEFAULT_DUTY_CYCLE;  //��ʹ��HDC������HDC�в����ȵ�������ʱ��ռ�ձ�
	static double HOTSPOT_DUTY_CYCLE;  //HDC���ȵ������ڵ�ռ�ձ�
	static int SLOT_TOTAL;
	static int BUFFER_CAPACITY;
	static double ENERGY;
	static Mode BUFFER_MODE;
	static Mode COPY_MODE;
	static Mode SEND_MODE;

	/**************************************  Prophet  *************************************/

	static double INIT_DELIVERY_PRED;
	static double DECAY_RATIO;
	static double TRANS_RATIO;



	static void initNodes()
	{
		if( nodes.empty() && deadNodes.empty() )
		{
			for(int i = 0; i < NUM_NODE; i++)
			{
				double generationRate = RATE_DATA_GENERATE;
				if(i % 5 == 0)
					generationRate *= 5;
				CNode* node = new CNode(generationRate, BUFFER_CAPACITY_NODE);
				node->generateID();
				node->initDeliveryPreds();
				CNode::nodes.push_back(node);
				CNode::idNodes.push_back( node->getID() );
			}
		}
	}

	~CNode(void){};

	static vector<CNode *>& getNodes()
	{
		if( SLOT_TOTAL == 0 || ( ZERO( DEFAULT_DUTY_CYCLE ) && ZERO( HOTSPOT_DUTY_CYCLE ) ) )
		{
			cout << "Error @ CNode::getNodes() : SLOT_TOTAL || ( DEFAULT_DUTY_CYCLE && HOTSPOT_DUTY_CYCLE ) = 0" << endl;
			_PAUSE;
		}

		if( nodes.empty() && deadNodes.empty() )
			initNodes();
		return nodes;
	}

	static vector<int>& getIdNodes()
	{
		if( nodes.empty() && deadNodes.empty() )
			initNodes();
		return idNodes;
	}

	static bool hasNodes(int currentTime)
	{
		if( nodes.empty() && deadNodes.empty() )
			initNodes();
		else
			idNodes.clear();
		
		for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); )
		{
			if( (*inode)->isAlive() )
			{
				idNodes.push_back( (*inode)->getID() );
				++inode;
			}
			else
			{
				(*inode)->die( currentTime );
				deadNodes.push_back( *inode );
				inode = nodes.erase( inode );
			}
		}

		return ( ! nodes.empty() );
	}

	static void newNodes(int n)
	{
		//���Ȼָ�֮ǰ��ɾ���Ľڵ�
		for(int i = NUM_NODE; i < NUM_NODE + n; i++)
		{
			if( deletedNodes.empty() )
				break;

			CNode::nodes.push_back(deletedNodes[0]);
			CNode::idNodes.push_back( deletedNodes[0]->getID() );
			++NUM_NODE;
			--n;
		}
		//����Բ������������µĽڵ�
		for(int i = NUM_NODE; i < NUM_NODE + n; i++)
		{
			double generationRate = RATE_DATA_GENERATE;
			if(i % 5 == 0)
				generationRate *= 5;
			CNode* node = new CNode(generationRate, BUFFER_CAPACITY_NODE);
			node->generateID();
			node->initDeliveryPreds();
			CNode::nodes.push_back(node);
			CNode::idNodes.push_back( node->getID() );
			++NUM_NODE;
			--n;
		}			
	}

	static void removeNodes(int n)
	{
		//FIXME: Random selected ?
		vector<CNode *>::iterator start = nodes.begin();
		vector<CNode *>::iterator end = nodes.end();
		vector<CNode *>::iterator fence = nodes.begin();
		fence += NUM_NODE + n;
		vector<CNode *> leftNodes(start, fence);

		//Remove invalid positoins belonging to the deleted nodes
		vector<CNode *> deletedNodes(fence, end);
		vector<int> deletedIDs;
		for(auto inode = deletedNodes.begin(); inode != deletedNodes.end(); ++inode)
			deletedIDs.push_back( (*inode)->getID() );

		for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
		{
			if( ifExists(deletedIDs, (*ipos)->getNode()) )
				ipos = CPosition::positions.erase(ipos);
			else
				++ipos;
		}

		nodes = leftNodes;
		NUM_NODE = nodes.size();
		idNodes.clear();
		for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
			idNodes.push_back( (*inode)->getID() );
		CNode::deletedNodes.insert(CNode::deletedNodes.end(), deletedNodes.begin(), deletedNodes.end());
	}

	static bool ifNodeExists(int id)
	{
		for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
		{
			if((*inode)->getID() == id)
				return true;
		}
		return false;
	}

	//�ýڵ㲻����ʱ�޷���ֵ���Խ��������Ա�����ÿ�ε���֮ǰ���ú���ifNodeExists()���м��
	static CNode* getNodeByID(int id)
	{
		for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
		{
			if((*inode)->getID() == id)
				return *inode;
		}
		return nullptr;
	}

	inline double getGenerationRate() const
	{
		return generationRate;
	}
	inline CHotspot* getAtHotspot() const
	{
		return atHotspot;
	}
	inline void setAtHotspot(CHotspot* atHotspot)
	{
		this->atHotspot = atHotspot;
	}
	inline bool isAtHotspot() const
	{
		if( MAC_PROTOCOL == _hdc )
			return dutyCycle == HOTSPOT_DUTY_CYCLE;
		else if( ROUTING_PROTOCOL == _har )
			return atHotspot != nullptr;
		else
			return false;
	}
	static double getSumEnergyConsumption()
	{
		double sumEnergyConsumption = 0;
		for(auto inode = getNodes().begin(); inode != getNodes().end(); ++inode)
			sumEnergyConsumption += (*inode)->getEnergyConsumption();
		return sumEnergyConsumption;
	}
	inline void generateID()
	{
		ID_COUNT++;
		this->ID = ID_COUNT;		
	}
	inline void die(int currentTime)
	{
		this->timeDeath = currentTime;
	}
	inline bool useHotspotDutyCycle() const
	{
		return ZERO( dutyCycle - HOTSPOT_DUTY_CYCLE );
	}
	inline double getAverageBufferSize() const
	{
		if(bufferSizeSum == 0)
			return 0;
		else
			return static_cast<double>(bufferSizeSum) / static_cast<double>(bufferChangeCount);
	}


	//���ȵ㴦��� dc
	void raiseDutyCycle()
	{
		if( ZERO( this->dutyCycle - HOTSPOT_DUTY_CYCLE ) )
			return;

		if( state < 0 )
			state = 0;
		dutyCycle = HOTSPOT_DUTY_CYCLE;
		SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
		SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;

		if( state < -SLOT_LISTEN )
			state = -SLOT_LISTEN;
	}
	
	//�ڷ��ȵ㴦���� dc
	void resetDutyCycle()
	{
		if( ZERO( this->dutyCycle - DEFAULT_DUTY_CYCLE ) )
			return;

		dutyCycle = DEFAULT_DUTY_CYCLE;
		SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
		SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;

		//��ɱ��μ���֮��������
		if( state > SLOT_LISTEN )
			state = SLOT_LISTEN;
	}

	//��������͹���״̬�������Ƿ���ڹ���״̬
	bool updateStatus(int currentTime);

	//ע�⣺���м���������Ӧ���ڵ��ô˺����ж�֮����У����ô˺���֮ǰ����ȷ����updateStatus
	bool isListening() const
	{
		if( ZERO(dutyCycle) )
			return false;

		return state >= 0;
	}

	void generateData(int currentTime)
	{
		int nData = generationRate * SLOT_DATA_GENERATE;
		for(int i = 0; i < nData; i++)
		{
			CData data(ID, currentTime);
			buffer.push_back(data);
		}
		updateBufferStatus(currentTime);
	}

	//����buffer״̬��¼���Ա����buffer status
	void recordBufferStatus()
	{
		bufferSizeSum += buffer.size();
		bufferChangeCount++;
	}

	//��������
	inline static double getEncounterPercentAtHotspot()
	{
		return static_cast<double>(encounterAtHotspot) / static_cast<double>(encounter);
	}
	inline static int getEncounter()
	{
		return encounter;
	}
	inline static int getEncounterAtHotspot()
	{
		return encounterAtHotspot;
	}

	//���ڼ�¼MA�ڵ���sensor����������
	inline static void encountAtHotspot()
	{
		encounterAtHotspot++;
		encounter++;
	}
	inline static void encountOnRoute()
	{
		encounterOnRoute++;
		encounter++;
	}

	vector<CData> sendAllData(Mode mode) override
	{
		return CGeneralNode::sendAllData(mode);
	}

	vector<CData> sendData(int n)
	{
		if( n >= buffer.size() )
			return sendAllData(SEND::DUMP);

		double bet = RandomFloat(0, 1);
		if(bet > PROB_DATA_FORWARD)
		{
			energyConsumption += n * BYTE_PER_DATA * CONSUMPTION_BYTE_SEND;
			return vector<CData>();
		}

		vector<CData> data;
		if( SEND_MODE == SEND::FIFO )
		{
			data.insert(data.begin(), buffer.begin(), buffer.begin() + n);
		}
		else if( SEND_MODE == SEND::LIFO )
		{
			data.insert(data.begin(), buffer.end() - n, buffer.end());
		}
		return data;
	}

	bool receiveData(int time, vector<CData> datas) override
	{
		if( datas.empty() )
			return false;
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return false;

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * BYTE_PER_DATA * datas.size();
		for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
			idata->arriveAnotherNode(time);
		buffer.insert(buffer.begin(), datas.begin(), datas.end() );
		updateBufferStatus(time);
		
		return true;
	}
	

	/*************************************  Epidemic  *************************************/

	bool hasSpokenRecently(CNode* node, int currentTime)
	{
		map<CNode *, int>::iterator icache = spokenCache.find( node );
		if( icache == spokenCache.end() )
			return false;
		else if( ( currentTime - icache->second ) < Epidemic::SPOKEN_MEMORY )
			return true;
		else
			return false;
	}

	void addToSpokenCache(CNode* node, int t)
	{
		spokenCache.insert( pair<CNode*, int>(node, t) );
	}

	//FIXME: ��������Դ����ӳ٣�����������send��receive������������봫���ӳٵļ���

	vector<int> sendSummaryVector()
	{
		updateSummaryVector();
		energyConsumption += CONSUMPTION_BYTE_SEND * BYTE_PER_CTRL;
		return summaryVector;
	}

	vector<int> receiveSummaryVector(vector<int> sv)
	{
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return vector<int>();

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * BYTE_PER_CTRL;
		return sv;	
	}

	//����Է��ڵ��SV�����㲢���������������ID�б�
	vector<int> sendRequestList(vector<int> sv)
	{
		if( sv.empty() )
			 return vector<int>();

		updateSummaryVector();
		RemoveFromList(sv, summaryVector);
		//������
		if( Epidemic::MAX_QUEUE_SIZE > 0  &&  sv.size() > Epidemic::MAX_QUEUE_SIZE )
		{
			vector<int>::iterator id = sv.begin();
			for(int count = 0; id != sv.end() &&  count < Epidemic::MAX_QUEUE_SIZE ; )
			{
				if( CData::getNodeByMask( *id ) != this->ID )
				{
					count++;
					++id;
				}
			}
			sv = vector<int>( sv.begin(), id );
		}

		energyConsumption += CONSUMPTION_BYTE_SEND * BYTE_PER_CTRL;
		return sv;
	}
	
	vector<int> receiveRequestList(vector<int> req)
	{
		if( req.empty() )
			return vector<int>();
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return vector<int>();

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * BYTE_PER_CTRL;
		return req;
	}

	vector<CData> sendDataByRequestList(vector<int> requestList)
	{
		if( requestList.empty() )
			return vector<CData>();

		vector<CData> result;
		result = getItemsByID(buffer, requestList);
		energyConsumption += CONSUMPTION_BYTE_SEND * BYTE_PER_DATA * result.size();
		
		return result;
	}


	/**************************************  Prophet  *************************************/
		
	void updateDeliveryPredsWith(int node, map<int, double> preds)
	{
		double oldPred = 0, transPred = 0, dstPred = 0;
		if( deliveryPreds.find(node) == deliveryPreds.end() )
			deliveryPreds[ node ] = INIT_DELIVERY_PRED;

		oldPred = this->deliveryPreds[ node ];
		deliveryPreds[ node ] = transPred = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;

		for(map<int, double>::iterator imap = preds.begin(); imap != preds.end(); ++imap)
		{
			int dst = imap->first;
			if( dst == this->ID )
				continue;
			if( deliveryPreds.find(node) == deliveryPreds.end() )
				deliveryPreds[ node ] = INIT_DELIVERY_PRED;

			oldPred = this->deliveryPreds[ dst ];
			dstPred = imap->second;
			deliveryPreds[ dst ] = oldPred + ( 1 - oldPred ) * transPred * dstPred * TRANS_RATIO;
		}
		
	}

	void updateDeliveryPredsWithSink()
	{
		double oldPred = deliveryPreds[SINK_ID];
		deliveryPreds[SINK_ID] = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;
	}

	map<int, double> sendDeliveryPreds(int currentTime)
	{
		energyConsumption += CONSUMPTION_BYTE_SEND * BYTE_PER_CTRL;
		
		return deliveryPreds;
	}

	map<int, double> receiveDeliveryPredsAndSV(int node, map<int, double> preds, vector<int> &sv)
	{
		if( preds.empty() )
			return map<int, double>();
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return map<int, double>();

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * BYTE_PER_CTRL;
		return preds;
	}

	vector<CData> sendDataByPredsAndSV(CNode *node, map<int, double> preds, vector<int> &sv)
	{
		if( preds.empty() )
			return vector<CData>();

		if( preds.find(SINK_ID)->second > this->deliveryPreds.find(SINK_ID)->second )
		{		
			vector<int> req = summaryVector;
			RemoveFromList(req, sv);
			return sendDataByRequestList( req );
		}
		else
			return vector<CData>();
	}

};

