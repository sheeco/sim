#include "Prophet.h"
#include "GlobalParameters.h"
#include "HDC.h"
#include "Node.h"
#include "Sink.h"
#include "SortHelper.h"
#include "SMac.h"

//extern string INFO_LOG;


void CProphet::SendData(int currentTime)
{
	if( ! ( currentTime % SLOT_DATA_SEND == 0 ) )
		return;
	cout << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;

	ofstream sink("sink.txt", ios::app);
	if(currentTime == 0)
	{
		sink << INFO_LOG ;
		sink << INFO_SINK ;
	}

	int nEncounterAtSink = 0;
	//���ؽ�����node����x��������
	vector<CNode *> nodes = CNode::getNodes();
	nodes = CSortHelper::mergeSort(nodes);

	//�жϹ���״̬����sinkͶ�����ݣ��ڵ��ͨ��
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		////�����������״̬��ֱ������
		//if( ! (*inode)->isListening() )
		//	continue;

		bool atSink = false;
		
		//�����������״̬������
		if( (*inode)->isListening() )
		{
			if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= CGeneralNode::TRANS_RANGE )
			{
				atSink = true;
				//deliver data to sink
				flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )                     " ;
				CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );
				(*inode)->updateDeliveryPredsWithSink();
				nEncounterAtSink++;
			}
		}

		//scan other nodes and forward data
		//inode < jnode�����κνڵ��ֻ��һ��ͨ�Ż���
		for(vector<CNode *>::iterator jnode = inode + 1; jnode != nodes.end(); ++jnode)
		{
			if( (*jnode)->getX() + CGeneralNode::TRANS_RANGE < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + CGeneralNode::TRANS_RANGE < (*jnode)->getX() )
				break;
			if( CBasicEntity::getDistance( **inode, **jnode ) > CGeneralNode::TRANS_RANGE )
				continue;

			if( (*inode)->isAtHotspot() || (*jnode)->isAtHotspot() )
				CNode::encountAtHotspot();
			else
				CNode::encountOnRoute();

			//�����������״̬������
			if( ! ( (*inode)->isListening() && (*jnode)->isListening() ) )
				continue;

			CNode::encountActive();

			//init by node with smaller id
			CNode *smaller, *greater = nullptr;
			smaller = (*inode)->getID() < (*jnode)->getID() ? *inode : *jnode ;
			greater = (*inode)->getID() > (*jnode)->getID() ? *inode : *jnode ;

			bool fail = false;
			bool skip = false;
			map<int, double> preds;
			vector<int> sv;
			vector<CData> data;

			//forward data
			
			preds = smaller->sendDeliveryPreds();
			sv = smaller->sendSummaryVector();

			if( preds.empty() )
				skip = true;
			else
			{
				preds = greater->receiveDeliveryPredsAndSV(preds, sv );
				if( preds.empty() )
					fail = true;
				else
				{
					greater->updateDeliveryPredsWith(smaller->getID(), preds);
					data = greater->sendDataByPredsAndSV(preds, sv );
					if( data.empty() )
						skip = true;
					else
						fail = ! smaller->receiveData(currentTime, data) ;
				}
			}
			if( ! fail )
			{
				preds = greater->sendDeliveryPreds();
				sv = greater->sendSummaryVector();
				if( preds.empty() && skip )
					skip = true;
				else
				{
					skip = false;
					preds = smaller->receiveDeliveryPredsAndSV(preds, sv );
					if( preds.empty() )
						fail = true;
					else
					{
						smaller->updateDeliveryPredsWith(greater->getID(), preds);
						data = smaller->sendDataByPredsAndSV(preds, sv );
						if( data.empty() )
							skip = true;
						else
							fail = ! greater->receiveData( currentTime, data ) ;
					}
				}
				if( ! fail )
				{	
					if( skip )
					{
						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )          " ;
					}
					else
					{
						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communication )          " ;
						
						//��Sink�����Ľڵ�����ڽ��յ������ڵ������֮����Sink���ж���Ͷ��
						if( atSink == true )
						{
							flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )                     " ;
							CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );						
						}

					}
				}
				else
				{
					flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
				}
			}
			else
			{
				flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
			}
		}

	}

	//�������нڵ��buffer״̬��¼
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
		(*inode)->recordBufferStatus();

	//����̨���ʱ����һλС��
	double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl<< endl;
	sink << currentTime << TAB << nEncounterAtSink << endl;
	sink.close();

}

bool CProphet::Operate(int currentTime)
{
	if( MAC_PROTOCOL == _hdc )
		CHDC::Operate(currentTime);	
	else
		CSMac::Operate(currentTime);

	if( ! CNode::hasNodes(currentTime) )
		return false;

	SendData(currentTime);

	PrintInfo(currentTime);

	return true;
}