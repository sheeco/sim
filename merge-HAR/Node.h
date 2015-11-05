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

	static double energyConsumption;

public:
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
	inline void setID(int ID)
	{
		this->ID = ID;
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

