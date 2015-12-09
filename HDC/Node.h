#pragma once

#include "BasicEntity.h"
#include "GeneralNode.h"
#include "Data.h"

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
	int queueSize;  //buffer中存储的非本节点产生的Data的计数，在每次updateStatus之后更新
	vector<int> summaryVector;
	vector<CNode *> recentNodes;


	//用于统计输出节点的buffer状态信息
	int bufferSizeSum;
	int bufferChangeCount;

	static int BUFFER_CAPACITY;
	static int MAX_QUEUE_SIZE;  //同意存储的来自其他节点的data的最大总数，超过该数目将丢弃（是否在Request之前检查？）
	static int ID_COUNT;
	static double energyConsumption;

	//注意：需要在每次接收数据之后手动调用此函数
	//将删除过期的消息（仅当使用TTL时），并更新SV（HOP<=1的消息不会放入SV）
	void updateStatus(int time)
	{
		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); idata++)
		{

		}
	}

public:

	static vector<CNode> nodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
	static vector<int> idNodes;  //用于储存所有传感器节点的ID，便于处理

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
	//传入对方节点的SV，计算并返回请求传输的数据ID列表
	vector<int> calculateRequestList(vector<int> sv)
	{
		RemoveFromList(sv, summaryVector);
		//待测试
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

