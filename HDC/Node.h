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
extern _MacProtocol MAC_PROTOCOL;
extern _RoutingProtocol ROUTING_PROTOCOL;


class CNode :
	public CGeneralNode
{
//protected:

//	int ID;  //node编号
//	double x;  //node现在的x坐标
//	double y;  //node现在的y坐标
//	int time;  //更新node坐标、工作状态（、Prophet中的衰减）的时间戳
//	bool flag;


private:

	double generationRate;
	double dutyCycle;

	int SLOT_LISTEN;  //由SLOT_TOTAL和DC计算得到
	int SLOT_SLEEP;  //由SLOT_TOTAL和DC计算得到
	int state;  //取值范围在[ - SLOT_SLEEP, + SLOT_LISTEN )之间，值大于等于0即代表Listen状态
	int timeData;  //上一次数据生成的时间
	int timeDeath;  //节点失效时间，默认值为-1
	CHotspot *atHotspot;

	//用于统计输出节点的buffer状态信息
	int bufferSizeSum;
	int bufferChangeCount;
	static int encounterAtHotspot;
	static int encounterOnRoute;
	static int visiterAtHotspot;
	static int visiterOnRoute;;

	static int ID_COUNT;
	static vector<CNode *> nodes;  //用于储存所有传感器节点，从原来的HAR::CNode::nodes移动到这里
	static vector<int> idNodes;  //用于储存所有传感器节点的ID，便于处理
	static vector<CNode *> deadNodes;  //能量耗尽的节点
	static vector<CNode *> deletedNodes;  //用于暂存Node个数动态变化时被暂时移出的节点

	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CNode *, int> spokenCache;

	/**************************************  Prophet  *************************************/

	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >，sink节点ID为0将位于最前，便于查找

	void init()
	{
		generationRate = 0;
		atHotspot = nullptr;
		SLOT_LISTEN = 0;
		SLOT_SLEEP = 0;
		state = 0;
		timeData = 0;
		timeDeath = 0;
		bufferCapacity = BUFFER_CAPACITY;
		bufferSizeSum = 0;
		bufferChangeCount = 0;
		dutyCycle = DEFAULT_DUTY_CYCLE;
		energy = ENERGY;
	}

	CNode()
	{
		init();
	}

	CNode(double generationRate)
	{
		init();
		this->generationRate = generationRate;
	}

	static void initNodes()
	{
		if( nodes.empty() && deadNodes.empty() )
		{
			for(int i = 0; i < NUM_NODE; i++)
			{
				double generationRate = DEFAULT_DATA_RATE;
				if(i % 5 == 0)
					generationRate *= 5;
				CNode* node = new CNode(generationRate);
				node->generateID();
				node->initDeliveryPreds();
				CNode::nodes.push_back(node);
				CNode::idNodes.push_back( node->getID() );
			}
		}
	}

	void generateData(int currentTime)
	{
		int timeDataIncre = currentTime - timeData;
		int nData = timeDataIncre * generationRate;
		if(nData > 0)
		{
			for(int i = 0; i < nData; ++i)
			{
				CData data(ID, currentTime);
				buffer.push_back(data);
			}
			timeData = currentTime;
		}
		updateBufferStatus(currentTime);
	}

	/*************************************  Epidemic  *************************************/

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
				++idata;
		}
	}	
	
	//队列管理
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
			cout << endl << "Error @ CNode::updateSummaryVector() : Buffer isn't clean !" << endl;
			_PAUSE;
		}

		summaryVector.clear();
		for(vector<CData>::iterator idata = buffer.begin(); idata != buffer.end(); ++idata)
		{

			if( CData::useHOP() && ( ! idata->allowForward() ) )
				continue;
			else
				summaryVector.push_back( idata->getID() );
		}
	}

	/**************************************  Prophet  *************************************/

	void initDeliveryPreds()
	{
		if( ! deliveryPreds.empty() )
			return;

		for(int id = 0; id <= NUM_NODE; id++)
		{
			if( id != ID )
				deliveryPreds[id] = INIT_DELIVERY_PRED;
		}
	}

	void decayDeliveryPreds(int currentTime)
	{
		for(map<int, double>::iterator imap = deliveryPreds.begin(); imap != deliveryPreds.end(); ++imap)
			deliveryPreds[ imap->first ] = imap->second * pow( DECAY_RATIO, ( currentTime - time ) / SLOT_MOBILITYMODEL );
	}


