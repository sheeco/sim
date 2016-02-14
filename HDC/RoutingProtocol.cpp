#include "RoutingProtocol.h"
#include "Node.h"
#include "HAR.h"

extern int RUNTIME;
extern int DATATIME;
extern string INFO_LOG;
extern ofstream debugInfo;
extern MacProtocol MAC_PROTOCOL;
extern RoutingProtocol ROUTING_PROTOCOL;
extern HotspotSelect HOTSPOT_SELECT;

//CRoutingProtocol::CRoutingProtocol(void)
//{
//}
//
//
//CRoutingProtocol::~CRoutingProtocol(void)
//{
//}

void CRoutingProtocol::ChangeNodeNumber(int currentTime)
{
	if( ! ( currentTime % SLOT_CHANGE_NUM_NODE == 0 ) )
		return;

	cout << endl << "########  < " << currentTime << " >  NODE NUMBER CHANGE" ;
	float bet = RandomFloat(-1, 1);
	if(bet > 0)
		bet = 0.2 + bet / 2;
	else
		bet = -0.2 + bet / 2;
	int delta = ROUND( bet * (NUM_NODE_MAX - NUM_NODE_MIN) );
	if(delta == 0)
		return ;
	if(delta < NUM_NODE_MIN - NUM_NODE)
		delta = NUM_NODE_MIN - NUM_NODE;
	else if(delta > NUM_NODE_MAX - NUM_NODE)
		delta = NUM_NODE_MAX - NUM_NODE;

	if(delta > 0)
	{
		CNode::newNodes(delta);
		cout << "####  ( " << delta << " nodes added )" << endl;
	}
	else
	{
		CNode::removeNodes(delta);
		cout << "####  ( " << -delta << " nodes removed )" << endl;
	}
}

void CRoutingProtocol::UpdateNodeStatus(int currentTime)
{
	if( ! ( currentTime % SLOT_MOBILITYMODEL == 0 ) )
		return;

	cout << endl << "########  < " << currentTime << " >  NODE LOCATION UPDATE" ;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		(*inode)->updateStatus(currentTime);
}

void CRoutingProtocol::GenerateData(int currentTime)
{
	if( ! ( currentTime % SLOT_DATA_GENERATE == 0 && currentTime <= DATATIME ) )
		return;

	//cout << endl << "########  < " << currentTime << " >  DATA GENERATION" ;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		(*inode)->generateData(currentTime);
}


string INFO_ENERGY_CONSUMPTION = "#Time	#AvgEnergyConsumption \n" ;
string INFO_BUFFER_STATISTICS =  "#Time	#AvgBufferStateInHistoryOfEachNode \n" ;
string INFO_DELIVERY_RATIO = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio \n" ;
string INFO_DELAY = "#Time	#AvgDelay \n" ;
string INFO_BUFFER = "#Time	#BufferStateOfEachNode \n" ;
string INFO_HOTSPOT = "#Time	#HotspotCount \n" ;
string INFO_ENCOUNTER = "#Time	#EncounterAtHotspot	#Encounter	#EncounterPercentAtHotspot \n" ;
string INFO_SINK = "#Time	#EncounterAtSink \n" ;

void CRoutingProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_RECORD_INFO == 0 ) )
		return;

	//Energy Consumption���ڵ�buffer״̬ ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{

		//ƽ���ܺ�
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == 0)
		{
			energy_consumption << INFO_LOG ; 
			energy_consumption << INFO_ENERGY_CONSUMPTION ;
		}
		energy_consumption << currentTime << TAB << CData::getAverageEnergyConsumption() / 1000 << endl;
		energy_consumption.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == 0)
		{
			buffer << INFO_LOG ;
			buffer << INFO_BUFFER_STATISTICS ;
		}
		buffer << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
			 buffer << (*inode)->getAverageBufferSize() << TAB;
		buffer << endl;
		buffer.close();

		//����Ͷ����-900������debug��
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG ;
			delivery_ratio << INFO_DELIVERY_RATIO ;
		}
		delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay("delay.txt", ios::app);
		if(currentTime == 0)
		{
			delay << INFO_LOG ;
			delay << INFO_DELAY ;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
		{
			//�ȵ����
			ofstream hotspot("hotspot.txt", ios::app);
			if(currentTime == startTimeForHotspotSelection)
			{
				hotspot << INFO_LOG ;
				hotspot << INFO_HOTSPOT ;
			}
			hotspot << currentTime << TAB << CHotspot::selectedHotspots.size() << endl; 
			hotspot.close();

			//MA�ͽڵ����������ͳ����Ϣ
			ofstream encounter("encounter.txt", ios::app);
			if(currentTime == startTimeForHotspotSelection)
			{
				encounter << INFO_LOG ;
				encounter << INFO_ENCOUNTER ;
			}
			encounter << currentTime << TAB << CNode::getEncounterAtHotspot() << TAB << CNode::getEncounter() << TAB << CNode::getEncounterPercentAtHotspot() << endl;
			encounter.close();

		}
	}

	//����Ͷ���ʡ�����Ͷ��ʱ��
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		////����Ͷ����-100�����ڻ������ߣ�
		//ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		//if(currentTime == 0)
		//{
		//	delivery_ratio << INFO_LOG;
		//	delivery_ratio << INFO_DELIVERY_RATIO ;
		//}
		//delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		//delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream node("buffer-node.txt", ios::app);
		if(currentTime == 0)
		{
			node << INFO_LOG ;
			node << INFO_BUFFER ;
		}
		node << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
			node << (*inode)->getBufferSize() << "  " ;
		node << endl;
		node.close();

	}

	////����debug���
	if( currentTime == RUNTIME )
	{
		debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << CData::getAverageEnergyConsumption() / 1000 << TAB ;
		if( MAC_PROTOCOL == _hdc)
		{
			if( HOTSPOT_SELECT == _merge )
				debugInfo << HAR::getAverageHotspotCost() << TAB << HAR::getAverageMergePercent() << TAB << HAR::getAverageOldPercent() << TAB ;
			if(TEST_HOTSPOT_SIMILARITY)
				debugInfo << HAR::getAverageSimilarityRatio() << TAB ;
			debugInfo << CData::getDeliveryAtHotspotPercent() << TAB ;
		}
		debugInfo.flush();
	}

}
