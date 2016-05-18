#include "Trace.h"
#include "FileHelper.h"

bool CCTrace::CONTINUOUS_TRACE = false;
int CCTrace::SLOT_TRACE = 0;
string CCTrace::EXTENSION_TRACE = ".trace";


CCTrace::CCTrace()
{
	init();
}

CCTrace::~CCTrace()
{
}

CCTrace* CCTrace::getTraceFromFile(string filename)
{
	CCTrace* trace = nullptr;

	try
	{
		// TODO: 文件第一行读取 SLOT_MOBILITY

		if( ! CFileHelper::IfExists(filename) )
		{
			stringstream error;
			error << "Error @ CCTrace::getTraceFromFile() : Cannot find file \"" << filename << "\" ! ";
			cout << endl << error.str() << endl;
			_PAUSE_;
			Exit(ENOENT, error.str());
		}
		FILE *file;
		file = fopen(filename.c_str(), "rb");
		trace = new CCTrace();

		while( !feof(file) )
		{
			double temp_time = 0;
			double temp_x = 0;
			double temp_y = 0;
			fscanf(file, "%lf %lf %lf", &temp_time, &temp_x, &temp_y);

			if( temp_time > RUNTIME )
				break;

			trace->addLocation(temp_time, CCoordinate(temp_x, temp_y) );
		}

		fclose(file);
		return trace;
	}
	catch( exception e )
	{
		cout << endl << "Error @ CCTrace::getTraceFromFile() : Unknown error without assertion" << endl;
		_PAUSE_;
		return nullptr;
	}
}