public:

	static int SLOT_TOTAL;
	static double DEFAULT_DUTY_CYCLE;  //不使用HDC，或者HDC中不在热点区域内时的占空比
	static double HOTSPOT_DUTY_CYCLE;  //HDC中热点区域内的占空比

	static double DEFAULT_DATA_RATE;  //( package / s )
	static int DATA_SIZE;  //( Byte )
	static int CTRL_SIZE;

	static int BUFFER_CAPACITY;
	static int ENERGY;
	static Mode RECEIVE_MODE;
	static Mode SEND_MODE;
	static Mode QUEUE_MODE;

	/**************************************  Prophet  *************************************/

	static double INIT_DELIVERY_PRED;
	static double DECAY_RATIO;
	static double TRANS_RATIO;



	~CNode(){};

	static vector<CNode *>& getNodes()
	{
		if( SLOT_TOTAL == 0 || ( ZERO( DEFAULT_DUTY_CYCLE ) && ZERO( HOTSPOT_DUTY_CYCLE ) ) )
		{
			cout << endl << "Error @ CNode::getNodes() : SLOT_TOTAL || ( DEFAULT_DUTY_CYCLE && HOTSPOT_DUTY_CYCLE ) = 0" << endl;
			_PAUSE;
		}

		if( nodes.empty() && deadNodes.empty() )
			initNodes();
		return nodes;
	}

	//包括已经失效的节点和删除的节点，按照ID排序
	static vector<CNode *> getAllNodes(bool sort);

	static vector<int>& getIdNodes()
	{
		if( nodes.empty() && deadNodes.empty() )
			initNodes();
		return idNodes;
	}

	static bool finiteEnergy()
	{
		return ENERGY > 0;
	}

	static bool hasNodes(int currentTime)
	{
		if( nodes.empty() && deadNodes.empty() )
		{
			initNodes();
			return true;
		}
		else
			idNodes.clear();
		
		bool death = false;
		for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); )
		{
			if( (*inode)->isAlive() )
			{
				idNodes.push_back( (*inode)->getID() );
				++inode;
			}
			else
			{
				(*inode)->die( currentTime );
				deadNodes.push_back( *inode );
				inode = nodes.erase( inode );
				death = true;
			}
		}

		if(death)
			cout << "####  [ Node ]  " << CNode::getNodes().size() << endl;

		return ( ! nodes.empty() );
	}

	//待测试
	static void newNodes(int n)
	{
		//优先恢复之前被删除的节点
		for(int i = NUM_NODE; i < NUM_NODE + n; i++)
		{
			if( deletedNodes.empty() )
				break;

			CNode::nodes.push_back(deletedNodes[0]);
			CNode::idNodes.push_back( deletedNodes[0]->getID() );
			++NUM_NODE;
			--n;
		}
		//如果仍不足数，构造新的节点
		for(int i = NUM_NODE; i < NUM_NODE + n; i++)
		{
			double generationRate = DEFAULT_DATA_RATE;
			if(i % 5 == 0)
				generationRate *= 5;
			CNode* node = new CNode(generationRate);
			node->generateID();
			node->initDeliveryPreds();
			CNode::nodes.push_back(node);
			CNode::idNodes.push_back( node->getID() );
			++NUM_NODE;
			--n;
		}			
	}

	//待测试
	static void removeNodes(int n)
	{
		//FIXME: Random selected ?
		vector<CNode *>::iterator start = nodes.begin();
		vector<CNode *>::iterator end = nodes.end();
		vector<CNode *>::iterator fence = nodes.begin();
		fence += NUM_NODE + n;
		vector<CNode *> leftNodes(start, fence);

		//Remove invalid positoins belonging to the deleted nodes
		vector<CNode *> deletedNodes(fence, end);
		vector<int> deletedIDs;
		for(auto inode = deletedNodes.begin(); inode != deletedNodes.end(); ++inode)
			deletedIDs.push_back( (*inode)->getID() );

		for(vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
		{
			if( IfExists(deletedIDs, (*ipos)->getNode()) )
				ipos = CPosition::positions.erase(ipos);
			else
				++ipos;
		}

		nodes = leftNodes;
		NUM_NODE = nodes.size();
		idNodes.clear();
		for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
			idNodes.push_back( (*inode)->getID() );
		CNode::deletedNodes.insert(CNode::deletedNodes.end(), deletedNodes.begin(), deletedNodes.end());
	}

	static bool ifNodeExists(int id)
	{
		for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
		{
			if((*inode)->getID() == id)
				return true;
		}
		return false;
	}

	//该节点不存在时无返回值所以将出错，所以必须在每次调用之前调用函数ifNodeExists()进行检查
	static CNode* getNodeByID(int id)
	{
		for(auto inode = nodes.begin(); inode != nodes.end(); ++inode)
		{
			if((*inode)->getID() == id)
				return *inode;
		}
		return nullptr;
	}

	static double getSumEnergyConsumption()
	{
		double sumEnergyConsumption = 0;
		auto allNodes = CNode::getAllNodes(false);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
			sumEnergyConsumption += (*inode)->getEnergyConsumption();
		return sumEnergyConsumption;
	}

	//相遇计数：HAR中统计节点和MA的相遇，否则统计节点间的相遇计数
	inline static double getEncounterAtHotspotPercent()
	{
		if(encounterAtHotspot == 0)
			return 0.0;
		return double(encounterAtHotspot) / double(encounterAtHotspot + encounterOnRoute);
	}
	inline static int getEncounter()
	{
		return encounterAtHotspot + encounterOnRoute;
	}
	inline static int getEncounterAtHotspot()
	{
		return encounterAtHotspot;
	}
	inline static void encountAtHotspot()
	{
		encounterAtHotspot++;
	}
	inline static void encountOnRoute()
	{
		encounterOnRoute++;
	}

	//访问计数：用于统计节点位于热点内的百分比（HAR路由中尚未添加调用）
	inline static double getVisiterAtHotspotPercent()
	{
		if(visiterAtHotspot == 0)
			return 0.0;
		return double(visiterAtHotspot) / double(visiterAtHotspot + visiterOnRoute);
	}
	inline static int getVisiter()
	{
		return visiterAtHotspot + visiterOnRoute;
	}
	inline static int getVisiterAtHotspot()
	{
		return visiterAtHotspot;
	}
	inline static void visitAtHotspot()
	{
		visiterAtHotspot++;
	}
	inline static void visitOnRoute()
	{
		visiterOnRoute++;
	}


	//不设置能量值时，始终返回true
	bool isAlive() const
	{
		if( energy == 0 )
			return true;
		else if( energy - energyConsumption <= 0 )
			return false;
		else
			return true;
	}
	inline double getEnergy() const
	{
		if( ! isAlive() )
			return 0;
		return energy - energyConsumption;
	}
	inline double getGenerationRate() const
	{
		return generationRate;
	}
	inline CHotspot* getAtHotspot() const
	{
		return atHotspot;
	}
	inline void setAtHotspot(CHotspot* atHotspot)
	{
		this->atHotspot = atHotspot;
	}
	inline bool isAtHotspot() const
	{
		if( MAC_PROTOCOL == _hdc )
			return EQUAL( dutyCycle, HOTSPOT_DUTY_CYCLE );
		else if( ROUTING_PROTOCOL == _har )
			return atHotspot != nullptr;
		else
			return false;
	}
	inline void generateID()
	{
		ID_COUNT++;
		this->ID = ID_COUNT;		
	}
	inline void die(int currentTime)
	{
		this->timeDeath = currentTime;
	}
	inline double getAverageBufferSize() const
	{
		if(bufferSizeSum == 0)
			return 0;
		else
			return double(bufferSizeSum) / double(bufferChangeCount);
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
	bool isListening() const
	{
		if( ZERO(dutyCycle) )
			return false;

		return state >= 0;
	}

	//更新buffer状态记录，以便计算buffer status
	void recordBufferStatus()
	{
		bufferSizeSum += buffer.size();
		bufferChangeCount++;
	}

	vector<CData> sendAllData(Mode mode) override
	{
		return CGeneralNode::sendAllData(mode);
	}

	vector<CData> sendData(int n)
	{
		if( n >= buffer.size() )
			return sendAllData(SEND::DUMP);

		double bet = RandomFloat(0, 1);
		if(bet > PROB_DATA_FORWARD)
		{
			energyConsumption += n * DATA_SIZE * CONSUMPTION_BYTE_SEND;
			return vector<CData>();
		}

		vector<CData> data;
		if( QUEUE_MODE == QUEUE::FIFO )
		{
			data.insert(data.begin(), buffer.begin(), buffer.begin() + n);
		}
		else if( QUEUE_MODE == QUEUE::LIFO )
		{
			data.insert(data.begin(), buffer.end() - n, buffer.end());
		}
		return data;
	}

	bool receiveData(int time, vector<CData> datas) override
	{
		if( datas.empty() )
			return false;
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return false;

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * DATA_SIZE * datas.size();
		for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
			idata->arriveAnotherNode(time);
		buffer.insert(buffer.begin(), datas.begin(), datas.end() );
		updateBufferStatus(time);
		
		return true;
	}
	

	/*************************************  Epidemic  *************************************/

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
		energyConsumption += CONSUMPTION_BYTE_SEND * CTRL_SIZE;
		return summaryVector;
	}

	vector<int> receiveSummaryVector(vector<int> sv)
	{
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return vector<int>();

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * CTRL_SIZE;
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
					++id;
				}
			}
			sv = vector<int>( sv.begin(), id );
		}

		energyConsumption += CONSUMPTION_BYTE_SEND * CTRL_SIZE;
		return sv;
	}
	
	vector<int> receiveRequestList(vector<int> req)
	{
		if( req.empty() )
			return vector<int>();
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return vector<int>();

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * CTRL_SIZE;
		return req;
	}

	vector<CData> sendDataByRequestList(vector<int> requestList)
	{
		if( requestList.empty() )
			return vector<CData>();

		vector<CData> result;
		result = GetItemsByID(buffer, requestList);
		energyConsumption += CONSUMPTION_BYTE_SEND * DATA_SIZE * result.size();
		
		return result;
	}


	/**************************************  Prophet  *************************************/
		
	void updateDeliveryPredsWith(int node, map<int, double> preds)
	{
		double oldPred = 0, transPred = 0, dstPred = 0;
		if( deliveryPreds.find(node) == deliveryPreds.end() )
			deliveryPreds[ node ] = INIT_DELIVERY_PRED;

		oldPred = this->deliveryPreds[ node ];
		deliveryPreds[ node ] = transPred = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;

		for(map<int, double>::iterator imap = preds.begin(); imap != preds.end(); ++imap)
		{
			int dst = imap->first;
			if( dst == this->ID )
				continue;
			if( deliveryPreds.find(node) == deliveryPreds.end() )
				deliveryPreds[ node ] = INIT_DELIVERY_PRED;

			oldPred = this->deliveryPreds[ dst ];
			dstPred = imap->second;
			deliveryPreds[ dst ] = oldPred + ( 1 - oldPred ) * transPred * dstPred * TRANS_RATIO;
		}
		
	}

	void updateDeliveryPredsWithSink()
	{
		double oldPred = deliveryPreds[SINK_ID];
		deliveryPreds[SINK_ID] = oldPred + ( 1 - oldPred ) * INIT_DELIVERY_PRED;
	}

	map<int, double> sendDeliveryPreds(int currentTime)
	{
		energyConsumption += CONSUMPTION_BYTE_SEND * CTRL_SIZE;
		
		return deliveryPreds;
	}

	map<int, double> receiveDeliveryPredsAndSV(int node, map<int, double> preds, vector<int> &sv)
	{
		if( preds.empty() )
			return map<int, double>();
		if( RandomFloat(0, 1) > PROB_DATA_FORWARD )
			return map<int, double>();

		energyConsumption += CONSUMPTION_BYTE_RECIEVE * CTRL_SIZE;
		return preds;
	}

	vector<CData> sendDataByPredsAndSV(CNode *node, map<int, double> preds, vector<int> &sv)
	{
		if( preds.empty() )
			return vector<CData>();

		if( preds.find(SINK_ID)->second > this->deliveryPreds.find(SINK_ID)->second )
		{		
			vector<int> req = summaryVector;
			RemoveFromList(req, sv);
			return sendDataByRequestList( req );
		}
		else
			return vector<CData>();
	}

};

