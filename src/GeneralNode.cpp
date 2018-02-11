#include "GeneralNode.h"
#include "Node.h"
#include "MacProtocol.h"


//vector<CData> CGeneralNode::sendAllData(_SEND mode)
//{
//	double bet = RandomFloat(0, 1);
//	if( bet > PROB_TRANS )
//	{
//		energyConsumption += buffer.size() * configs.data.SIZE_DATA * configs.trans.CONSUMPTION_BYTE_SEND;
//		return vector<CData>();
//	}
//
//	if(buffer.empty())
//		return vector<CData>();
//	else
//	{
//		vector<CData> data = buffer;
//		energyConsumption += buffer.size() * configs.data.SIZE_DATA * configs.trans.CONSUMPTION_BYTE_SEND;
//		if(mode == _dump)
//			buffer.clear();
//		return data;
//	}
//}

//void CGeneralNode::receiveFrame(CFrame* frame, int currentTime)
//{
//	
//}

//vector<CData> CGeneralNode::bufferData(int time, vector<CData> datas)
//{
//	return vector<CData>();
//}

void CGeneralNode::Overhear(int currentTime)
{
	consumeEnergy(configs.trans.CONSUMPTION_BYTE_RECEIVE * configs.data.SIZE_HEADER_MAC, currentTime);
}
