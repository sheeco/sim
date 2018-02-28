#include "GeneralNode.h"
#include "Node.h"
#include "MacProtocol.h"


//vector<CData> CGeneralNode::sendAllData(_SEND mode)
//{
//	double bet = RandomFloat(0, 1);
//	if( bet > PROB_TRANS )
//	{
//		energyConsumption += buffer.size() * getConfig<int>("data", "size_data") * getConfig<double>("trans", "consumption_byte_send");
//		return vector<CData>();
//	}
//
//	if(buffer.empty())
//		return vector<CData>();
//	else
//	{
//		vector<CData> data = buffer;
//		energyConsumption += buffer.size() * getConfig<int>("data", "size_data") * getConfig<double>("trans", "consumption_byte_send");
//		if(mode == _dump)
//			buffer.clear();
//		return data;
//	}
//}

//void CGeneralNode::receiveFrame(CFrame* frame, int now)
//{
//	
//}

//vector<CData> CGeneralNode::bufferData(int time, vector<CData> datas)
//{
//	return vector<CData>();
//}

void CGeneralNode::Overhear(int now)
{
	consumeEnergy(getConfig<double>("trans", "consumption_byte_receive") * getConfig<int>("data", "size_header_mac"), now);
}
