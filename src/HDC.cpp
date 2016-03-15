#include "HDC.h"
#include "HAR.h"
#include "SortHelper.h"

extern _MAC_PROTOCOL MAC_PROTOCOL;
extern string FILE_DEBUG;
extern int RUNTIME;


void CHDC::PrintInfo(int currentTime)
{
	CMacProtocol::PrintInfo(currentTime);

	if( ! ( ( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE == 0 
		      && currentTime >= CHotspot::TIME_HOSPOT_SELECT_START )
			|| currentTime == RUNTIME  ) )
		return;

	HAR::PrintHotspotInfo(currentTime);
}

void CHDC::PrintFinal(int currentTime)
{
	CMacProtocol::PrintFinal(currentTime);

	ofstream debug(FILE_DEBUG, ios::app);
	if( CRoutingProtocol::TEST_HOTSPOT_SIMILARITY )
		debug << HAR::getAverageSimilarityRatio() << TAB ;
	debug.close();
	
}

void CHDC::UpdateDutyCycleForNodes(int currentTime)
{
	if( ! ( currentTime % SLOT_MOBILITYMODEL == 0 ) )
		return;

	vector<CHotspot *> hotspots = CHotspot::selectedHotspots;
	if( hotspots.empty() )
		return;
	cout << "########  < " << currentTime << " >  DUTY CYCLE UPDATE" << endl ;

	int atHotspotCount = 0;
	vector<CNode *> nodes = CNode::getNodes();
	nodes = CSortHelper::mergeSort( nodes );
	hotspots = CSortHelper::mergeSort( hotspots, CSortHelper::ascendByLocationX );
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		CHotspot *atHotspot = nullptr;

		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
		{
			if( (*ihotspot)->getX() + CGeneralNode::TRANS_RANGE < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + CGeneralNode::TRANS_RANGE < (*ihotspot)->getX() )
				break;
			if( CBasicEntity::getDistance( **inode, **ihotspot ) <= CGeneralNode::TRANS_RANGE )		
			{
				atHotspot = *ihotspot;
				break;
			}
		}

		//update duty cycle
		if( (*inode)->isAtHotspot() && atHotspot == nullptr )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " leaves Hotspot " << (*inode)->getAtHotspot()->getID() << " )              " ;			
			(*inode)->setAtHotspot(nullptr);
			(*inode)->resetDutyCycle();
		}
		else if(  ( ! (*inode)->isAtHotspot() ) && atHotspot != nullptr )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " enters Hotspot " << atHotspot->getID() << " )               " ;
			(*inode)->setAtHotspot(atHotspot);
			(*inode)->raiseDutyCycle();
		}
		if( (*inode)->isAtHotspot() )
		{
			CNode::visitAtHotspot();
			++atHotspotCount;
		}
		else
			CNode::visitOnRoute();

	}

	//����̨���ʱ����һλС��
	double encounterRatio = NDigitFloat( CNode::getEncounterAtHotspotPercent() * 100, 1);
	flash_cout << "####  [ Hotspot Encounter ]  " << encounterRatio << " %                                           " << endl;
	//flash_cout << "####  [ At Hotspot ]  " << atHotspotCount << " / " << CNode::getNodes().size() << "                              " ;

}

bool CHDC::Operate(int currentTime)
{
	//Node Number Test:
	if( TEST_DYNAMIC_NUM_NODE )
		CMacProtocol::ChangeNodeNumber(currentTime);

	//TODO: move update of state after update of dc
	CMacProtocol::UpdateNodeStatus(currentTime);

	HAR::HotspotSelection(currentTime);

	//�ж��Ƿ�λ���ȵ����򣬸���ռ�ձ�
	if( MAC_PROTOCOL == _hdc )
		UpdateDutyCycleForNodes(currentTime);

	return true;
}