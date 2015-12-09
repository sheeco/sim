#pragma once

#include "BasicEntity.h"
#include "GeneralNode.h"
#include "Data.h"

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
	int queueSize;  //buffer�д洢�ķǱ��ڵ������Data�ļ�������ÿ��updateStatus֮�����
	vector<int> summaryVector;
	vector<CNode *> recentNodes;


	//����ͳ������ڵ��buffer״̬��Ϣ
	int bufferSizeSum;
	int bufferChangeCount;

	static int BUFFER_CAPACITY;
	static int MAX_QUEUE_SIZE;  //ͬ��洢�����������ڵ��data�������������������Ŀ���������Ƿ���Request֮ǰ��飿��
	static int ID_COUNT;
	static double energyConsumption;

	//ע�⣺��Ҫ��ÿ�ν�������֮���ֶ����ô˺���
	//��ɾ�����ڵ���Ϣ������ʹ��TTLʱ����������SV��HOP<=1����Ϣ�������SV��
	void updateStatus(int time)
	{
		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); idata++)
		{

		}
	}

public:

	static vector<CNode> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���

	CNode(void)
	{
		generationRate = 0;
	}

	CNode(double generationRate, int capacityBuffer)
	{
		this->generationRate = generationRate;
		bufferSizeSum = 0;
		bufferChangeCount = 0;
	}

	~CNode(void);

	inline double getGenerationRate()
	{
		return generationRate;
	}
	static inline double getEnergyConsumption()
	{
		return energyConsumption;
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

	inline vector<int> getSummaryVector()
	{
		return summaryVector;
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
	vector<CData> sendData(vector<int> requestList)
	{
		vector<CData> result;
		result = getItemsByID(buffer, requestList);
		energyConsumption += CONSUMPTION_DATA_SEND * result.size();
	}
	void receiveData(vector<CData> datas, int time)
	{
		energyConsumption += CONSUMPTION_DATA_RECIEVE * datas.size();
		for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); idata++)
			idata->arriveAnotherNode(time);
		buffer.insert(buffer.begin(), datas.begin(), datas.end() );
	}

	static bool ifNodeExists(int id)
	{
		for(vector<CNode>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
		{
			if(inode->getID() == id)
				return true;
		}
		return false;
	}
	//�ýڵ㲻����ʱ�޷���ֵ���Խ��������Ա�����ÿ�ε���֮ǰ���ú���ifNodeExists()���м��
	static CNode getNodeByID(int id)
	{
		for(vector<CNode>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
		{
			if(inode->getID() == id)
				return *inode;
		}
	}

	void generateData(int time);
	vector<CData> sendData(int num);
	vector<CData> sendAllData();
	void failSendData();
	//����buffer״̬��¼���Ա����buffer status
	void updateBufferStatus()
	{
		bufferSizeSum += buffer.size();
		bufferChangeCount++;
	}

};

