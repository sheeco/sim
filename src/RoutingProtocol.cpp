#include "RoutingProtocol.h"
#include "Node.h"
#include "HAR.h"

extern int RUNTIME;
extern int DATATIME;
extern string INFO_LOG;
extern ofstream debugInfo;
extern _MAC_PROTOCOL MAC_PROTOCOL;
extern _ROUTING_PROTOCOL ROUTING_PROTOCOL;
extern _HOTSPOT_SELECT HOTSPOT_SELECT;

int CRoutingProtocol::SLOT_DATA_SEND = SLOT_MOBILITYMODEL;  //数据发送slot
bool CRoutingProtocol::TEST_HOTSPOT_SIMILARITY = false;
bool CRoutingProtocol::TEST_DYNAMIC_NUM_NODE = false;
int CRoutingProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //动态节点个数测试时，节点个数发生变化的周期


//CRoutingProtocol::CRoutingProtocol()
//{
//}
//
//CRoutingProtocol::~CRoutingProtocol()
//{
//}

void CRoutingProtocol::ChangeNodeNumber(int currentTime)
{
	if( ! ( currentTime % SLOT_CHANGE_NUM_NODE == 0 ) )
		return;

	cout << endl << "########  < " << currentTime << " >  NODE NUMBER CHANGE" ;
	
	int delta = CNode::ChangeNodeNumber();

	if(delta >= 0)
	{
		cout << "####  ( " << delta << " nodes added )" << endl;
	}
	else
	{
		cout << "####  ( " << -delta << " nodes removed )" << endl;
	}
}

void CRoutingProtocol::UpdateNodeStatus(int currentTime)
{
	if( ! ( currentTime % SLOT_MOBILITYMODEL == 0 ) )
		return;

	//cout << endl << "########  < " << currentTime << " >  NODE LOCATION UPDATE" ;
	bool allAlive = true;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		allAlive &= (*inode)->updateStatus(currentTime);
	if( ! allAlive )
		CNode::ClearDeadNodes();
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


string INFO_ENERGY_CONSUMPTION = "#Time	#AvgEC	(#SumEC	#NodeCount	#CurrentEnergy...) \n" ;
string INFO_BUFFER_STATISTICS =  "#Time	#AvgBufferStateInHistoryOfEachNode \n" ;
string INFO_DELIVERY_RATIO = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% \n" ;
string INFO_DELAY = "#Time	#AvgDelay \n" ;
string INFO_BUFFER = "#Time	#BufferStateOfEachNode \n" ;
string INFO_ENCOUNTER = "#Time	(#EncounterAtHotspot%	#EncounterAtHotspot	#EncounterActive%	#EncounterActive)	#Encounter	 \n" ;
string INFO_SINK = "#Time	#EncounterAtSink \n" ;

void CRoutingProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_RECORD_INFO == 0
			|| currentTime == RUNTIME ) )
		return;

	//投递率、延迟、Energy Consumption、节点buffer状态统计 ...
	if( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//数据投递率-900（用于debug）
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG ;
			delivery_ratio << INFO_DELIVERY_RATIO ;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//数据投递延迟
		ofstream delay("delay.txt", ios::app);
		if(currentTime == 0)
		{
			delay << INFO_LOG ;
			delay << INFO_DELAY ;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//MA和节点 / 节点间的相遇次数
		ofstream encounter("encounter.txt", ios::app);
		if(currentTime == 0)
		{
			encounter << INFO_LOG ;
			encounter << INFO_ENCOUNTER ;
		}
		encounter << currentTime << TAB;
		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
			encounter << CNode::getEncounterAtHotspotPercent() << TAB << CNode::getEncounterAtHotspot() << TAB 
					  << CNode::getEncounterActivePercent() << TAB << CNode::getEncounterActive << TAB;
		encounter << CNode::getEncounter() << TAB;
		encounter << endl;
		encounter.close();

		//平均能耗
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == 0)
		{
			energy_consumption << INFO_LOG ; 
			energy_consumption << INFO_ENERGY_CONSUMPTION ;
		}
		energy_consumption << currentTime << TAB << CData::getAverageEnergyConsumption() ;
		if( CNode::finiteEnergy() )
		{			
			//节点剩余能量
			energy_consumption << TAB << CNode::getSumEnergyConsumption() << TAB << CNode::getNodes().size() << TAB;
			vector<CNode *> allNodes = CNode::getAllNodes(true);
			for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
				energy_consumption << (*inode)->getEnergy() << TAB;
		}
		energy_consumption << endl;
		energy_consumption.close();

		//每个节点buffer状态的历史平均值
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == 0)
		{
			buffer << INFO_LOG ;
			buffer << INFO_BUFFER_STATISTICS ;
		}
		buffer << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << (*inode)->getAverageBufferSize() << TAB;
		}
		buffer << endl;
		buffer.close();

	}

	//数据投递率、节点buffer状态
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//数据投递率-100（用于绘制曲线）
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG;
			delivery_ratio << INFO_DELIVERY_RATIO ;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//每个节点的当前buffer状态
		ofstream buffer("buffer-node.txt", ios::app);
		if(currentTime == 0)
		{
			buffer << INFO_LOG ;
			buffer << INFO_BUFFER ;
		}
		buffer << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << (*inode)->getBufferSize() << "  " ;
		}
		buffer << endl;
		buffer.close();

	}

	PrintFinal(currentTime);

}

void CRoutingProtocol::PrintFinal(int currentTime)
{
	if( currentTime != RUNTIME )
		return;

	////最终debug输出
	if( CNode::finiteEnergy() )
		debugInfo << CData::getDeliveryCount() << TAB ;
	else
		debugInfo << CData::getDeliveryRatio() << TAB ;
	debugInfo << CData::getAverageDelay() << TAB << CData::getAverageEnergyConsumption() << TAB ;
	if( CNode::finiteEnergy() )
		debugInfo << currentTime << TAB << CNode::getNodes().size() << TAB ;
	if( MAC_PROTOCOL == _hdc )
	{
		if( HOTSPOT_SELECT == _merge )
			debugInfo << HAR::getAverageMergePercent() << TAB << HAR::getAverageOldPercent() << TAB ;
		if( TEST_HOTSPOT_SIMILARITY )
			debugInfo << HAR::getAverageSimilarityRatio() << TAB ;
		debugInfo << CNode::getEncounterActivePercent() << CNode::getEncounterAtHotspotPercent() << TAB ;
	}
	if( ROUTING_PROTOCOL != _har )
		debugInfo << INFO_LOG.replace(0, 1, "");
	debugInfo.flush();

}