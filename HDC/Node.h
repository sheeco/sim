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
//	int ID;  //node编号
//	double x;  //node现在的x坐标
//	double y;  //node现在的y坐标
//	int time;  //更新node坐标的时间戳
//	bool flag;

private:
	double generationRate;
	vector<CData> buffer;
	double energyConsumption;
	int queueSize;  //buffer中存储的非本节点产生的Data的计数，在每次updateStatus之后更新
	vector<int> summaryVector;
	map<CNode *, int> spokenCache;
	double dutyCycle;
	int state;


	//用于统计输出节点的buffer状态信息
	int bufferSizeSum;
	int bufferChangeCount;

	static int STATE_WORK;
	static int STATE_REST;
	static int BUFFER_CAPACITY;
	static int MAX_QUEUE_SIZE;  //同意存储的来自其他节点的data的最大总数，超过该数目将丢弃（是否在Request之前检查？）默认值等于buffer容量
	static int SPOKEN_MEMORY;  //在这个时间内交换过数据的节点暂时不再交换数据

	static int ID_COUNT;
	static double SUM_ENERGY_CONSUMPTION;

	//将删除过期的消息（仅当使用TTL时）
	void dropOverdueData(int currentTime)
	{
		if( CData::useHOP() )
			return;

		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
		{
			idata->updateStatus(currentTime);
			//如果TTL过期，丢弃
			if( idata->isOverdue() )
				idata = buffer.erase( idata );
		}
	}	
	
	//将按照(1)“其他节点-本节点”(2)“旧-新”的顺序对buffer中的数据进行排序
	//超出MAX_QUEUE_SIZE时从前端丢弃数据，溢出时将从从前端丢弃数据
	//注意：必须在dropOverdueData之后调用
	void dropDataIfOverflow(int currentTime);

	//注意：需要在每次收到数据之后、生成新数据之后调用此函数
	void updateBufferStatus(int currentTime)
	{
		dropOverdueData(currentTime);
		dropDataIfOverflow(currentTime);
	}

	//更新SV（HOP <= 1的消息不会放入SV）
	//注意：应确保调用之前buffer状态为最新，且需要在每次向其他节点发送SV之前调用此函数
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

	//注意：需要在每次向其他节点发送SV之前、向sink投递数据之前、收到数据之后、生成新数据之后调用此函数
	//void updateStatus(int currentTime)
	//{
	//	updateBufferStatus(currentTime);
	//	updateSummaryVector();
	//}

public:

	static vector<CNode> nodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
	static vector<int> idNodes;  //用于储存所有传感器节点的ID，便于处理

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

	//当且仅当遇到sink时调用
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
	////该节点不存在时无返回值所以将出错，所以必须在每次调用之前调用函数ifNodeExists()进行检查
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

	//更新buffer状态记录，以便计算buffer status


};

