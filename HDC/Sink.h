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
//	int ID;  //Sink�ڵ�ı��
//	double x;  //Sink�ڵ�x����
//	double y;  //Sink�ڵ�y����
//	int time;  //������Ϣ��ʱ���
//	bool flag;

private:

	static CSink* sink;

	CSink(void)
	{
		this->ID = SINK_ID;
		this->x = SINK_X;
		this->y = SINK_Y;		
		this->bufferCapacity = BUFFER_CAPACITY_SINK;
	}

	~CSink(void){};


public:

	//SinkΪ����ģʽ
	static CSink* getSink()
	{
		if(sink == NULL)
			sink = new CSink();
		return sink;
	}

	//��Node��ȡ����
	static void receiveData(vector<CData> data, int timeArrival)
	{
		RemoveFromList( data, sink->buffer );
		for(vector<CData>::iterator idata = data.begin(); idata != data.end(); idata++)
		{
			idata->arriveSink(timeArrival);
			sink->buffer.push_back(*idata);
		}
		//ʵ��������ͳ��sink���ܺ�
		sink->energyConsumption += CONSUMPTION_DATA_RECIEVE * SIZE_DATA * data.size();
	}

};

