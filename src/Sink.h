#pragma once

#include "BasicEntity.h"
#include "Data.h"
#include "Route.h"
#include "GeneralNode.h"
#include "HAR.h"
#include "Node.h"


class CSink :
	public CGeneralNode
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

	CSink()
	{
		this->ID = SINK_ID;
		this->setLocation(SINK_X, SINK_Y);		
		this->bufferCapacity = BUFFER_CAPACITY;
	}

	~CSink(){};


public:

	//TODO: 参数统一读取 / 类内读取 ？
	static int SINK_ID;
	static double SINK_X;
	static double SINK_Y;
	static int BUFFER_CAPACITY;

	//Sink为单例模式
	static CSink* getSink()
	{
		if(sink == nullptr)
			sink = new CSink();
		return sink;
	}

	//从Node收取数据
	bool receiveData(int timeArrival, vector<CData> data) override
	{
		RemoveFromList( data, sink->buffer );
		for(auto idata = data.begin(); idata != data.end(); ++idata)
		{
			idata->arriveSink(timeArrival);
			sink->buffer.push_back(*idata);
		}
		//实际上无需统计sink的能耗
		sink->energyConsumption += CONSUMPTION_BYTE_RECIEVE * CNode::DATA_SIZE * data.size();
		return true;
	}

	vector<CData> sendAllData(_SEND mode) override
	{
		cout << "Errror @ CSink::sendAllData() : This function mustn't be called !" << endl;
		_PAUSE_;
		return vector<CData>();
	}

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

