#pragma once

#ifndef __TRACE_H__
#define __TRACE_H__

#include "Coordinate.h"
#include "Global.h"
#include "Configuration.h"


typedef pair<int, CCoordinate> CTraceEntry;

class CCTrace :
	virtual public CGeoEntity
{
private:

	map<int, CCoordinate> trace;  //时间和坐标的序列
	// FIXME: change to rangeTrace
	pair<int, int> range;
	bool continuous;

	inline void init()
	{
		range = pair<int, int>(INVALID, INVALID);
		continuous = false;
	}
public:

	CCTrace();
	CCTrace(bool continuous);;
	~CCTrace();

	static int getInterval()
	{
		return getConfig<int>("trace", "interval");
	}

	inline void setTrace(map<int, CCoordinate> trace)
	{
		this->trace = trace;
	}
	inline map<int, CCoordinate> getTrace() const
	{
		return this->trace;
	}
	inline bool hasEntry(int time) const
	{
		return trace.find(time) != trace.end();
	}
	//检查是否存在某一时间的合法轨迹（不超出轨迹长度）
	inline bool isValid(int time) const
	{
		return time >= range.first && time <= range.second;
	}
	inline pair<int, int> getRange() const
	{
		return this->range;
	}
	inline int length() const
	{
		return trace.size();
	}

	inline void addLocation(int time, CCoordinate location)
	{
		// 部分数据集的轨迹文件中可能存在重复行，因此暂时不做此检查
		//if( trace.find(time) != trace.end() )
		//{
		//	throw pair<int, string>(EFORMAT, string("CCTrace::addTrace() : Duplicate locations at Time " + time + " ! ") );
		//}

		trace.insert(pair<int, CCoordinate>(time, location));
		if( range.first == INVALID || time < range.first )
			range.first = time;
		if( time > range.second )
			range.second = time;
	}

	//调用之前应先使用 isValid() 进行合法性检查，否则可能报错；
	inline CCoordinate getLocation(int time)
	{
		if(! this->isValid(time) )
			throw pair<int, string>(EPARSE, string("CCTrace::getLocation() : Cannot find trace at Time " + STRING(time) + " ( should call CCTrace::isValid() for assertion ) "));

		//TODO: add dynamic slot_trace
		int interval = getConfig<int>("trace", "interval");
		if( interval <= 0 )
			throw pair<int, string>(EPARSE, string("CCTrace::getLocation() : trace.interval = " + STRING(interval)));

		CCoordinate fromLocation, toLocation;
		int fromTime = 0, toTime = 0;
		if( time % interval == 0 )
			return trace[time];

		fromTime = time - ( time % interval );
		fromLocation = trace[fromTime];
		toTime = fromTime + interval;
		toLocation = trace[toTime];

		if( this->continuous )
		{
			double ratio = double(time - fromTime) / double(toTime - fromTime);
			return fromLocation + ratio * ( toLocation - fromLocation );
		}
		else
			return fromLocation;

	}

	//从文件中解析轨迹
	static CCTrace* readTraceFromFile(string filename, bool continuous);

};

#endif // __TRACE_H__
