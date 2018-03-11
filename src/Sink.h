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
	static int encounterActive;  //��Ч����
	static int encounter;

	CSink()
	{
		this->ID = getConfig<int>("sink", "id");
		this->setName("Sink");
		this->setLocation( getConfig<double>("sink", "x"), getConfig<double>("sink", "y"));		
		this->capacityBuffer = getConfig<int>("sink", "buffer");
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

	//void Occupy(int time) override
	//{
	//	return;
	//}

	//TODO: put statistic here?
	void updateStatus(int time)
	{
		//updateTimerOccupied(time);

		this->time = time;
	}
	static void UpdateStatus(int time)
	{
		getSink()->updateStatus(time);
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

//	void receiveFrame(CFrame* frame, int now) override;


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
//		sink->energyConsumption += getConfig<double>("trans", "consumption_byte_receive") * getConfig<int>("data", "size_data") * data.size();
//		return true;
//	}

//	vector<CData> sendAllData(_SEND mode) override
//	{
//		cout << "Errror @ CSink::sendAllData() : This function mustn't be called !" << endl;
//		_PAUSE_;
//		return vector<CData>();
//	}

};

#endif // __SINK_H__
