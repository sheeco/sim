#pragma once

#include "BasicEntity.h"
#include "Data.h"
#include "Route.h"

using namespace std;

class CSink :
	public CGeneralNode
{
//protected:
//	int ID;  //Sink�ڵ�ı��
//	double x;  //Sink�ڵ�x����
//	double y;  //Sink�ڵ�y����
//	int time;  //������Ϣ��ʱ���
//	bool flag;

private:
	vector<CData> buffer;
	vector<CRoute> newRoutes;
	static CSink* sink;

public:
	CSink(void)
	{}
	~CSink(void);

	//SinkΪ����ģʽ
	static CSink* getSink()
	{
		if(sink == NULL)
		{
			sink = new CSink();
			if(sink == NULL)
			{
				cout<<"Error: CSink::getSink() �ڴ治�� sink = NULL"<<endl;
				_PAUSE;
			}
			sink->ID = SINK_ID;
			sink->x = SINK_X;
			sink->y = SINK_Y;
			sink->time = 0;
		}
		return sink;
	}

	//ȡ���µ�·�߼���
	static inline void setNewRoutes(vector<CRoute> newRoutes)
	{
		sink->newRoutes = newRoutes;
	}
	static inline vector<CRoute> getNewRoutes()
	{
		return sink->newRoutes;
	}
	//�ж��Ƿ���δ�����ȥ����·��
	static inline bool hasMoreNewRoutes()
	{
		if(sink->newRoutes.empty())
			return false;
		else 
			return true;
	}
	//�����ȵ���hasMoreNewRoutes�ж�
	static inline CRoute popRoute()
	{
		CRoute result = sink->newRoutes[0];
		sink->newRoutes.erase(sink->newRoutes.begin());
		return result;
	}
	//��MANode��ȡ����
	static void receiveData(vector<CData> data, int timeArrival);


};

