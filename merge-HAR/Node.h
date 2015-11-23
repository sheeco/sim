#pragma once

#include "Base.h"
#include "Data.h"

class CNode :
	public CBase
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
	int capacityBuffer;

	int bufferSizeSum;
	int bufferChangeCount;

	static int ID_COUNT;
	static double energyConsumption;

public:

	static vector<CNode> nodes;  //���ڴ������д������ڵ㣬��ԭ����HAR::CNode::nodes�ƶ�������
	static vector<int> idNodes;  //���ڴ������д������ڵ��ID�����ڴ���

	CNode(void)
	{
		generationRate = 0;
		capacityBuffer = BUFFER_CAPACITY_NODE;
	}

	CNode(double generationRate, int capacityBuffer)
	{
		this->generationRate = generationRate;
		this->capacityBuffer = capacityBuffer;
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
		ID_COUNT++;
		this->ID = ID_COUNT;
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

