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

	map<int, CCoordinate> trace;  //ʱ������������
	int lengthTrace;  //��ֹʱ��

	inline void init()
	{
		lengthTrace = 0;
	}

public:

	CCTrace();
	~CCTrace();

	//����Ƿ����ĳһʱ��ĺϷ��켣���������켣���ȣ�
	inline bool isValid(int time)
	{
		return time <= lengthTrace;
	}

	inline void addLocation(int time, CCoordinate location)
	{
		// �������ݼ��Ĺ켣�ļ��п��ܴ����ظ��У������ʱ�����˼��
		//if( trace.find(time) != trace.end() )
		//{
		//	throw pair<int, string>(EFORMAT, string("CCTrace::addTrace() : Duplicate locations at Time " + time + " ! ") );
		//}

		trace.insert(pair<int, CCoordinate>(time, location));
		if( time > lengthTrace )
			lengthTrace = time;
	}

	//����֮ǰӦ��ʹ�� isValid() ���кϷ��Լ�飬������ܱ���
	inline CCoordinate getLocation(int time)
	{
		CCoordinate fromLocation, toLocation;
		int fromTime = 0, toTime = 0;

		if( configs.trace.SLOT_TRACE > 0 )
		{
			try 
			{ 
				if( time % configs.trace.SLOT_TRACE == 0 )
					return trace[time];

				fromTime = time - ( time % configs.trace.SLOT_TRACE );
				fromLocation = trace[fromTime];
				toTime = fromTime + configs.trace.SLOT_TRACE;
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

			throw pair<int, string>(EPARSE, string("CCTrace::getLocation() : configs.trace.SLOT_TRACE = " + STRING(configs.trace.SLOT_TRACE) ) );
		}

		if( configs.trace.CONTINUOUS_TRACE )
		{
			double ratio = double( time - fromTime ) / double( toTime - fromTime );
			return fromLocation + ratio * ( toLocation - fromLocation );
		}
		else
			return fromLocation;

	}

	//���ļ��н����켣
	static CCTrace* readTraceFromFile(string filename);

};

#endif // __TRACE_H__
