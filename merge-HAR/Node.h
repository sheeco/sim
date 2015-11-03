#pragma once

#include "Base.h"
#include "Data.h"

class CNode :
	public CBase
{
//protected:
//	int ID;  //node编号
//	double x;  //node现在的x坐标
//	double y;  //node现在的y坐标
//	int time;  //更新node坐标的时间戳
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
	//更新buffer状态记录，以便计算buffer status
	void updateBufferStatus()
	{
		bufferSizeSum += buffer.size();
		bufferChangeCount++;
	}

};

