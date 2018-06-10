#pragma once

#ifndef __TRACE_H__
#define __TRACE_H__

#include "Entity.h"
#include "Global.h"


//节点轨迹类
class Trace :
	virtual public CGeoEntity
{
protected:

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

	Trace();
	Trace(bool continuous);;
	~Trace();

	static int getInterval();

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
	//检查是否存在某一时间的合法轨迹（不超出轨迹长度即可）
	inline bool isValid(int time) const
	{
		return time >= range.first && time <= range.second;
	}
	inline int getStartTime() const
	{
		return this->range.first;
	}
	inline int getEndTime() const
	{
		return this->range.second;
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
		//	throw pair<int, string>(EFORMAT, string("Trace::addTrace() : Duplicate locations at Time " + time + " ! ") );
		//}

		trace.insert(pair<int, CCoordinate>(time, location));
		if( range.first == INVALID || time < range.first )
			range.first = time;
		if( time > range.second )
			range.second = time;
	}

	//调用之前应先使用 isValid() 进行合法性检查，否则可能报错；
	CCoordinate getLocation(int time);

	//从文件中解析轨迹
	static Trace readTraceFromFile(string filename, bool continuous);

};

#endif // __TRACE_H__
