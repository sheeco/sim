#include "RoutingProtocol.h"
#include "Node.h"
#include "HAR.h"

extern int RUNTIME;
extern int DATATIME;
extern string INFO_LOG;
extern ofstream debugInfo;
extern _MacProtocol MAC_PROTOCOL;
extern _RoutingProtocol ROUTING_PROTOCOL;
extern _HotspotSelect HOTSPOT_SELECT;

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

	//cout << endl << "########  < " << currentTime << " >  NODE LOCATION UPDATE" ;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		(*inode)->updateStatus(currentTime);
}

//void CRoutingProtocol::GenerateData(int currentTime)
//{
//	if( ! ( currentTime % SLOT_DATA_GENERATE == 0 && currentTime <= DATATIME ) )
//		return;
//
//	//cout << endl << "########  < " << currentTime << " >  DATA GENERATION" ;
//	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
//		(*inode)->generateData(currentTime);
//}


string INFO_ENERGY_CONSUMPTION = "#Time	#AvgEnergyConsumption (#NodeCount	#CurrentEnergy...) \n" ;
string INFO_BUFFER_STATISTICS =  "#Time	#AvgBufferStateInHistoryOfEachNode \n" ;
string INFO_DELIVERY_RATIO = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio \n" ;
string INFO_DELAY = "#Time	#AvgDelay \n" ;
string INFO_BUFFER = "#Time	#BufferStateOfEachNode \n" ;
string INFO_ENCOUNTER = "#Time	(#EncounterAtHotspot)	#Encounter	(#EncounterPercentAtHotspot) \n" ;
string INFO_SINK = "#Time	#EncounterAtSink \n" ;

void CRoutingProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_RECORD_INFO == 0
			|| currentTime == RUNTIME ) )
		return;

	//Ͷ���ʡ��ӳ١�Energy Consumption���ڵ�buffer״̬ͳ�� ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
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

		//MA�ͽڵ� / �ڵ�����������
		ofstream encounter("encounter.txt", ios::app);
		if(currentTime == 0)
		{
			encounter << INFO_LOG ;
			encounter << INFO_ENCOUNTER ;
		}
		encounter << currentTime << TAB;
		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
			encounter << CNode::getEncounterAtHotspot() << TAB;
		encounter << CNode::getEncounter() << TAB;
		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
			encounter << CNode::getEncounterAtHotspotPercent();
		encounter << endl;
		encounter.close();

		//ƽ���ܺ�
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == 0)
		{
			energy_consumption << INFO_LOG ; 
			energy_consumption << INFO_ENERGY_CONSUMPTION ;
		}
		energy_consumption << currentTime << TAB << CData::getAverageEnergyConsumption() ;
		if( CNode::finiteEnergy() )
		{			
			//�ڵ�ʣ������
			energy_consumption << TAB << CNode::getNodes().size() << TAB;
			vector<CNode *> allNodes = CNode::getAllNodes();
			for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
				energy_consumption << (*inode)->getEnergy() << TAB;
		}
		energy_consumption << endl;
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

	}

	//����Ͷ���ʡ��ڵ�buffer״̬
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG;
			delivery_ratio << INFO_DELIVERY_RATIO ;
		}
		delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

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
		debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << CData::getAverageEnergyConsumption() << TAB ;
		if( CNode::finiteEnergy() )
			debugInfo << currentTime << TAB;
		if( MAC_PROTOCOL == _hdc )
		{
			if( HOTSPOT_SELECT == _merge )
				debugInfo << HAR::getAverageMergePercent() << TAB << HAR::getAverageOldPercent() << TAB ;
			if( TEST_HOTSPOT_SIMILARITY )
				debugInfo << HAR::getAverageSimilarityRatio() << TAB ;
			debugInfo << CNode::getEncounterAtHotspotPercent() << TAB ;
		}
		debugInfo.flush();
	}

}
