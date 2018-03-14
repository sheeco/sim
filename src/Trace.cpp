#include "Trace.h"
#include "Configuration.h"
#include "FileHelper.h"

Trace::Trace()
{
	init();
}

Trace::Trace(bool continuous)
{
	init();
	this->continuous = continuous;
}

Trace::~Trace()
{
}

int Trace::getInterval()
{
	return getConfig<int>("trace", "interval");
}

CCoordinate Trace::getLocation(int time)
{
	if(!this->isValid(time))
		throw pair<int, string>(EPARSE, string("Trace::getLocation() : Cannot find trace at Time " + STRING(time) + " ( should call Trace::isValid() for assertion ) "));

	//TODO: add dynamic slot_trace
	int interval = getConfig<int>("trace", "interval");
	if(interval <= 0)
		throw pair<int, string>(EPARSE, string("Trace::getLocation() : trace.interval = " + STRING(interval)));

	CCoordinate fromLocation, toLocation;
	int fromTime = 0, toTime = 0;
	if(time % interval == 0)
		return trace[time];

	fromTime = time - ( time % interval );
	fromLocation = trace[fromTime];
	toTime = fromTime + interval;
	toLocation = trace[toTime];

	if(this->continuous)
	{
		double ratio = double(time - fromTime) / double(toTime - fromTime);
		return fromLocation + ratio * ( toLocation - fromLocation );
	}
	else
		return fromLocation;

}

Trace Trace::readTraceFromFile(string filename, bool continuous)
{
	Trace trace(continuous);

	try
	{
		if( ! CFileHelper::IfExists(filename) )
		{
			throw pair<int, string>(EFILE, string("Trace::readTraceFromFile() : Cannot find file \"" + filename + "\" ! ") );
		}
		FILE *file;
		file = fopen(filename.c_str(), "rb");

		while( !feof(file) )
		{
			double temp_time = INVALID;
			double temp_x = INVALID;
			double temp_y = INVALID;
			fscanf(file, "%lf %lf %lf", &temp_time, &temp_x, &temp_y);

			if( temp_time < 0 )
				break;
			else if( temp_time > getConfig<int>("simulation", "runtime") + getConfig<int>("trace", "interval") )
				break;

			trace.addLocation(int(temp_time), CCoordinate(temp_x, temp_y) );
		}

		fclose(file);
		return trace;
	}
	catch( exception e )
	{
		throw string("Trace::readTraceFromFile() : ") + e.what();
	}
}