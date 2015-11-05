#include "Node.h"

double CNode::energyConsumption = 0;

CNode::~CNode(void)
{
}

void CNode::generateData(int time)
{
	int nData = generationRate * SLOT_DATA_GENERATE;
	for(int i = 0; i < nData; i++)
	{
		CData data(ID, time);
		if(buffer.size() > capacityBuffer)
		{
			cout<<"Error: CNode::generateData() buffer overflown"<<endl;
			_PAUSE;
		}
		else if(buffer.size() == capacityBuffer)
		{
			buffer.erase(buffer.begin());  //如果buffer已满，删除最早的一个Data
			CData::overflow();
		}
		buffer.push_back(data);
	}
}

vector<CData> CNode::sendData(int num)
{
	if( buffer.empty() )
	{
		vector<CData> data;
		return data;
	}
	//FIFO
	if(num > buffer.size())
		num = buffer.size();
	vector<CData>::iterator begin = buffer.begin();
	vector<CData> data(begin, begin + num);
	buffer.erase(begin, begin + num);
	//Consume Energy
	energyConsumption += num * (CONSUMPTION_DATA_SEND * 3 + CONSUMPTION_DATA_RECIEVE * 4);
	return data;
}

vector<CData> CNode::sendAllData()
{
	vector<CData> data = buffer;
	energyConsumption += buffer.size() * (CONSUMPTION_DATA_SEND * 3 + CONSUMPTION_DATA_RECIEVE * 4);
	buffer.clear();
	return data;
}

void CNode::failSendData()
{
	energyConsumption += buffer.size() * (CONSUMPTION_DATA_SEND * 3 + CONSUMPTION_DATA_RECIEVE * 4);
}