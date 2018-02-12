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

//	int ID;  //Sink�ڵ�ı��
//	CCoordinate location;  //Sink�ڵ�y����
//	int time;  //������Ϣ��ʱ���
//	bool flag;
//	vector<CData> buffer;


private:

	static CSink* sink;
	vector<CRoute> newRoutes;

	static int encounterActive;  //��Ч����
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

	//SinkΪ����ģʽ
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

	//����������ͳ�ƽڵ�� sink ������
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


//	//��Node��ȡ����
//	bool receiveData(int timeArrival, vector<CData> data) override
//	{
//		RemoveFromList( data, sink->buffer );
//		for(auto idata = data.begin(); idata != data.end(); ++idata)
//		{
//			idata->arriveSink(timeArrival);
//			sink->buffer.push_back(*idata);
//		}
//		//ʵ��������ͳ��sink���ܺ�
//		sink->energyConsumption += configs.trans.CONSUMPTION_BYTE_RECEIVE * configs.data.SIZE_DATA * data.size();
//		return true;
//	}

//	vector<CData> sendAllData(_SEND mode) override
//	{
//		cout << "Errror @ CSink::sendAllData() : This function mustn't be called !" << endl;
//		_PAUSE_;
//		return vector<CData>();
//	}


	/********************************************* HAR·�� ***********************************************/

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

#endif // __SINK_H__
