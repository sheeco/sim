#pragma once

#ifndef __BASIC_NODE_H__
#define __BASIC_NODE_H__

#include "Global.h"
#include "GeneralNode.h"
#include "Hotspot.h"
#include "Frame.h"
#include "Trace.h"

class CBasicNode :
	virtual public CGeneralNode
{
//protected:

//	int ID;  //node编号
//	CCoordinate location;  //node现在的y坐标
//	int time;  //更新node坐标、工作状态（、Prophet中的衰减）的时间戳
//	bool flag;


private:

	CCTrace* trace;
	double dataRate;

	int timeLastData;  //上一次数据生成的时间
	int timeDeath;  //节点失效时间，默认值为-1
	bool recyclable;  //在节点死亡之后，指示节点是否仍可被回收，默认为 true，直到 trace 信息终止赋 false（暂未使用，如果有充电行为则应该读取此参数）
	CHotspot *atHotspot;

	//用于统计输出节点的buffer状态信息
	int sumBufferRecord;
	int countBufferRecord;
	static int encounterAtHotspot;
//	static int encounterActiveAtHotspot;
//	static int encounterActive;  //有效相遇
	static int encounter;
	static int visiterAtHotspot;
	static int visiter;

	static int COUNT_ID;


	void generateData(int currentTime);
	//static void initNodes();


	/****************************************  MAC  ***************************************/


//	CFrame sendCTSWithIndex(CBasicNode* dst, int currentTime);
//	CFrame sendDataWithIndex(CBasicNode* dst, vector<CData> datas, int currentTime);
//	vector<CData> bufferData(int time, vector<CData> datas) override;



	/*************************************  Epidemic  *************************************/

	vector<int> summaryVector;
	map<CBasicNode &, int> spokenCache;

//	vector<CData> getDataBySV(vector<int> sv);
//	vector<CData> getDataBySV(vector<int> sv, int max);


protected:

	CBasicNode();

	CBasicNode(double dataRate);

	virtual ~CBasicNode();


public:

	static int MIN_NUM_NODE;
	static int MAX_NUM_NODE;
	static int INIT_NUM_NODE;

	static int DEFAULT_SLOT_CARRIER_SENSE;  //发送RTS之前，载波侦听的时间

	static double DEFAULT_DATA_RATE;  //( Byte / s )
	static int SIZE_DATA;  //( Byte )

	static int CAPACITY_BUFFER;
	static int CAPACITY_ENERGY;
	// TODO:
	static int SPEED_TRANS;  // Byte / s
	static int LIFETIME_SPOKEN_CACHE;  //在这个时间内交换过数据的节点暂时不再交换数据
	static _RECEIVE MODE_RECEIVE;
	static _SEND MODE_SEND;
	static _QUEUE MODE_QUEUE;


	virtual void init();

	/****************************************  MAC  ***************************************/

	// TODO: trigger operations here ?
	// *TODO: move even between 2 locations ?
	// TODO: how to trigger RTS ?
	//更新所有node的坐标、占空比和工作状态，生成数据；调用之后应调用 isAlive() 检验存活
	void updateStatus(int currentTime);

//	void receiveRTS(CFrame frame);

//	void receiveFrame(CFrame* frame, int currentTime) override;

	bool hasSpokenRecently(CBasicNode& node, int currentTime);

	void addToSpokenCache(CBasicNode& node, int t);

	//由给出的ID列表提取对应数据
	vector<CData> getDataByRequestList(vector<int> requestList) const;

	//（仅MODE_SEND == _dump时）删除 ACK 的数据
	void checkDataByAck(vector<CData> ack);

	// TODO: skip sending RTS if node has received any RTS ? yes if *trans delay countable
	//标记已收到过其他节点的 RTS (暂未使用)
	//bool skipRTS()
	//{
	//	return false;
	//}

	static double calTimeForTrans(CFrame* frame)
	{
		return 0;
		//return ROUND(double(frame->getSize()) / double(SPEED_TRANS));
	}

	/*************************** 能耗相关 ***************************/

	//不设置能量值时，始终返回true
	bool isAlive() const 
	{
		if( CAPACITY_ENERGY == 0 )
			return true;
		else if( CAPACITY_ENERGY - energyConsumption <= 0 )
			return false;
		else
			return true;
	}

	double getEnergy() const 
	{
		if( ! isAlive() )
			return 0;
		return CAPACITY_ENERGY - energyConsumption;
	}

	bool isRecyclable() const 
	{
		return recyclable;
	}

	//如果 recycable 为 true，则可以在动态节点个数测试时被恢复
	//注意：有节点死亡之后，必须调用ClearDeadNodes()
	void die(int currentTime, bool recyclable) 
	{
		this->timeDeath = currentTime;
		this->recyclable = recyclable;
	}

	static bool finiteEnergy();


	/*************************** ------- ***************************/


	/*************************** 队列管理 ***************************/

	bool isFull()
	{
		return buffer.size() == capacityBuffer;
	}

	//手动将数据压入 buffer，不伴随其他任何操作
	//注意：必须在调用此函数之后手动调用 updateBufferStatus() 检查溢出
	void pushIntoBuffer(vector<CData> datas);

	//返回溢出的数据
	//注意：调用之前应该确保数据已排序
	static vector<CData> removeDataByCapacity(vector<CData> &datas, int capacity, bool fromLeft);

//	//将删除过期的消息（仅当使用TTL时），返回移出的数据分组
//	vector<CData> dropOverdueData(int currentTime);

	//将按照(1)“其他节点-本节点”(2)“旧-新”的顺序对buffer中的数据进行排序
	//超出MAX_DATA_RELAY时从前端丢弃数据，溢出时将从前端丢弃数据并返回
	//注意：必须在dropOverdueData之后调用
	vector<CData> dropDataIfOverflow();

	//注意：需要在每次收到数据之后、投递数据之后、生成新数据之后调用此函数
	vector<CData> updateBufferStatus(int currentTime);

	/*************************** ------- ***************************/

	//获取最新 SV（HOP == 0 的消息不会放入SV）
	//注意：应确保调用之前buffer状态为最新
	vector<int> updateSummaryVector();

	//返回节点允许接收的最大数据数
	int getCapacityForward();


	/*************************** 相遇 & 访问 & 传输计数 ***************************/

	//相遇计数：HAR中统计节点和MA的相遇，否则统计节点间的相遇计数
	//注意：暂时只支持Prophet路由，其他路由尚未添加相关代码
	//所有可能的相遇（与MAC和路由协议无关，只与数据集和统计时槽有关）
	static void encount() 
	{
		++encounter;
	}
//	//有效相遇，即邻居节点发现时槽上的节点和监听时槽上的节点（即将触发数据传输）的相遇
//	static void encountActive() 
//	{
//		++encounterActive;
//	}
	//热点区域所有可能的相遇（只与数据集和热点选取有关）
	static void encountAtHotspot() 
	{
		++encounterAtHotspot;
	}
//	//热点区域内的有效相遇，即邻居节点发现时槽上的节点和监听时槽上的节点（即将触发数据传输）的相遇
//	static void encountActiveAtHotspot() 
//	{
//		++encounterActiveAtHotspot;
//	}

	static int getEncounter() 
	{
		return encounter;
	}
//	static int getEncounterActive()
//	{
//		return encounterActive;
//	}
	static int getEncounterAtHotspot() 
	{
		return encounterAtHotspot;
	}
//	static int getEncounterActiveAtHotspot() 
//	{
//		return encounterActiveAtHotspot;
//	}
//	//所有有效相遇中，发生在热点区域的比例
//	static double getPercentEncounterActiveAtHotspot() 
//	{
//		if(encounterActiveAtHotspot == 0)
//			return 0.0;
//		return double(encounterActiveAtHotspot) / double(encounterActive);
//	}
	static double getPercentEncounterAtHotspot() 
	{
		if(encounterAtHotspot == 0)
			return 0.0;
		return double(encounterAtHotspot) / double(encounter);
	}
//	static double getPercentEncounterActive() 
//	{
//		if(encounterActive == 0)
//			return 0.0;
//		return double(encounterActive) / double(encounter);
//	}

	//访问计数：用于统计节点位于热点内的百分比（HAR路由中尚未添加调用）
	static void visitAtHotspot() 
	{
		++visiterAtHotspot;
	}
	static void visit() 
	{
		++visiter;
	}

	static double getPercentVisiterAtHotspot() 
	{
		if(visiterAtHotspot == 0)
			return 0.0;
		return double(visiterAtHotspot) / double(visiter);
	}
	static int getVisiter() 
	{
		return visiter;
	}
	static int getVisiterAtHotspot() 
	{
		return visiterAtHotspot;
	}

	/*************************** ------- ***************************/

	double getDataRate() const 
	{
		return dataRate;
	}

	CHotspot* getAtHotspot() const 
	{
		return atHotspot;
	}

	void setAtHotspot(CHotspot* atHotspot) 
	{
		this->atHotspot = atHotspot;
	}

	bool isAtHotspot() const 
	{
		return atHotspot != nullptr;
	}

	void generateID() 
	{
		++COUNT_ID;
		this->ID = COUNT_ID;		
	}

	void loadTrace()
	{
		trace = CCTrace::getTraceFromFile(CCTrace::getTraceFilename(this->ID));
	}

	double getAverageSizeBuffer() const 
	{
		if(sumBufferRecord == 0)
			return 0;
		else
			return double(sumBufferRecord) / double(countBufferRecord);
	}

	//更新buffer状态记录，以便计算buffer status
	void recordBufferStatus();

//	vector<CData> sendAllData(_SEND mode) override;

//	vector<CData> sendData(int n);

//	bool receiveData(int time, vector<CData> datas) override;

	/*************************************  Epidemic  *************************************/

	//FIXME: 如果不忽略传输延迟，则以下所有send和receive函数都必须加入传输延迟的计算

//	vector<int> sendSummaryVector();

//	vector<int> receiveSummaryVector(vector<int> sv);

//	//传入对方节点的SV，计算并返回请求传输的数据ID列表
//	vector<int> sendRequestList(vector<int> sv);

//	vector<int> receiveRequestList(vector<int> req);


	/**************************************  Prophet  *************************************/

//	map<int, double> sendDeliveryPreds();
//
//	map<int, double> receiveDeliveryPredsAndSV(map<int, double> preds, vector<int>& sv);

};

#endif // __BASIC_NODE_H__
