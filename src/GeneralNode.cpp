#include "GeneralNode.h"
#include "Node.h"

double CGeneralNode::CONSUMPTION_BYTE_SEND = 0.008;  //( mJ / Byte )
double CGeneralNode::CONSUMPTION_BYTE_RECEIVE = 0.004;
double CGeneralNode::CONSUMPTION_LISTEN = 13.5;  // ( mJ / s )
double CGeneralNode::CONSUMPTION_SLEEP = 0.015;

int CGeneralNode::RANGE_TRANS = 0;  //transmission range
double CGeneralNode::PROB_TRANS = 0;
int CGeneralNode::SIZE_CTRL = 0;


//vector<CData> CGeneralNode::sendAllData(_SEND mode)
//{
//	double bet = RandomFloat(0, 1);
//	if( bet > PROB_TRANS )
//	{
//		energyConsumption += buffer.size() * CNode::SIZE_DATA * CONSUMPTION_BYTE_SEND;
//		return vector<CData>();
//	}
//
//	if(buffer.empty())
//		return vector<CData>();
//	else
//	{
//		vector<CData> data = buffer;
//		energyConsumption += buffer.size() * CNode::SIZE_DATA * CONSUMPTION_BYTE_SEND;
//		if(mode == _dump)
//			buffer.clear();
//		return data;
//	}
//}

//void CGeneralNode::receivePackage(CPackage* package, int currentTime)
//{
//	
//}

//vector<CData> CGeneralNode::bufferData(int time, vector<CData> datas)
//{
//	return vector<CData>();
//}

