#pragma once

#ifndef __SINK_H__
#define __SINK_H__

#include "BasicEntity.h"
#include "Data.h"
#include "Route.h"
#include "GeneralNode.h"
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

	static int encounterActive;  //��Ч����
	static int encounter;

	CSink()
	{
		this->ID = SINK_ID;
		this->setLocation(SINK_X, SINK_Y);		
		this->capacityBuffer = CAPACITY_BUFFER;
	}

	~CSink(){};


public:

	// TODO: ����ͳһ��ȡ / ���ڶ�ȡ ��
	static int SINK_ID;
	static double SINK_X;
	static double SINK_Y;
	static int CAPACITY_BUFFER;

	//SinkΪ����ģʽ
	static CSink* getSink()
	{
		if(sink == nullptr)
			sink = new CSink();
		return sink;
	}

	bool isListening() const
	{
		return true;
	}

	CPackage* sendRTS(int currentTime);

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

//	void receivePackage(CPackage* package, int currentTime) override;


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
//		sink->energyConsumption += CONSUMPTION_BYTE_RECEIVE * CNode::SIZE_DATA * data.size();
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
