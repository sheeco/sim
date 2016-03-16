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

//	int ID;  //Sink�ڵ�ı��
//	CCoordinate location;  //Sink�ڵ�y����
//	int time;  //������Ϣ��ʱ���
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

	//TODO: ����ͳһ��ȡ / ���ڶ�ȡ ��
	static int SINK_ID;
	static double SINK_X;
	static double SINK_Y;
	static int BUFFER_CAPACITY;

	//SinkΪ����ģʽ
	static CSink* getSink()
	{
		if(sink == nullptr)
			sink = new CSink();
		return sink;
	}

	//��Node��ȡ����
	bool receiveData(int timeArrival, vector<CData> data) override
	{
		RemoveFromList( data, sink->buffer );
		for(auto idata = data.begin(); idata != data.end(); ++idata)
		{
			idata->arriveSink(timeArrival);
			sink->buffer.push_back(*idata);
		}
		//ʵ��������ͳ��sink���ܺ�
		sink->energyConsumption += CONSUMPTION_BYTE_RECIEVE * CNode::DATA_SIZE * data.size();
		return true;
	}

	vector<CData> sendAllData(_SEND mode) override
	{
		cout << "Errror @ CSink::sendAllData() : This function mustn't be called !" << endl;
		_PAUSE_;
		return vector<CData>();
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
		if( sink->newRoutes.empty() )
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

};
