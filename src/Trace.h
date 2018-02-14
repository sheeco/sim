#pragma once

#ifndef __TRACE_H__
#define __TRACE_H__

#include "Coordinate.h"
#include "Global.h"
#include "Configuration.h"


class CCTrace :
	virtual public CGeoEntity
{
private:

	map<int, CCoordinate> trace;  //时间和坐标的序列
	int lengthTrace;  //截止时间

	inline void init()
	{
		lengthTrace = 0;
	}

public:

	CCTrace();
	~CCTrace();

	//检查是否存在某一时间的合法轨迹（不超出轨迹长度）
	inline bool isValid(int time)
	{
		return time <= lengthTrace;
	}

	inline void addLocation(int time, CCoordinate location)
	{
		// 部分数据集的轨迹文件中可能存在重复行，因此暂时不做此检查
		//if( trace.find(time) != trace.end() )
		//{
		//	throw pair<int, string>(EFORMAT, string("CCTrace::addTrace() : Duplicate locations at Time " + time + " ! ") );
		//}

		trace.insert(pair<int, CCoordinate>(time, location));
		if( time > lengthTrace )
			lengthTrace = time;
	}

	//调用之前应先使用 isValid() 进行合法性检查，否则可能报错；
	inline CCoordinate getLocation(int time)
	{
		CCoordinate fromLocation, toLocation;
		int fromTime = 0, toTime = 0;

		if( getConfig<int>("trace", "interval") > 0 )
		{
			try 
			{ 
				if( time % getConfig<int>("trace", "interval") == 0 )
					return trace[time];

				fromTime = time - ( time % getConfig<int>("trace", "interval") );
				fromLocation = trace[fromTime];
				toTime = fromTime + getConfig<int>("trace", "interval");
				toLocation = trace[toTime];
			}
			catch(exception e)
			{
				throw pair<int, string>(EPARSE, string("CCTrace::getLocation() : Cannot find trace at Time " + STRING(time) + " ( should call CCTrace::isValid() for assertion ) ") );
			}
		}
		else
		{
			//TODO: add dynamic slot_trace

			throw pair<int, string>(EPARSE, string("CCTrace::getLocation() : trace.interval = " + STRING(getConfig<int>("trace", "interval")) ) );
		}

		if( getConfig<bool>("trace", "continuous_trace") )
		{
			double ratio = double( time - fromTime ) / double( toTime - fromTime );
			return fromLocation + ratio * ( toLocation - fromLocation );
		}
		else
			return fromLocation;

	}

	//从文件中解析轨迹
	static CCTrace* readTraceFromFile(string filename);

};

#endif // __TRACE_H__
