#pragma once

#ifndef __TRACE_H__
#define __TRACE_H__

#include "Entity.h"
#include "Global.h"


class Trace :
	virtual public CGeoEntity
{
protected:

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
	//����Ƿ����ĳһʱ��ĺϷ��켣���������켣���ȼ��ɣ�
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
		// �������ݼ��Ĺ켣�ļ��п��ܴ����ظ��У������ʱ�����˼��
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

	//����֮ǰӦ��ʹ�� isValid() ���кϷ��Լ�飬������ܱ���
	CCoordinate getLocation(int time);

	//���ļ��н����켣
	static Trace readTraceFromFile(string filename, bool continuous);

};

#endif // __TRACE_H__
