#pragma once

#ifndef __SINK_H__
#define __SINK_H__

#include "BasicEntity.h"
#include "Data.h"
#include "Route.h"
#include "GeneralNode.h"
#include "Node.h"


class CSink :
	virtual public CGeneralNode
{
//protected:

//	int ID;  //Sink节点的编号
//	CCoordinate location;  //Sink节点y坐标
//	int time;  //更新信息的时间戳
//	bool flag;
//	vector<CData> buffer;


private:

	static CSink* sink;
	vector<CRoute> newRoutes;

	static int encounterActive;  //有效相遇
	static int encounter;

	CSink()
	{
		this->ID = configs.sink.SINK_ID;
		this->setName("Sink");
		this->setLocation( configs.sink.X, configs.sink.Y);		
		this->capacityBuffer = configs.sink.CAPACITY_BUFFER;
	}

	~CSink(){};


public:

	//Sink为单例模式
	static CSink* getSink()
	{
		if(sink == nullptr)
			sink = new CSink();
		return sink;
	}

	bool isAwake() const
	{
		return true;
	}

	CFrame* sendRTS(int currentTime);

	//void Occupy(int time) override
	//{
	//	return;
	//}

	void updateStatus(int time) override
	{
		//updateTimerOccupied(time);

		this->time = time;
	}

	//相遇计数：统计节点和 sink 的相遇
	static void encount() 
	{
		++encounter;
	}
	static void encountActive() 
	{
		++encounterActive;
	}

	static int getEncounter() 
	{
		return encounter;
	}
	static int getEncounterActive()
	{
		return encounterActive;
	}
	static double getPercentEncounterActive() 
	{
		if(encounterActive == 0)
			return 0.0;
		return double(encounterActive) / double(encounter);
	}

	static vector<CData> bufferData(int time, vector<CData> datas);

//	void receiveFrame(CFrame* frame, int currentTime) override;


//	//从Node收取数据
//	bool receiveData(int timeArrival, vector<CData> data) override
//	{
//		RemoveFromList( data, sink->buffer );
//		for(auto idata = data.begin(); idata != data.end(); ++idata)
//		{
//			idata->arriveSink(timeArrival);
//			sink->buffer.push_back(*idata);
//		}
//		//实际上无需统计sink的能耗
//		sink->energyConsumption += configs.trans.CONSUMPTION_BYTE_RECEIVE * configs.data.SIZE_DATA * data.size();
//		return true;
//	}

//	vector<CData> sendAllData(_SEND mode) override
//	{
//		cout << "Errror @ CSink::sendAllData() : This function mustn't be called !" << endl;
//		_PAUSE_;
//		return vector<CData>();
//	}


	/********************************************* HAR路由 ***********************************************/

	//取得新的路线集合
	static inline void setNewRoutes(vector<CRoute> newRoutes)
	{
		sink->newRoutes = newRoutes;
	}
	static inline vector<CRoute> getNewRoutes()
	{
		return sink->newRoutes;
	}
	//判断是否还有未分配出去的新路线
	static inline bool hasMoreNewRoutes()
	{
		if( sink->newRoutes.empty() )
			return false;
		else 
			return true;
	}
	//必须先调用hasMoreNewRoutes判断
	static inline CRoute popRoute()
	{
		CRoute result = sink->newRoutes[0];
		sink->newRoutes.erase(sink->newRoutes.begin());
		return result;
	}

};

#endif // __SINK_H__
