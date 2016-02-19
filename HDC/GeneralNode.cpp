#include "GeneralNode.h"
#include "Node.h"


vector<CData> CGeneralNode::sendAllData(Mode mode)
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
		if(mode == SEND::DUMP)
			buffer.clear();
		return data;
	}
}

