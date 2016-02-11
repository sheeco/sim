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

void Epidemic::SendData(int currentTime)
{
	if( ! currentTime % SLOT_DATA_SEND == 0 )
		return;
	cout << endl << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;

	ofstream sink("sink.txt", ios::app);
	if(currentTime == 0)
	{
		sink << INFO_LOG;
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
			flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink )                    " ;
			CSink::getSink()->receiveData( (*inode)->sendAllData(false), currentTime );
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
				flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )            " ;
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
							smaller->sendDataByRequestList(
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
								greater->sendDataByRequestList(
								greater->receiveRequestList( req ) ) , currentTime) ;
					}
				}
				if( ! fail )
				{
					smaller->addToSpokenCache(greater, currentTime);
					greater->addToSpokenCache(smaller, currentTime);			
					if( skip )
						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )            " ;
					else
						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communication )            " ;
				}
				else
					flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )            " ;
			}
			else
				flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )            " ;

		}

	}

	//�������нڵ��buffer״̬��¼
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	{
		(*inode)->recordBufferStatus();
	}

	//����̨���ʱ����һλС��
	double deliveryRatio = ROUND( CData::getDataArrivalCount() / (double)CData::getDataCount() * 1000 );
	deliveryRatio = deliveryRatio / (double)10;
	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                    " << endl;
	sink << currentTime << TAB << nEncounterAtSink << endl;
	sink.close();

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

	UpdateNodeStatus(currentTime);

	GenerateData(currentTime);

	SendData(currentTime);

	PrintInfo(currentTime);

	return true;
}