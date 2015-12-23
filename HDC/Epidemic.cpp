#include "Epidemic.h"
#include "Node.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Sink.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "HDC.h"

int Epidemic::MAX_QUEUE_SIZE = CNode::BUFFER_CAPACITY;
int Epidemic::SPOKEN_MEMORY = 0;

void Epidemic::UpdateNodeStatus(int currentTime)
{
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
	{
		(*inode)->updateStatus(currentTime);
	}
	//�����²�Э��HDC���ж��Ƿ�λ���ȵ����򣬸���ռ�ձ�
	if( DO_HDC )
		CHDC::UpdateDutyCycleForNodes(currentTime);
}

void Epidemic::GenerateData(int currentTime)
{
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
	{
		(*inode)->generateData(currentTime);
	}
}

void Epidemic::SendData(int currentTime)
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
			cout << endl << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )" << endl;
			CSink::getSink()->receiveData( (*inode)->deliverAllData(), currentTime );
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

			//if spoken recently, skip
			if( (*inode)->hasSpokenRecently( (*jnode), currentTime) )
			{
				cout << endl << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )" << endl;
				continue;
			}
			//init by node with smaller id
			CNode *smaller, *greater = NULL;
			smaller = (*inode)->getID() < (*jnode)->getID() ? *inode : *jnode ;
			greater = (*inode)->getID() > (*jnode)->getID() ? *inode : *jnode ;

			bool fail = false;
			bool skip = false;
			vector<int> sv;
			vector<int> req;

			//forward data
			
			sv = smaller->sendSummaryVector();

			if( sv.empty() )
				skip = true;
			else
			{
				sv = greater->receiveSummaryVector( sv );
				if( sv.empty() )
					fail = true;
				else
				{
					req = greater->sendRequestList( sv );
					if( req.empty() )
						skip = true;
					else
						fail = ! greater->receiveData(
							smaller->sendData(
							smaller->receiveRequestList( req ) ) , currentTime) ;
				}
			}
			if( ! fail )
			{
				sv = greater->sendSummaryVector();
				if( sv.empty() && skip )
					skip = true;
				else
				{
					skip = false;
					sv = smaller->receiveSummaryVector( sv );
					if( sv.empty() )
						fail = true;
					else
					{
						req = smaller->sendRequestList( sv );
						if( req.empty() )
							skip = true;
						else
							fail = ! smaller->receiveData(
								greater->sendData(
								greater->receiveRequestList( req ) ) , currentTime) ;
					}
				}
				if( ! fail )
				{
					smaller->addToSpokenCache(greater, currentTime);
					greater->addToSpokenCache(smaller, currentTime);			
					if( skip )
						cout << endl << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )" << endl;
					else
						cout << endl << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communication )" << endl;
				}
				else
					cout << endl << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )" << endl;
			}
			else
				cout << endl << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )" << endl;

		}

	}

	//�������нڵ��buffer״̬��¼
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	{
		(*inode)->recordBufferStatus();
	}

	cout << endl << "####  [ Delivery Ratio ]  " << CData::getDataArrivalCount() / (double)CData::getDataCount() * 100 << " % " << endl;
	sink << currentTime << TAB << nEncounterAtSink << endl;
	sink.close();

}

void Epidemic::PrintInfo(int currentTime)
{
	//Energy Consumption���ڵ�buffer״̬ ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0 )
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
		debugInfo << CNode::DEFAULT_DUTY_CYCLE << TAB << CNode::HOTSPOT_DUTY_CYCLE << TAB << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << CData::getAverageEnergyConsumption() / 1000 << TAB ;
		//debugInfo << CData::getDeliveryAtHotspotPercent() << TAB ;
		debugInfo << logInfo.replace(0, 1, "");
	}

}

bool Epidemic::Operate(int currentTime)
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
		cout << endl << "########  [ " << currentTime << " ]  NODE LOCATION UPDATE" << endl;
		UpdateNodeStatus(currentTime);
	}

	if( currentTime % SLOT_DATA_GENERATE == 0 && currentTime <= DATATIME )
	{
		cout << endl << "########  [ " << currentTime << " ]  DATA GENERATION" << endl;
		GenerateData(currentTime);
	}

	if( currentTime % SLOT_DATA_SEND == 0 )
	{
		cout << endl << "########  [ " << currentTime << " ]  DATA DELIVERY" << endl;
		SendData(currentTime);
	}

	if( currentTime % SLOT_RECORD_INFO == 0 )
		PrintInfo(currentTime);

	return true;
}