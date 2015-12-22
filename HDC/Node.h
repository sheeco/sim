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

class CNode :
	public CGeneralNode
{
//protected:
//	int ID;  //node���
//	double x;  //node���ڵ�x����
//	double y;  //node���ڵ�y����
//	int time;  //����node���꼰����״̬��ʱ���
//	bool flag;

private:

	double generationRate;
	vector<int> summaryVector;
	map<CNode *, int> spokenCache;
	double dutyCycle;
	int SLOT_LISTEN;  //��SLOT_TOTAL��DC����õ�
	int SLOT_SLEEP;  //��SLOT_TOTAL��DC����õ�
	int state;  //ȡֵ��Χ��[ - SLOT_TOTAL, + SLOT_LISTEN )֮�䣬ֵ���ڵ���0������Listen״̬
	int timeDeath;  //�ڵ�ʧЧʱ�䣬Ĭ��ֵΪ-1
	CHotspot *atHotspot;


	//����ͳ������ڵ��buffer״̬��Ϣ
	int bufferSizeSum;
	int bufferChangeCount;

	static int ID_COUNT;
	static double SUM_ENERGY_CONSUMPTION;

	static vector<CNode *> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���
	static vector<CNode *> deadNodes;  //�����ľ��Ľڵ�


	CNode(void){};

	CNode(double generationRate, int capacityBuffer)
	{
		bufferSizeSum = 0;
		bufferChangeCount = 0;
		dutyCycle = DEFAULT_DUTY_CYCLE;
		SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
		SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;
		state = 0;
		timeDeath = -1;
		atHotspot = NULL;

		this->generationRate = generationRate;
		this->bufferCapacity = capacityBuffer;
		if( ENERGY > 1 )
			this->energy = ENERGY;
	}

	~CNode(void){};


	//����������ֵʱ��ʼ�շ���true
	bool isAlive()
	{
		if( energy == 0 )
			return true;
		else if( energy - energyConsumption <= 0 )
			return false;
		else
			return true;
	}

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
				idata++;
		}
	}	
	
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
		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); idata++)
		{

			if( CData::useHOP() && ( ! idata->allowForward() ) )
				continue;
			else
				summaryVector.push_back( idata->getID() );
		}
	}


