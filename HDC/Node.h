#pragma once

#include "BasicEntity.h"
#include "GeneralNode.h"
#include "Data.h"
#include <map>

using namespace std;

class CNode :
	public CGeneralNode
{
//protected:
//	int ID;  //node���
//	double x;  //node���ڵ�x����
//	double y;  //node���ڵ�y����
//	int time;  //����node�����ʱ���
//	bool flag;

private:
	double generationRate;
	vector<CData> buffer;
	double energyConsumption;
	int queueSize;  //buffer�д洢�ķǱ��ڵ������Data�ļ�������ÿ��updateStatus֮�����
	vector<int> summaryVector;
	map<CNode *, int> spokenCache;
	double dutyCycle;
	int state;


	//����ͳ������ڵ��buffer״̬��Ϣ
	int bufferSizeSum;
	int bufferChangeCount;

	static int STATE_WORK;
	static int STATE_REST;
	static int BUFFER_CAPACITY;
	static int MAX_QUEUE_SIZE;  //ͬ��洢�����������ڵ��data�������������������Ŀ���������Ƿ���Request֮ǰ��飿��Ĭ��ֵ����buffer����
	static int SPOKEN_MEMORY;  //�����ʱ���ڽ��������ݵĽڵ���ʱ���ٽ�������

	static int ID_COUNT;
	static double SUM_ENERGY_CONSUMPTION;

	//��ɾ�����ڵ���Ϣ������ʹ��TTLʱ��
	void dropOverdueData(int currentTime)
	{
		if( CData::useHOP() )
			return;

		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
		{
			idata->updateStatus(currentTime);
			//���TTL���ڣ�����
			if( idata->isOverdue() )
				idata = buffer.erase( idata );
		}
	}	
	
	//������(1)�������ڵ�-���ڵ㡱(2)����-�¡���˳���buffer�е����ݽ�������
	//����MAX_QUEUE_SIZEʱ��ǰ�˶������ݣ����ʱ���Ӵ�ǰ�˶�������
	//ע�⣺������dropOverdueData֮�����
	void dropDataIfOverflow(int currentTime);

	//ע�⣺��Ҫ��ÿ���յ�����֮������������֮����ô˺���
	void updateBufferStatus(int currentTime)
	{
		dropOverdueData(currentTime);
		dropDataIfOverflow(currentTime);
	}

	//����SV��HOP <= 1����Ϣ�������SV��
	//ע�⣺Ӧȷ������֮ǰbuffer״̬Ϊ���£�����Ҫ��ÿ���������ڵ㷢��SV֮ǰ���ô˺���
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

	//ע�⣺��Ҫ��ÿ���������ڵ㷢��SV֮ǰ����sinkͶ������֮ǰ���յ�����֮������������֮����ô˺���
	//void updateStatus(int currentTime)
	//{
	//	updateBufferStatus(currentTime);
	//	updateSummaryVector();
	//}

public:

	static vector<CNode> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���

	CNode(void)
	{
		generationRate = 0;
		energyConsumption = 0;
	}

	CNode(double generationRate, int capacityBuffer)
	{
		this->generationRate = generationRate;
		energyConsumption = 0;
		bufferSizeSum = 0;
		bufferChangeCount = 0;
	}

	~CNode(void);

	inline double getGenerationRate()
	{
		return generationRate;
	}
	inline double getEnergyConsumption()
	{
		return energyConsumption;
	}
	static double getSumEnergyConsumption()
	{
		return SUM_ENERGY_CONSUMPTION;
	}
	inline void setSinkID()
	{
		this->ID = SINK_ID;
	}
	inline void generateID()
	{
		this->ID = ID_COUNT;		
		ID_COUNT++;
	}
	inline void moveTo(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	inline bool hasData()
	{
		return ( ! buffer.empty() );
	}
	inline int getBufferSize()
	{
		return buffer.size();
	}
	inline double getAverageBufferSize()
	{
		if(bufferSizeSum == 0)
			return 0;
		else
			return (double)bufferSizeSum / (double)bufferChangeCount;
	}

	bool hasSpokenRecently(CNode* node, int currentTime)
	{
		map<CNode *, int>::iterator icache = spokenCache.find( node );
		if( icache == spokenCache.end() )
			return false;
		else if( ( currentTime - icache->second ) < SPOKEN_MEMORY )
			return true;
		else
			return false;
	}

	void addToSpokenCache(CNode* node, int t)
	{
		spokenCache.insert( pair<CNode*, int>(node, t) );
	}

	vector<int> sendSummaryVector()
	{
		updateSummaryVector();
		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
		return summaryVector;
	}

	vector<int> receiveSummaryVector(vector<int> sv)
	{
		energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_CONTROL;
		return sv;	
	}

	//����Է��ڵ��SV�����㲢���������������ID�б�
	vector<int> calculateRequestList(vector<int> sv)
	{
		RemoveFromList(sv, summaryVector);
		//������
		if( MAX_QUEUE_SIZE > 0  &&  sv.size() > MAX_QUEUE_SIZE )
		{
			vector<int>::iterator id = sv.begin();
			for(int count = 0; id != sv.end() &&  count < MAX_QUEUE_SIZE ; )
			{
				if( CData::getNodeByMask( *id ) != this->ID )
				{
					count++;
					id++;
				}
			}
			sv = vector<int>( sv.begin(), id );
		}
		return sv;
	}

	vector<int> sendRequestList(vector<int> req)
	{
		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_CONTROL;
		return req;
	}
	
	vector<int> receiveRequestList(vector<int> req)
	{
		energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_CONTROL;
		return req;
	}

	vector<CData> sendData(vector<int> requestList)
	{
		vector<CData> result;
		result = getItemsByID(buffer, requestList);
		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_DATA * result.size();
	}
	void receiveData(vector<CData> datas, int time)
	{
		energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_DATA * datas.size();
		for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); idata++)
			idata->arriveAnotherNode(time);
		buffer.insert(buffer.begin(), datas.begin(), datas.end() );
		updateBufferStatus(time);
	}

	//���ҽ�������sinkʱ����
	vector<CData> sendAllData()
	{
		vector<CData> data = buffer;
		energyConsumption += CONSUMPTION_DATA_SEND * SIZE_DATA * buffer.size();
		buffer.clear();
		return data;
	}

	void generateData(int time)
	{
		int nData = generationRate * SLOT_DATA_GENERATE;
		for(int i = 0; i < nData; i++)
		{
			CData data(ID, time);
			buffer.push_back(data);
		}
		updateBufferStatus(time);
	}

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

	//vector<CData> sendData(int num);
	
	//void failSendData();

	//����buffer״̬��¼���Ա����buffer status


};

