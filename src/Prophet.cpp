#include "GlobalParameters.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "Prophet.h"
#include "SortHelper.h"

int CProphet::MAX_DATA_TRANS = 0;

#ifdef USE_PRED_TOLERANCE

double CProphet::TOLERANCE_PRED = 0;
//double CProphet::DECAY_TOLERANCE_PRED = 1;

#endif


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
//	//���ؽ�����node����x��������
//	vector<CNode *> nodes = CNode::getNodes();
//	nodes = CSortHelper::mergeSort(nodes);
//
//	//�жϹ���״̬����sinkͶ�����ݣ��ڵ��ͨ��
//	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
//	{
//		////�����������״̬��ֱ������
//		//if( ! (*inode)->isListening() )
//		//	continue;
//
//		bool atSink = false;
//		
//		//�����������״̬������
//		if( (*inode)->isListening() )
//		{
//			if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= CGeneralNode::RANGE_TRANS )
//			{
//				atSink = true;
//				//deliver data to sink
//				flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getSizeBuffer() << " data to Sink )                     " ;
//				CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );
//				(*inode)->updateDeliveryPredsWithSink();
//				++nEncounterAtSink;
//			}
//		}
//
//		//scan other nodes and forward data
//		//inode < jnode�����κνڵ��ֻ��һ��ͨ�Ż���
//		for(vector<CNode *>::iterator jnode = inode + 1; jnode != nodes.end(); ++jnode)
//		{
//			if( (*jnode)->getX() + CGeneralNode::RANGE_TRANS < (*inode)->getX() )
//				continue;
//			if( (*inode)->getX() + CGeneralNode::RANGE_TRANS < (*jnode)->getX() )
//				break;
//			if( CBasicEntity::getDistance( **inode, **jnode ) > CGeneralNode::RANGE_TRANS )
//				continue;
//
//			if( (*inode)->isAtHotspot() || (*jnode)->isAtHotspot() )
//				CNode::encountAtHotspot();
//			else
//				CNode::encountOnRoute();
//
//			//�����������״̬������
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
//						//��Sink�����Ľڵ�����ڽ��յ������ڵ������֮����Sink���ж���Ͷ��
//						if( atSink == true )
//						{
//							flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getSizeBuffer() << " data to Sink )                     " ;
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
//	//�������нڵ��buffer״̬��¼
//	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
//		(*inode)->recordBufferStatus();
//
//	//����̨���ʱ����һλС��
//	double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
//	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl<< endl;
//	sink << currentTime << TAB << nEncounterAtSink << endl;
//	sink.close();
//
//}

CProphet::CProphet() {}

CProphet::~CProphet() {}

bool CProphet::shouldForward(CNode* node, map<int, double> dstPred)
{
	double predNode = node->getDeliveryPreds().find(CSink::getSink()->getID())->second;
	double predDst = dstPred.find(CSink::getSink()->getID())->second;
#ifdef USE_PRED_TOLERANCE
	predDst += TOLERANCE_PRED;
#endif

//	if( predNode == predDst )
//		return Bet(0.5);
//	else
	return ( predDst > predNode );
}

vector<CData> CProphet::getDataForTrans(CNode* node)
{
	vector<CData> datas = node->getAllData();

	if( MAX_DATA_TRANS > 0
		&& datas.size() > MAX_DATA_TRANS )
	{
		datas = CSortHelper::mergeSort(datas, CSortHelper::ascendByTimeBirth);
		if( CNode::MODE_QUEUE == CGeneralNode::_fifo )
			datas = vector<CData>(datas.begin(), datas.begin() + MAX_DATA_TRANS);
		else
			datas = vector<CData>(datas.rbegin(), datas.rbegin() + MAX_DATA_TRANS);
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
