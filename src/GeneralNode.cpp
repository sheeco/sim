#include "GeneralNode.h"
#include "Node.h"

double CGeneralNode::CONSUMPTION_BYTE_SEND = 0.008;  //( mJ / Byte )
double CGeneralNode::CONSUMPTION_BYTE_RECIEVE = 0.004;
double CGeneralNode::CONSUMPTION_LISTEN = 13.5;  // ( mJ / s )
double CGeneralNode::CONSUMPTION_SLEEP = 0.015;

int CGeneralNode::TRANS_RANGE = 0;  //transmission range
double CGeneralNode::PROB_DATA_FORWARD = 0;
int CGeneralNode::CTRL_SIZE = 0;


//vector<CData> CGeneralNode::sendAllData(_SEND mode)
//{
//	double bet = RandomFloat(0, 1);
//	if( bet > PROB_DATA_FORWARD )
//	{
//		energyConsumption += buffer.size() * CNode::DATA_SIZE * CONSUMPTION_BYTE_SEND;
//		return vector<CData>();
//	}
//
//	if(buffer.empty())
//		return vector<CData>();
//	else
//	{
//		vector<CData> data = buffer;
//		energyConsumption += buffer.size() * CNode::DATA_SIZE * CONSUMPTION_BYTE_SEND;
//		if(mode == _dump)
//			buffer.clear();
//		return data;
//	}
//}

CPackage* CGeneralNode::sendRTS(int currentTime) 
{
	CCtrl rts(ID, currentTime, CTRL_SIZE, CCtrl::_rts);
	CPackage* package = new CPackage(*this, CGeneralNode(), rts);
	consumeEnergy( package->getSize() * CONSUMPTION_BYTE_SEND );
	return package;
}

//void CGeneralNode::receivePackage(CPackage* package, int currentTime)
//{
//	
//}

//vector<CData> CGeneralNode::bufferData(int time, vector<CData> datas)
//{
//	return vector<CData>();
//}

