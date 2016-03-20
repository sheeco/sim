#include "Epidemic.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "SortHelper.h"

int CEpidemic::MAX_QUEUE_SIZE = -1;


//void CEpidemic::SendData(int currentTime)
//{
//	if( ! ( currentTime % SLOT_DATA_SEND == 0 ) )
//		return;
//	cout << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;
//
//	ofstream sink( PATH_ROOT + PATH_LOG + FILE_SINK, ios::app);
//	if(currentTime == 0)
//	{
//		sink << endl << INFO_LOG << endl ;
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
//		//�����������״̬��ֱ������
//		if( ! (*inode)->isListening() )
//			continue;
//
//		if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= CGeneralNode::TRANS_RANGE )
//		{
//			//deliver data to sink
//			flash_cout << "####  ( Node " << (*inode)->getID() << " deliver " << (*inode)->getBufferSize() << " data to Sink )                    " ;
//			CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );
//			++nEncounterAtSink;
//		}
//
//		//scan other nodes and forward data
//		//inode < jnode�����κνڵ��ֻ��һ��ͨ�Ż���
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
//			//if spoken recently, skip
//			if( (*inode)->hasSpokenRecently( (*jnode), currentTime) )
//			{
//				flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )            " ;
//				continue;
//			}
//			//init by node with smaller id
//			CNode *smaller, *greater = nullptr;
//			smaller = (*inode)->getID() < (*jnode)->getID() ? *inode : *jnode ;
//			greater = (*inode)->getID() > (*jnode)->getID() ? *inode : *jnode ;
//
//			bool fail = false;
//			bool skip = false;
//			vector<int> sv;
//			vector<int> req;
//
//			//forward data
//			
//			sv = smaller->sendSummaryVector();
//
//			if( sv.empty() )
//				skip = true;
//			else
//			{
//				sv = greater->receiveSummaryVector( sv );
//				if( sv.empty() )
//					fail = true;
//				else
//				{
//					req = greater->sendRequestList( sv );
//					if( req.empty() )
//						skip = true;
//					else
//						fail = ! greater->receiveData( currentTime, 
//							smaller->sendDataByRequestList(
//							smaller->receiveRequestList( req ) ) ) ;
//				}
//			}
//			if( ! fail )
//			{
//				sv = greater->sendSummaryVector();
//				if( sv.empty() && skip )
//					skip = true;
//				else
//				{
//					skip = false;
//					sv = smaller->receiveSummaryVector( sv );
//					if( sv.empty() )
//						fail = true;
//					else
//					{
//						req = smaller->sendRequestList( sv );
//						if( req.empty() )
//							skip = true;
//						else
//							fail = ! smaller->receiveData(currentTime, 
//								greater->sendDataByRequestList(
//								greater->receiveRequestList( req ) ) ) ;
//					}
//				}
//				if( ! fail )
//				{
//					smaller->addToSpokenCache(greater, currentTime);
//					greater->addToSpokenCache(smaller, currentTime);			
//					if( skip )
//						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )            " ;
//					else
//						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communication )            " ;
//				}
//				else
//					flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )            " ;
//			}
//			else
//				flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )            " ;
//
//		}
//
//	}
//
//	//�������нڵ��buffer״̬��¼
//	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
//	{
//		(*inode)->recordBufferStatus();
//	}
//
//	//����̨���ʱ����һλС��
//	double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
//	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl << endl;
//	sink << currentTime << TAB << nEncounterAtSink << endl;
//	sink.close();
//
//}

bool CEpidemic::Operate(int currentTime)
{
//	if( MAC_PROTOCOL == _hdc )
//		CHDC::Operate(currentTime);	
//	else
//		CSMac::Operate(currentTime);
//
//	if( ! CNode::hasNodes(currentTime) )
//		return false;
//
//	SendData(currentTime);
//
//	PrintInfo(currentTime);

	return true;
}