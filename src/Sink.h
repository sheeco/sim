#pragma once

#ifndef __SINK_H__
#define __SINK_H__

#include "Data.h"
#include "Route.h"
#include "GeneralNode.h"


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

	inline void generateID() override;
	CSink();

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

};

#endif // __SINK_H__
