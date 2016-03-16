#include "MacProtocol.h"
#include "Node.h"

//extern _MAC_PROTOCOL MAC_PROTOCOL;
//extern _ROUTING_PROTOCOL ROUTING_PROTOCOL;
//extern string INFO_LOG;
//extern string FILE_DEBUG;
//extern int RUNTIME;

int CMacProtocol::MAC_SIZE = 0;  //Mac Header Size
bool CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
int CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //��̬�ڵ��������ʱ���ڵ���������仯������


//CMacProtocol::CMacProtocol()
//{
//}
//
//CMacProtocol::~CMacProtocol()
//{
//}

void CMacProtocol::ChangeNodeNumber(int currentTime)
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

void CMacProtocol::UpdateNodeStatus(int currentTime)
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

void CMacProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_RECORD_INFO == 0
			|| currentTime == RUNTIME ) )
		return;

	//Ͷ���ʡ��ӳ١�Energy Consumption���ڵ�buffer״̬ͳ�� ...
	if( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//MA�ͽڵ� / �ڵ�����������
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
			energy_consumption << TAB << CNode::getSumEnergyConsumption() << TAB << CNode::getNodes().size() << TAB;
			vector<CNode *> allNodes = CNode::getAllNodes(true);
			for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
				energy_consumption << (*inode)->getEnergy() << TAB;
		}
		energy_consumption << endl;
		energy_consumption.close();

	}

}

void CMacProtocol::PrintFinal(int currentTime)
{
	ofstream debug(FILE_DEBUG, ios::app);
	debug << CData::getAverageEnergyConsumption() << TAB << CNode::getEncounterActivePercent() << TAB ;
	if( MAC_PROTOCOL == _hdc )
	{
		debug << CNode::getEncounterAtHotspotPercent() << TAB ;
	}
	if( CNode::finiteEnergy() )
		debug << currentTime << TAB << CNode::getNodes().size() << TAB ;

	debug.close();

}