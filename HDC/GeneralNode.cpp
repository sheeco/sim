#include "GeneralNode.h"
#include "Node.h"

double CGeneralNode::CONSUMPTION_BYTE_SEND = 0.008;  //( mJ / Byte )
double CGeneralNode::CONSUMPTION_BYTE_RECIEVE = 0.004;
double CGeneralNode::CONSUMPTION_LISTEN = 13.5;  // ( mJ / s )
double CGeneralNode::CONSUMPTION_SLEEP = 0.015;
int CGeneralNode::TRANS_RANGE = 100;  //transmission range
double CGeneralNode::PROB_DATA_FORWARD = 1.0;

vector<CData> CGeneralNode::sendAllData(_Send mode)
{
	double bet = RandomFloat(0, 1);
	if( bet > PROB_DATA_FORWARD )
	{
		energyConsumption += buffer.size() * CNode::DATA_SIZE * CONSUMPTION_BYTE_SEND;
		return vector<CData>();
	}

	if(buffer.empty())
		return vector<CData>();
	else
	{
		vector<CData> data = buffer;
		energyConsumption += buffer.size() * CNode::DATA_SIZE * CONSUMPTION_BYTE_SEND;
		if(mode == _dump)
			buffer.clear();
		return data;
	}
}

