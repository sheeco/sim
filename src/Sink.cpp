#include "Sink.h"
#include "MacProtocol.h"
#include "Configuration.h"

CSink* CSink::sink = nullptr;
int CSink::encounterActive = 0;
int CSink::encounter = 0;


void CSink::generateID()
{
	this->setID(getConfig<int>("sink", "id"));
}

CSink::CSink()
{
	this->generateID();
	this->setName("Sink");
	this->setLocation(getConfig<double>("sink", "x"), getConfig<double>("sink", "y"));
	this->capacityBuffer = getConfig<int>("sink", "buffer");
}

vector<CData> CSink::bufferData(int time, vector<CData> datas)
{
	vector<CData> ack = datas;
	for(auto idata = datas.begin(); idata != datas.end(); ++idata)
	{
		if( idata->getTimeArrival() == INVALID )
			idata->arriveSink(time);
		sink->buffer.push_back(*idata);
	}
	
	return ack;
}
