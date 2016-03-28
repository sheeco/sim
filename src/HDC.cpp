#include "Global.h"
#include "HDC.h"
#include "HAR.h"
#include "SortHelper.h"
#include "HotspotSelect.h"

CHDC::CHDC()
{
	
}

CHDC::~CHDC()
{
	
}

void CHDC::UpdateDutyCycleForNodes(int currentTime)
{
	if( ! ( currentTime % SLOT_MOBILITYMODEL == 0 ) )
		return;

	vector<CHotspot *> hotspots = CHotspot::selectedHotspots;
	if( hotspots.empty() )
		return;

	static bool print = false;
	if( currentTime == 0 
		|| print )
	{
		flash_cout << "########  < " << currentTime << " >  DUTY CYCLE UPDATE            " << endl ;
		print = false;
	}

	int atHotspotCount = 0;
	vector<CNode *> nodes = CNode::getNodes();
	nodes = CSortHelper::mergeSort( nodes );
	hotspots = CSortHelper::mergeSort( hotspots, CSortHelper::ascendByLocationX );
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		CHotspot *atHotspot = nullptr;

		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
		{
			if( (*ihotspot)->getX() + CGeneralNode::RANGE_TRANS < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + CGeneralNode::RANGE_TRANS < (*ihotspot)->getX() )
				break;
			if( CBasicEntity::withinRange( **inode, **ihotspot, CGeneralNode::RANGE_TRANS ) )		
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
	if( ( currentTime + SLOT ) % SLOT_LOG == 0 )
	{
		double encounterRatio = NDigitFloat( CNode::getPercentEncounterAtHotspot() * 100, 1);
		flash_cout << "####  [ Hotspot Encounter ]  " << encounterRatio << " %                                           " << endl << endl;
		print = true;
	}
	//flash_cout << "####  [ At Hotspot ]  " << atHotspotCount << " / " << CNode::getNodes().size() << "                              " ;

}

void CHDC::PrintInfo(int currentTime)
{
	CMacProtocol::PrintInfo(currentTime);

	if( ! ( ( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE == 0 
		      && currentTime >= CHotspot::TIME_HOSPOT_SELECT_START )
			|| currentTime == RUNTIME  ) )
		return;

	CHotspotSelect::PrintInfo(currentTime);
}

void CHDC::PrintFinal(int currentTime)
{
	CMacProtocol::PrintFinal(currentTime);

	CHotspotSelect::PrintFinal(currentTime);
	
}

bool CHDC::Operate(int currentTime)
{
	//Node Number Test:
	if( TEST_DYNAMIC_NUM_NODE )
		CMacProtocol::ChangeNodeNumber(currentTime);

	HAR::HotspotSelection(currentTime);

	UpdateDutyCycleForNodes(currentTime);

	CommunicateWithNeighbor(currentTime);

	return true;
}

