#pragma once

#include "BasicEntity.h"
#include "Data.h"
#include "Route.h"

using namespace std;

extern double SINK_X;
extern double SINK_Y;

class CSink :
	public CGeneralNode
{
//protected:
//	int ID;  //Sink节点的编号
//	double x;  //Sink节点x坐标
//	double y;  //Sink节点y坐标
//	int time;  //更新信息的时间戳
//	bool flag;
//	vector<CData> buffer;

private:

	static CSink* sink;
	vector<CRoute> newRoutes;

	CSink(void)
	{
		this->ID = SINK_ID;
		this->x = SINK_X;
		this->y = SINK_Y;		
		this->bufferCapacity = BUFFER_CAPACITY_SINK;
	}

	~CSink(void){};


public:

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
		sink->energyConsumption += CONSUMPTION_BYTE_RECIEVE * BYTE_PER_DATA * data.size();
		return true;
	}

	vector<CData> sendAllData(Mode mode) override
	{
		cout << "Errror @ CSink::sendAllData() : This function mustn't be called !" << endl;
		_PAUSE;
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

