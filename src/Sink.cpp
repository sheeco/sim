#include "Sink.h"
#include "MacProtocol.h"

CSink* CSink::sink = nullptr;
int CSink::encounterActive = 0;
int CSink::encounter = 0;
int CSink::SINK_ID = 0; //0为sink节点预留，传感器节点ID从1开始
double CSink::SINK_X = 0;
double CSink::SINK_Y = 0;
int CSink::CAPACITY_BUFFER = 999999;  //无限制

// TODO: untested
//void CSink::receivePackage(CPackage* package, int currentTime)
//{
//	energyConsumption += package->getSize() * CONSUMPTION_BYTE_RECIEVE;
//
//	vector<CGeneralData*> contents = package->getContent();
//	CGeneralNode* dst = package->getSrcNode();
//	CCtrl* ctrlToSend = nullptr;
//
//	for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); )
//	{
//		if( typeid(**icontent) == typeid(CCtrl) )
//		{
//			CCtrl* ctrl = dynamic_cast<CCtrl*>(*icontent);
//			switch( ctrl->getType() )
//			{
//			case CCtrl::_rts:
//				break;
//
//			case CCtrl::_cts:
//
//				// TODO: connection established ?
//
//				break;
//			
//			case CCtrl::_index:
//				break;
//			
//			case CCtrl::_ack:
//				break;
//
//			default:
//				break;
//			}
//
//			++icontent;
//		}
//		else if( typeid(**icontent) == typeid(CData) )
//		{
//			//extract data content
//			vector<CData> datas;
//			do
//			{
//				datas.push_back( *dynamic_cast<CData*>(*icontent) );
//				++icontent;
//			} while( icontent != contents.end() );
//			
//			//accept data into buffer
//			vector<CData> ack;
//			ack = bufferData(currentTime, datas);
//
//			//ACK
//			ctrlToSend = new CCtrl(ID, ack, currentTime, SIZE_CTRL, CCtrl::_ack);
//		}
//	}
//
//	free(package);
//	CPackage* packageToSend = nullptr;
//	if( ctrlToSend != nullptr )
//	{
//		packageToSend = new CPackage(*this, *dst, *ctrlToSend);		
//		energyConsumption += packageToSend->getSize() * CONSUMPTION_BYTE_SEND;
//		CMacProtocol::transmitPackage( packageToSend, dst, currentTime );
//	}
//
//}

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
