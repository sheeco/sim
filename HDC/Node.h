#pragma once

#include "BasicEntity.h"
#include "Data.h"
#include "GeneralNode.h"

using namespace std;

class CNode :
	public CGeneralNode
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

	static int ID_COUNT;
	static double energyConsumption;

public:

	static vector<CNode> nodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
	static vector<int> idNodes;  //用于储存所有传感器节点的ID，便于处理

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

	static bool ifNodeExists(int id)
	{
		for(vector<CNode>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
		{
			if(inode->getID() == id)
				return true;
		}
		return false;
	}
	//该节点不存在时无返回值所以将出错，所以必须在每次调用之前调用函数ifNodeExists()进行检查
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
	//更新buffer状态记录，以便计算buffer status
	void updateBufferStatus()
	{
		bufferSizeSum += buffer.size();
		bufferChangeCount++;
	}

};

