#pragma once

#include "Coordinate.h"
#include "Global.h"


class CCTrace :
	public CGeoEntity
{
private:

	map<int, CCoordinate> trace;  //ʱ������������
	int lengthTrace;  //��ֹʱ��

	inline void init()
	{
		lengthTrace = 0;
	}

public:

	static bool CONTINUOUS_TRACE;  //�Ƿ񽫴ӹ켣�ļ��еõ���ɢ��ģ���Ϊ����������
	static int SLOT_TRACE;  //�ƶ�ģ���е� slot���������ļ��е�����eg. NCSUģ����Ϊ30����ֵΪ 0 ʱ����ʾ�޹̶���ʱ�ۣ�
	static string EXTENSION_TRACE;

	CCTrace();
	~CCTrace();

	//����Ƿ����ĳһʱ��ĺϷ��켣���������켣���ȣ�
	inline bool isValid(int time)
	{
		return time <= lengthTrace;
	}

	inline void addLocation(int time, CCoordinate location)
	{
		if( trace.find(time) != trace.end() )
		{
			stringstream error;
			error << "Error @ CCTrace::addTrace() : Duplicate locations at Time " << time << " ! ";
			cout << endl << error.str() << endl;
			_PAUSE_;
			Exit(ENOEXEC, error.str());
		}
		else
		{
			trace.insert(pair<int, CCoordinate>(time, location));
			if( time > lengthTrace )
				lengthTrace = time;
		}
	}

	//����֮ǰӦ��ʹ�� isValid() ���кϷ��Լ�飬������ܱ���
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
			double ratio = ( time - fromTime ) / ( toTime - fromTime );
			return fromLocation + ratio * ( toLocation - fromLocation );
		}
		else
			return fromLocation;

	}

	static string getTraceFilename(int nodeID)
	{
		char buffer[30] = { '\0' };
		sprintf(buffer, "%d%s", nodeID, EXTENSION_TRACE.c_str());
		string filename = PATH_TRACE + DATASET + "/" + buffer;  //.exe �ļ������� bin/ �ļ�����
		return filename;
	}

	//���ļ��н����켣
	static CCTrace* getTraceFromFile(string filename);

};

