#pragma once

#include "BasicEntity.h"
#include "Data.h"
#include "Route.h"

using namespace std;

class CSink :
	public CGeneralNode
{
//protected:
//	int ID;  //Sink节点的编号
//	double x;  //Sink节点x坐标
//	double y;  //Sink节点y坐标
//	int time;  //更新信息的时间戳
//	bool flag;

private:

	static CSink* sink;

	CSink(void)
	{
		if(sink == NULL)
		{
			sink = new CSink();
			sink->ID = SINK_ID;
			sink->x = SINK_X;
			sink->y = SINK_Y;		
			sink->bufferCapacity = BUFFER_CAPACITY_SINK;
		}
	}

	~CSink(void){};


public:

	//Sink为单例模式
	static CSink* getSink()
	{
		if(sink == NULL)
			sink = new CSink();
		return sink;
	}

	//从Node收取数据
	static void receiveData(vector<CData> data, int timeArrival)
	{
		RemoveFromList( data, sink->buffer );
		for(vector<CData>::iterator idata = data.begin(); idata != data.end(); idata++)
		{
			idata->arriveSink(timeArrival);
			sink->buffer.push_back(*idata);
		}
		//实际上无需统计sink的能耗
		sink->energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_DATA * data.size();
	}

};