public:

	static double DEFAULT_DUTY_CYCLE;  //
	static double HOTSPOT_DUTY_CYCLE;  //
	static int SLOT_TOTAL;
	static int BUFFER_CAPACITY;
	static double ENERGY;

	inline double getGenerationRate()
	{
		return generationRate;
	}
	inline CHotspot* getAtHotspot()
	{
		return atHotspot;
	}
	inline void setAtHotspot(CHotspot* atHotspot)
	{
		this->atHotspot = atHotspot;
	}
	static double getSumEnergyConsumption()
	{
		return SUM_ENERGY_CONSUMPTION;
	}
	inline void generateID()
	{
		this->ID = ID_COUNT;		
		ID_COUNT++;
	}
	inline void die(int currentTime)
	{
		this->timeDeath = currentTime;
	}
	inline bool useHotspotDutyCycle()
	{
		return dutyCycle == HOTSPOT_DUTY_CYCLE;
	}
	inline double getAverageBufferSize()
	{
		if(bufferSizeSum == 0)
			return 0;
		else
			return (double)bufferSizeSum / (double)bufferChangeCount;
	}

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
				CNode::nodes.push_back(node);
				CNode::idNodes.push_back( node->getID() );
			}
		}
	}

	static vector<CNode *>& getNodes()
	{
		if( SLOT_TOTAL == 0 || DEFAULT_DUTY_CYCLE == 0 || HOTSPOT_DUTY_CYCLE == 0 )
		{
			cout << "Error @ CNode::getNodes() SLOT_TOTAL & DEFAULT_DUTY_CYCLE & HOTSOT_DUTY_CYCLE = 0" << endl;
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
				inode++;
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

	//���ȵ㴦��� dc
	void raiseDutyCycle()
	{
		//hotspot dc Ϊ�ϸ�ֵʱ�����ȵ㴦���͵��� dc ֵ
		if( DEFAULT_DUTY_CYCLE < HOTSPOT_DUTY_CYCLE )
		{
			if( this->dutyCycle == HOTSPOT_DUTY_CYCLE )
				return;
		
			if( state < 0 )
				state = 0;
			dutyCycle = HOTSPOT_DUTY_CYCLE;
			SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
			SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;
		}
		//
		else
		{
			if( this->dutyCycle == DEFAULT_DUTY_CYCLE )
				return;
		
			if( state < 0 )
				state = 0;
			dutyCycle = DEFAULT_DUTY_CYCLE;
			SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
			SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;
		}

		if( state < -SLOT_LISTEN )
			state = -SLOT_LISTEN;
	}
	
	//�ڷ��ȵ㴦���� dc
	void resetDutyCycle()
	{
		//hotspot dc Ϊ�ϸ�ֵʱ���ڷ��ȵ㴦���͵�Ĭ�� dc ֵ
		if( DEFAULT_DUTY_CYCLE < HOTSPOT_DUTY_CYCLE )
		{
			if( this->dutyCycle == DEFAULT_DUTY_CYCLE )
				return;
		
			dutyCycle = DEFAULT_DUTY_CYCLE;
			SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
			SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;
		}
		//hotspot dc Ϊ�ϵ�ֵʱ���ڷ��ȵ㴦���͵��� dc ֵ
		else
		{
			if( this->dutyCycle == HOTSPOT_DUTY_CYCLE )
				return;
		
			dutyCycle = HOTSPOT_DUTY_CYCLE;
			SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
			SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;
		}

		//��ɱ��μ���֮��������
		if( state > SLOT_LISTEN )
			state = SLOT_LISTEN;
	}

	//��������͹���״̬�������Ƿ���ڹ���״̬
	bool updateStatus(int currentTime);

	//ע�⣺���м���������Ӧ���ڵ��ô˺����ж�֮����У����ô˺���֮ǰ����ȷ����updateStatus
	bool isListening()
	{
		return state >= 0;
	}


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
		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
		SUM_ENERGY_CONSUMPTION += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
		return summaryVector;
	}

	vector<int> receiveSummaryVector(vector<int> sv)
	{
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return vector<int>();

		energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_CONTROL;
		SUM_ENERGY_CONSUMPTION += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
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
					id++;
				}
			}
			sv = vector<int>( sv.begin(), id );
		}

		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
		SUM_ENERGY_CONSUMPTION += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
		return sv;
	}
	
	vector<int> receiveRequestList(vector<int> req)
	{
		if( req.empty() )
			return vector<int>();
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return vector<int>();

		energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_CONTROL;
		SUM_ENERGY_CONSUMPTION += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
		return req;
	}

	vector<CData> sendData(vector<int> requestList)
	{
		if( requestList.empty() )
			return vector<CData>();

		vector<CData> result;
		result = getItemsByID(buffer, requestList);
		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_DATA * result.size();
		SUM_ENERGY_CONSUMPTION += CONSUMPTION_DATA_SEND * SIZE_DATA * result.size();

		return result;
	}

	bool receiveData(vector<CData> datas, int currentTime)
	{
		if( datas.empty() )
			return false;
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return false;

		energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_DATA * datas.size();
		SUM_ENERGY_CONSUMPTION += CONSUMPTION_DATA_SEND * SIZE_DATA * datas.size();
		for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); idata++)
			idata->arriveAnotherNode(currentTime);
		buffer.insert(buffer.begin(), datas.begin(), datas.end() );
		updateBufferStatus(currentTime);
		
		return true;
	}

	//���ҽ�������sinkʱ����
	vector<CData> deliverAllData()
	{
		vector<CData> data = buffer;
		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_DATA * buffer.size();
		SUM_ENERGY_CONSUMPTION += CONSUMPTION_DATA_SEND * SIZE_DATA * buffer.size();
		buffer.clear();
		return data;
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
	
	//static bool ifNodeExists(int id)
	//{
	//	for(vector<CNode>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	//	{
	//		if(inode->getID() == id)
	//			return true;
	//	}
	//	return false;
	//}
	////�ýڵ㲻����ʱ�޷���ֵ���Խ��������Ա�����ÿ�ε���֮ǰ���ú���ifNodeExists()���м��
	//static CNode getNodeByID(int id)
	//{
	//	for(vector<CNode>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	//	{
	//		if(inode->getID() == id)
	//			return *inode;
	//	}
	//}

};

