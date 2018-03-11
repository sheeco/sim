#include "Sink.h"
#include "MacProtocol.h"

CSink* CSink::sink = nullptr;
int CSink::encounterActive = 0;
int CSink::encounter = 0;


vector<CData> CSink::bufferData(int time, vector<CData> datas)
{
	vector<CData> ack = datas;
	RemoveFromList( datas, sink->buffer );
	for(auto idata = datas.begin(); idata != datas.end(); ++idata)
	{
		if( idata->getTimeArrival() == INVALID )
			idata->arriveSink(time);
		sink->buffer.push_back(*idata);
	}
	
	return ack;
}
