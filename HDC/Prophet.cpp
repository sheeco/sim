#include "Prophet.h"
#include "GlobalParameters.h"
#include "HDC.h"
#include "Node.h"
#include "Sink.h"
#include "Preprocessor.h"

extern bool TEST_DYNAMIC_NUM_NODE;
extern MacProtocol MAC_PROTOCOL;
extern RoutingProtocol ROUTING_PROTOCOL;
extern string INFO_SINK;

void Prophet::SendData(int currentTime)
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
	nodes = CPreprocessor::mergeSort(nodes);

	//�жϹ���״̬����sinkͶ�����ݣ��ڵ��ͨ��
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		//�����������״̬��ֱ������
		if( ! (*inode)->isListening() )
			continue;

		if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= TRANS_RANGE )
		{
			//deliver data to sink
			flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )                     " ;
			CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(SEND::DUMP) );
			(*inode)->updateDeliveryPredsWithSink();
			nEncounterAtSink++;
		}

		//scan other nodes and forward data
		//inode < jnode�����κνڵ��ֻ��һ��ͨ�Ż���
		for(vector<CNode *>::iterator jnode = inode + 1; jnode != nodes.end(); ++jnode)
		{
			if( (*jnode)->getX() + TRANS_RANGE < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + TRANS_RANGE < (*jnode)->getX() )
				break;
			if( CBasicEntity::getDistance( **inode, **jnode ) > TRANS_RANGE )
				continue;

			if( (*inode)->isAtHotspot() || (*jnode)->isAtHotspot() )
				CNode::encountAtHotspot();
			else
				CNode::encountOnRoute();

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
			
			preds = smaller->sendDeliveryPreds(currentTime);
			sv = smaller->sendSummaryVector();

			if( preds.empty() )
				skip = true;
			else
			{
				preds = greater->receiveDeliveryPredsAndSV( smaller->getID(), preds, sv );
				if( preds.empty() )
					fail = true;
				else
				{
					greater->updateDeliveryPredsWith(smaller->getID(), preds);
					data = greater->sendDataByPredsAndSV( smaller, preds, sv );
					if( data.empty() )
						skip = true;
					else
						fail = ! smaller->receiveData(currentTime, data) ;
				}
			}
			if( ! fail )
			{
				preds = greater->sendDeliveryPreds(currentTime);
				sv = greater->sendSummaryVector();
				if( preds.empty() && skip )
					skip = true;
				else
				{
					skip = false;
					preds = smaller->receiveDeliveryPredsAndSV( greater->getID(), preds, sv );
					if( preds.empty() )
						fail = true;
					else
					{
						smaller->updateDeliveryPredsWith(greater->getID(), preds);
						data = smaller->sendDataByPredsAndSV( greater, preds, sv );
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
	double deliveryRatio = 0;
	if( CData::getDataArrivalCount() > 0 )
		deliveryRatio = CData::getDataArrivalCount() / double(CData::getDataCount()) * 1000;
	deliveryRatio = ROUND( deliveryRatio );
	deliveryRatio = deliveryRatio / double( 10 );
	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl<< endl;
	sink << currentTime << TAB << nEncounterAtSink << endl;
	sink.close();

}

bool Prophet::Operate(int currentTime)
{
	if( ROUTING_PROTOCOL != _prophet )
		return false;

	//Node Number Test:
	if( TEST_DYNAMIC_NUM_NODE )
		ChangeNodeNumber(currentTime);

	if( ! CNode::hasNodes(currentTime) )
		return false;

	UpdateNodeStatus(currentTime);

	//�����²�Э��HDC���ж��Ƿ�λ���ȵ����򣬸���ռ�ձ�
	if( MAC_PROTOCOL == _hdc )
		CHDC::UpdateDutyCycleForNodes(currentTime);

	GenerateData(currentTime);

	SendData(currentTime);

	PrintInfo(currentTime);

	return true;
}