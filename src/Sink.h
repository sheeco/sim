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

//	int ID;  //Sink节点的编号
//	CCoordinate location;  //Sink节点y坐标
//	int time;  //更新信息的时间戳
//	bool flag;
//	vector<CData> buffer;


private:

	static CSink* sink;
	static int encounterActive;  //有效相遇
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

	//Sink为单例模式
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

	CFrame* sendRTS(int now);

	//void Occupy(int time) override
	//{
	//	return;
	//}

	void updateStatus(int time)
	{
		//updateTimerOccupied(time);

		this->time = time;
	}

	//相遇计数：统计节点和 sink 的相遇
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


//	//从Node收取数据
//	bool receiveData(int timeArrival, vector<CData> data) override
//	{
//		RemoveFromList( data, sink->buffer );
//		for(auto idata = data.begin(); idata != data.end(); ++idata)
//		{
//			idata->arriveSink(timeArrival);
//			sink->buffer.push_back(*idata);
//		}
//		//实际上无需统计sink的能耗
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
