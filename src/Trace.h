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

	map<int, CCoordinate> trace;  //ʱ������������
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
	//����Ƿ����ĳһʱ��ĺϷ��켣���������켣���ȣ�
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
		// �������ݼ��Ĺ켣�ļ��п��ܴ����ظ��У������ʱ�����˼��
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

	//����֮ǰӦ��ʹ�� isValid() ���кϷ��Լ�飬������ܱ���
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

	//���ļ��н����켣
	static CCTrace* readTraceFromFile(string filename, bool continuous);

};

#endif // __TRACE_H__
