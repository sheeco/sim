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
//	int ID;  //node编号
//	double x;  //node现在的x坐标
//	double y;  //node现在的y坐标
//	int time;  //更新node坐标及工作状态的时间戳
//	bool flag;

private:

	double generationRate;
	vector<int> summaryVector;
	map<CNode *, int> spokenCache;
	double dutyCycle;
	int SLOT_LISTEN;  //由SLOT_TOTAL和DC计算得到
	int SLOT_SLEEP;  //由SLOT_TOTAL和DC计算得到
	int state;  //取值范围在[ - SLOT_TOTAL, + SLOT_LISTEN )之间，值大于等于0即代表Listen状态
	int timeDeath;  //节点失效时间，默认值为-1
	CHotspot *atHotspot;


	//用于统计输出节点的buffer状态信息
	int bufferSizeSum;
	int bufferChangeCount;

	static int ID_COUNT;
	static double SUM_ENERGY_CONSUMPTION;

	static vector<CNode *> nodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
	static vector<int> idNodes;  //用于储存所有传感器节点的ID，便于处理
	static vector<CNode *> deadNodes;  //能量耗尽的节点


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


	//不设置能量值时，始终返回true
	bool isAlive()
	{
		if( energy == 0 )
			return true;
		else if( energy - energyConsumption <= 0 )
			return false;
		else
			return true;
	}

	//将删除过期的消息（仅当使用TTL时）
	void dropOverdueData(int currentTime)
	{
		if( buffer.empty() )
			return;
		if( CData::useHOP() )
			return;

		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); )
		{
			idata->updateStatus(currentTime);
			//如果TTL过期，丢弃
			if( idata->isOverdue() )
				idata = buffer.erase( idata );
			else
				idata++;
		}
	}	
	
	//将按照(1)“其他节点-本节点”(2)“旧-新”的顺序对buffer中的数据进行排序
	//超出MAX_QUEUE_SIZE时从前端丢弃数据，溢出时将从从前端丢弃数据
	//注意：必须在dropOverdueData之后调用
	void dropDataIfOverflow(int currentTime);

	//注意：需要在每次收到数据之后、投递数据之后、生成新数据之后调用此函数
	void updateBufferStatus(int currentTime)
	{
		dropOverdueData(currentTime);
		dropDataIfOverflow(currentTime);
	}

	//更新SV（HOP <= 1的消息不会放入SV）
	//注意：应确保调用之前buffer状态为最新，且需要在每次发送SV、收到SV并计算requestList之前调用此函数
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
		return ZERO( dutyCycle - HOTSPOT_DUTY_CYCLE );
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

	//在热点处提高 dc
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
	
	//在非热点处降低 dc
	void resetDutyCycle()
	{
		if( ZERO( this->dutyCycle - DEFAULT_DUTY_CYCLE ) )
			return;

		dutyCycle = DEFAULT_DUTY_CYCLE;
		SLOT_LISTEN = SLOT_TOTAL * dutyCycle;
		SLOT_SLEEP = SLOT_TOTAL - SLOT_LISTEN;

		//完成本次监听之后再休眠
		if( state > SLOT_LISTEN )
			state = SLOT_LISTEN;
	}

	//更新坐标和工作状态，返回是否出于工作状态
	bool updateStatus(int currentTime);

	//注意：所有监听动作都应该在调用此函数判断之后进行，调用此函数之前必须确保已updateStatus
	bool isListening()
	{
		if( ZERO(dutyCycle) )
			return false;

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

	//FIXME: 如果不忽略传输延迟，则以下所有send和receive函数都必须加入传输延迟的计算

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

	//传入对方节点的SV，计算并返回请求传输的数据ID列表
	vector<int> sendRequestList(vector<int> sv)
	{
		if( sv.empty() )
			 return vector<int>();

		updateSummaryVector();
		RemoveFromList(sv, summaryVector);
		//待测试
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

	//当且仅当遇到sink时调用
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

	//更新buffer状态记录，以便计算buffer status
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

};

