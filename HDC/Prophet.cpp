#include "Prophet.h"
#include "GlobalParameters.h"
#include "HDC.h"
#include "Node.h"


void Prophet::UpdateNodeStatus(int currentTime)
{
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
	{
		(*inode)->updateStatus(currentTime);
	}
	//�����²�Э��HDC���ж��Ƿ�λ���ȵ����򣬸���ռ�ձ�
	if( DO_HDC )
		CHDC::UpdateDutyCycleForNodes(currentTime);
}

void Prophet::GenerateData(int currentTime)
{
	CRoutingProtocol::GenerateData(currentTime);
}

void Prophet::SendData(int currentTime)
{
	ofstream sink("sink.txt", ios::app);
	if(currentTime == 0)
	{
		sink << logInfo;
		sink << "#Time" << TAB << "#EncounterAtSink" << endl;
	}

	int nEncounterAtSink = 0;
	//���ؽ�����node����x��������
	vector<CNode *> nodes = CNode::getNodes();
	nodes = CPreprocessor::mergeSort(nodes);

	//�жϹ���״̬����sinkͶ�����ݣ��ڵ��ͨ��
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	{
		//�����������״̬��ֱ������
		if( ! (*inode)->isListening() )
			continue;

		if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= TRANS_RANGE )
		{
			//deliver data to sink
			cout << CR ;			
			cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )                " ;
			CSink::getSink()->receiveData( (*inode)->sendAllData(false), currentTime );
			(*inode)->updateDeliveryPredsWithSink();
			nEncounterAtSink++;
		}

		//scan other nodes and forward data
		//inode < jnode�����κνڵ��ֻ��һ��ͨ�Ż���
		for(vector<CNode *>::iterator jnode = inode + 1; jnode != nodes.end(); jnode++)
		{
			if( (*jnode)->getX() + TRANS_RANGE < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + TRANS_RANGE < (*jnode)->getX() )
				break;
			if( CBasicEntity::getDistance( **inode, **jnode ) > TRANS_RANGE )
				continue;

			//init by node with smaller id
			CNode *smaller, *greater = NULL;
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
						fail = ! smaller->receiveData( data, currentTime) ;
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
							fail = ! greater->receiveData( data , currentTime) ;
					}
				}
				if( ! fail )
				{	
					if( skip )
					{
						cout << CR ;
						cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )          " ;
					}
					else
					{
						cout << CR ;
						cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communication )          " ;
					}
				}
				else
				{
					cout << CR ;
					cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
				}
			}
			else
			{
				cout << CR ;
				cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
			}
		}

	}

	//�������нڵ��buffer״̬��¼
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	{
		(*inode)->recordBufferStatus();
	}

	cout << CR ;
	//����̨���ʱ����һλС��
	double deliveryRatio = ROUND( CData::getDataArrivalCount() / (double)CData::getDataCount() * 1000 );
	deliveryRatio = deliveryRatio / (double)10;
	cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                  " << endl;
	sink << currentTime << TAB << nEncounterAtSink << endl;
	sink.close();

}

void Prophet::PrintInfo(int currentTime)
{
	//Energy Consumption���ڵ�buffer״̬ ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{

		//ƽ���ܺ�
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == 0)
		{
			energy_consumption << logInfo;
			energy_consumption << "#Time" << TAB << "#AvgEnergyConsumption" << endl;
		}
		energy_consumption << currentTime << TAB << CData::getAverageEnergyConsumption() / 1000 << endl;
		energy_consumption.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == 0)
		{
			buffer << logInfo;
			buffer << "#Time" << TAB << "#AvgBufferStateInHistoryOfEachNode" << endl;
		}
		buffer << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
			 buffer << (*inode)->getAverageBufferSize() << TAB;
		buffer << endl;
		buffer.close();

		//����Ͷ����-900������debug��
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay("delay.txt", ios::app);
		if(currentTime == 0)
		{
			delay << logInfo;
			delay << "#Time" << TAB << "#AvgDelay" << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

	}

	//����Ͷ���ʡ�����Ͷ��ʱ��
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream node("buffer-node.txt", ios::app);
		if(currentTime == 0)
		{
			node << logInfo;
			node << "#Time" << TAB << "#BufferStateOfEachNode" << endl;
		}
		node << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
			node << (*inode)->getBufferSize() << "  " ;
		node << endl;
		node.close();

	}

	////����debug���
	if( currentTime == RUNTIME )
	{
		debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << CData::getAverageEnergyConsumption() / 1000 << TAB ;
		//debugInfo << CData::getDeliveryAtHotspotPercent() << TAB ;
		debugInfo << logInfo.replace(0, 1, "");
		debugInfo.flush();
	}

}

bool Prophet::Operate(int currentTime)
{
	////Node Number Test:
	//if( TEST_DYNAMIC_NUM_NODE )
	//{
	//	if(currentTime % SLOT_CHANGE_NUM_NODE == 0 && currentTime > 0)
	//	{
	//		ChangeNodeNumber();
	//	}
	//}

	if( ! CNode::hasNodes(currentTime) )
		return false;

	if( currentTime % SLOT_MOBILITYMODEL == 0 )
	{
		cout << endl << "########  < " << currentTime << " >  NODE LOCATION UPDATE" ;
		UpdateNodeStatus(currentTime);
	}

	if( currentTime % SLOT_DATA_GENERATE == 0 && currentTime <= DATATIME )
	{
		cout << endl << "########  < " << currentTime << " >  DATA GENERATION" ;
		GenerateData(currentTime);
	}

	if( currentTime % SLOT_DATA_SEND == 0 )
	{
		cout << endl << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;
		SendData(currentTime);
	}

	if( currentTime % SLOT_RECORD_INFO == 0 )
		PrintInfo(currentTime);

	return true;
}