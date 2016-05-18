#pragma once

#ifndef __TRACE_H__
#define __TRACE_H__

#include "Coordinate.h"
#include "Global.h"


class CCTrace :
	public CGeoEntity
{
private:

	map<int, CCoordinate> trace;  //时间和坐标的序列
	int lengthTrace;  //截止时间

	inline void init()
	{
		lengthTrace = 0;
	}

public:

	static bool CONTINUOUS_TRACE;  //是否将从轨迹文件中得到的散点模拟成为连续的折线
	static int SLOT_TRACE;  //移动模型中的 slot，由数据文件中得来（eg. NCSU模型中为30）；值为 0 时，表示未赋值；值为 -1 时，表示无固定的时槽；
	static string EXTENSION_TRACE;

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
		//	stringstream error;
		//	error << "Error @ CCTrace::addTrace() : Duplicate locations at Time " << time << " ! ";
		//	cout << endl << error.str() << endl;
		//	_PAUSE_;
		//	Exit(ENOEXEC, error.str());
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

		if( SLOT_TRACE > 0 )
		{
			try 
			{ 
				if( time % SLOT_TRACE == 0 )
					return trace[time];

				fromTime = time - ( time % SLOT_TRACE );
				fromLocation = trace[fromTime];
				toTime = fromTime + SLOT_TRACE;
				toLocation = trace[toTime];
			}
			catch(exception e)
			{
				stringstream error;
				error << "Error @ CCTrace::getLocation() : Cannot find trace at Time " << time << " ( should call CCTrace::isValid() for assertion ) ";
				cout << endl << error.str() << endl;
				_PAUSE_;
				Exit(ENOEXEC, error.str());
			}
		}
		else
		{
			//UNDONE: dynamic slot_trace

			stringstream error;
			error << "Error @ CCTrace::getLocation() : SLOT_TRACE = " << SLOT_TRACE;
			cout << endl << error.str() << endl;
			_PAUSE_;
			Exit(ENOEXEC, error.str());
		}

		if( CONTINUOUS_TRACE )
		{
			double ratio = (double)( time - fromTime ) / (double)( toTime - fromTime );
			return fromLocation + ratio * ( toLocation - fromLocation );
		}
		else
			return fromLocation;

	}

	static string getTraceFilename(int nodeID)
	{
		char buffer[30] = { '\0' };
		sprintf(buffer, "%d%s", nodeID, EXTENSION_TRACE.c_str());
		string filename = PATH_TRACE + DATASET + "/" + buffer;  //.exe 文件必须在 bin/ 文件夹下
		return filename;
	}

	//从文件中解析轨迹
	static CCTrace* getTraceFromFile(string filename);

};

#endif // __TRACE_H__
