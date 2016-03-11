#include "RoutingProtocol.h"
#include "Node.h"
#include "HAR.h"
#include "HDC.h"

extern _MAC_PROTOCOL MAC_PROTOCOL;
extern _ROUTING_PROTOCOL ROUTING_PROTOCOL;
extern _HOTSPOT_SELECT HOTSPOT_SELECT;
extern string INFO_LOG;
extern string FILE_DEBUG;
extern int RUNTIME;

int CRoutingProtocol::SLOT_DATA_SEND = SLOT_MOBILITYMODEL;  //数据发送slot
bool CRoutingProtocol::TEST_HOTSPOT_SIMILARITY = false;


//CRoutingProtocol::CRoutingProtocol()
//{
//}
//
//CRoutingProtocol::~CRoutingProtocol()
//{
//}

string INFO_BUFFER_STATISTICS =  "#Time	#AvgBufferStateInHistoryOfEachNode \n" ;
string INFO_DELIVERY_RATIO = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% \n" ;
string INFO_DELAY = "#Time	#AvgDelay \n" ;
string INFO_BUFFER = "#Time	#BufferStateOfEachNode \n" ;

void CRoutingProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_RECORD_INFO == 0
			|| currentTime == RUNTIME ) )
		return;

	if( MAC_PROTOCOL == _hdc )
		CHDC::PrintInfo(currentTime);
//	else
//		CSMAC::PrintInfo(currentTime);

	//投递率、延迟、节点buffer状态统计 ...
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

//		//MA和节点 / 节点间的相遇次数
//		ofstream encounter("encounter.txt", ios::app);
//		if(currentTime == 0)
//		{
//			encounter << INFO_LOG ;
//			encounter << INFO_ENCOUNTER ;
//		}
//		encounter << currentTime << TAB;
//		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
//			encounter << CNode::getEncounterAtHotspotPercent() << TAB << CNode::getEncounterAtHotspot() << TAB 
//					  << CNode::getEncounterActivePercent() << TAB << CNode::getEncounterActive << TAB;
//		encounter << CNode::getEncounter() << TAB;
//		encounter << endl;
//		encounter.close();

//		//平均能耗
//		ofstream energy_consumption("energy-consumption.txt", ios::app);
//		if(currentTime == 0)
//		{
//			energy_consumption << INFO_LOG ; 
//			energy_consumption << INFO_ENERGY_CONSUMPTION ;
//		}
//		energy_consumption << currentTime << TAB << CData::getAverageEnergyConsumption() ;
//		if( CNode::finiteEnergy() )
//		{			
//			//节点剩余能量
//			energy_consumption << TAB << CNode::getSumEnergyConsumption() << TAB << CNode::getNodes().size() << TAB;
//			vector<CNode *> allNodes = CNode::getAllNodes(true);
//			for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
//				energy_consumption << (*inode)->getEnergy() << TAB;
//		}
//		energy_consumption << endl;
//		energy_consumption.close();

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

}

void CRoutingProtocol::PrintFinal(int currentTime)
{
	ofstream debug(FILE_DEBUG, ios::app);
	if( CNode::finiteEnergy() )
		debug << CData::getDeliveryCount() << TAB ;
	else
		debug << CData::getDeliveryRatio() << TAB ;
	debug << CData::getAverageDelay() << TAB ;

	if( MAC_PROTOCOL == _hdc && ROUTING_PROTOCOL != _har )
		CHDC::PrintFinal(currentTime);
//	else
//		CSMAC::PrintFinal(currentTime);

	debug.close();

}