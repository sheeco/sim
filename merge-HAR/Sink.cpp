#include "Sink.h"

CSink* CSink::sink = NULL;

//CSink::CSink(void)
//{
//}

CSink::~CSink(void)
{
}

void CSink::receiveData(vector<CData> data, int timeArrival)
{
	for(vector<CData>::iterator idata = data.begin(); idata != data.end(); idata++)
	{
		//FIXME: ����sink����Ϊ�����ѳɹ�Ͷ��
		idata->arriveSink(timeArrival);
		sink->buffer.push_back(*idata);
	}
	//FIXME: Energy Consumption ?
}
