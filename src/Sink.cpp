#include "Sink.h"
#include "MacProtocol.h"

CSink* CSink::sink = nullptr;
int CSink::encounterActive = 0;
int CSink::encounter = 0;


CFrame* CSink::sendRTS(int currentTime) 
{
	vector<CPacket*> packets;
	packets.push_back( new CCtrl(ID, currentTime, getConfig<int>("data", "size_ctrl"), CCtrl::_rts) );
	CFrame* frame = new CFrame(*this, packets);

	return frame;
}

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
