#include "Trace.h"
#include "Configuration.h"
#include "FileHelper.h"

CCTrace::CCTrace()
{
	init();
}

CCTrace::~CCTrace()
{
}

CCTrace* CCTrace::readTraceFromFile(string filename)
{
	CCTrace* trace = nullptr;

	try
	{
		// TODO: 文件第一行读取 SLOT_MOBILITY

		if( ! CFileHelper::IfExists(filename) )
		{
			throw pair<int, string>(EFILE, string("CCTrace::getTraceFromFile() : Cannot find file \"" + filename + "\" ! ") );
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

			if( temp_time > configs.simulation.RUNTIME + configs.trace.SLOT_TRACE )
				break;

			trace->addLocation(int(temp_time), CCoordinate(temp_x, temp_y) );
		}

		fclose(file);
		return trace;
	}
	catch( exception e )
	{
		throw string("CCTrace::getTraceFromFile() : Unknown error without assertion");
	}
}