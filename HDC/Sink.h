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
	vector<CData> buffer;
	vector<CRoute> newRoutes;
	static CSink* sink;

public:
	CSink(void)
	{}
	~CSink(void);

	//Sink为单例模式
	static CSink* getSink()
	{
		if(sink == NULL)
		{
			sink = new CSink();
			if(sink == NULL)
			{
				cout<<"Error: CSink::getSink() 内存不足 sink = NULL"<<endl;
				_PAUSE;
			}
			sink->ID = SINK_ID;
			sink->x = SINK_X;
			sink->y = SINK_Y;
			sink->time = 0;
		}
		return sink;
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
		if(sink->newRoutes.empty())
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
	//从MANode收取数据
	static void receiveData(vector<CData> data, int timeArrival);


};

