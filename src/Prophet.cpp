#include "GlobalParameters.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "Prophet.h"
#include "SortHelper.h"

int CProphet::MAX_DATA_TRANS = 0;


//void CProphet::SendData(int currentTime)
//{
//	if( ! ( currentTime % SLOT_DATA_SEND == 0 ) )
//		return;
//	cout << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;
//
//	ofstream sink( PATH_ROOT + PATH_LOG + FILE_SINK, ios::app);
//	if(currentTime == 0)
//	{
//		sink << INFO_LOG << endl ;
//		sink << INFO_SINK ;
//	}
//
//	int nEncounterAtSink = 0;
//	//本地将所有node按照x坐标排序
//	vector<CNode *> nodes = CNode::getNodes();
//	nodes = CSortHelper::mergeSort(nodes);
//
//	//判断工作状态，向sink投递数据，节点间通信
//	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
//	{
//		////如果处于休眠状态，直接跳过
//		//if( ! (*inode)->isListening() )
//		//	continue;
//
//		bool atSink = false;
//		
//		//如果处于休眠状态，跳过
//		if( (*inode)->isListening() )
//		{
//			if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= CGeneralNode::TRANS_RANGE )
//			{
//				atSink = true;
//				//deliver data to sink
//				flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )                     " ;
//				CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );
//				(*inode)->updateDeliveryPredsWithSink();
//				++nEncounterAtSink;
//			}
//		}
//
//		//scan other nodes and forward data
//		//inode < jnode，即任何节点对只有一次通信机会
//		for(vector<CNode *>::iterator jnode = inode + 1; jnode != nodes.end(); ++jnode)
//		{
//			if( (*jnode)->getX() + CGeneralNode::TRANS_RANGE < (*inode)->getX() )
//				continue;
//			if( (*inode)->getX() + CGeneralNode::TRANS_RANGE < (*jnode)->getX() )
//				break;
//			if( CBasicEntity::getDistance( **inode, **jnode ) > CGeneralNode::TRANS_RANGE )
//				continue;
//
//			if( (*inode)->isAtHotspot() || (*jnode)->isAtHotspot() )
//				CNode::encountAtHotspot();
//			else
//				CNode::encountOnRoute();
//
//			//如果处于休眠状态，跳过
//			if( ! ( (*inode)->isListening() && (*jnode)->isListening() ) )
//				continue;
//
//			CNode::encountActive();
//
//			//init by node with smaller id
//			CNode *smaller, *greater = nullptr;
//			smaller = (*inode)->getID() < (*jnode)->getID() ? *inode : *jnode ;
//			greater = (*inode)->getID() > (*jnode)->getID() ? *inode : *jnode ;
//
//			bool fail = false;
//			bool skip = false;
//			map<int, double> preds;
//			vector<int> sv;
//			vector<CData> data;
//
//			//forward data
//			
//			preds = smaller->sendDeliveryPreds();
//			sv = smaller->sendSummaryVector();
//
//			if( preds.empty() )
//				skip = true;
//			else
//			{
//				preds = greater->receiveDeliveryPredsAndSV(preds, sv );
//				if( preds.empty() )
//					fail = true;
//				else
//				{
//					greater->updateDeliveryPredsWith(smaller->getID(), preds);
//					data = greater->sendDataByPredsAndSV(preds, sv );
//					if( data.empty() )
//						skip = true;
//					else
//						fail = ! smaller->receiveData(currentTime, data) ;
//				}
//			}
//			if( ! fail )
//			{
//				preds = greater->sendDeliveryPreds();
//				sv = greater->sendSummaryVector();
//				if( preds.empty() && skip )
//					skip = true;
//				else
//				{
//					skip = false;
//					preds = smaller->receiveDeliveryPredsAndSV(preds, sv );
//					if( preds.empty() )
//						fail = true;
//					else
//					{
//						smaller->updateDeliveryPredsWith(greater->getID(), preds);
//						data = smaller->sendDataByPredsAndSV(preds, sv );
//						if( data.empty() )
//							skip = true;
//						else
//							fail = ! greater->receiveData( currentTime, data ) ;
//					}
//				}
//				if( ! fail )
//				{	
//					if( skip )
//					{
//						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )          " ;
//					}
//					else
//					{
//						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communication )          " ;
//						
//						//在Sink附近的节点可以在接收到其他节点的数据之后向Sink进行二次投递
//						if( atSink == true )
//						{
//							flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )                     " ;
//							CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );						
//						}
//
//					}
//				}
//				else
//				{
//					flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
//				}
//			}
//			else
//			{
//				flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
//			}
//		}
//
//	}
//
//	//更新所有节点的buffer状态记录
//	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
//		(*inode)->recordBufferStatus();
//
//	//控制台输出时保留一位小数
//	double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
//	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl<< endl;
//	sink << currentTime << TAB << nEncounterAtSink << endl;
//	sink.close();
//
//}

CProphet::CProphet() {}

CProphet::~CProphet() {}

vector<CData> CProphet::selectDataByIndex(CNode* node, CCtrl* ctrl)
{
	vector<CData> datas;
	if( ctrl->getPred().find(CSink::getSink()->getID())->second > node->getDeliveryPreds().find(CSink::getSink()->getID())->second )
	{		
		vector<int> req = ctrl->getSV();
		RemoveFromList(req, node->updateSummaryVector());
		datas = node->getDataByRequestList( req );
		if( MAX_DATA_TRANS > 0
			&& datas.size() > MAX_DATA_TRANS )
		{
			datas = CSortHelper::mergeSort(datas, CSortHelper::ascendByTimeBirth);
			if( CNode::QUEUE_MODE == CGeneralNode::_fifo )
				datas = vector<CData>(datas.begin(), datas.begin() + MAX_DATA_TRANS);
			else
				datas = vector<CData>(datas.rbegin(), datas.rbegin() + MAX_DATA_TRANS);
		}
	}

	return datas;
}

vector<CData> CProphet::bufferData(CNode* node, vector<CData> datas, int time)
{
	if( datas.empty() )
		return vector<CData>();

	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
		idata->arriveAnotherNode(time);

	node->pushIntoBuffer(datas);
	vector<CData> ack = datas;

	vector<CData> overflow = node->updateBufferStatus(time);

	RemoveFromList(ack, overflow);

	return ack;
}

bool CProphet::Operate(int currentTime)
{
	if( ! CNode::hasNodes(currentTime) )
		return false;

	if( MAC_PROTOCOL == _hdc )
		CHDC::Operate(currentTime);	
	else
		CSMac::Operate(currentTime);

	PrintInfo(currentTime);

	return true;
}
