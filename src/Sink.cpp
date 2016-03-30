#include "Sink.h"
#include "MacProtocol.h"

CSink* CSink::sink = nullptr;
int CSink::encounterActive = 0;
int CSink::encounter = 0;
int CSink::SINK_ID = 0; //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ
double CSink::SINK_X = 0;
double CSink::SINK_Y = 0;
int CSink::CAPACITY_BUFFER = 999999;  //������


CPackage* CSink::sendRTS(int currentTime) 
{
	vector<CGeneralData*> contents;
	contents.push_back( new CCtrl(ID, currentTime, SIZE_CTRL, CCtrl::_rts) );
	CPackage* package = new CPackage(*this, CGeneralNode(), contents);

	return package;
}

vector<CData> CSink::bufferData(int time, vector<CData> datas)
{
	vector<CData> ack = datas;
	RemoveFromList( datas, sink->buffer );
	for(auto idata = datas.begin(); idata != datas.end(); ++idata)
	{
		idata->arriveSink(time);
		sink->buffer.push_back(*idata);
	}
	
	return ack;
}
