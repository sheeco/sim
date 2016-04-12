#include "Global.h"
#include "HDC.h"
#include "HAR.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "CTrace.h"


CHDC::CHDC()
{
	
}

CHDC::~CHDC()
{
	
}

void CHDC::UpdateDutyCycleForNodes(int currentTime)
{
	if( ! ( currentTime % CCTrace::SLOT_TRACE == 0 ) )
		return;

	vector<CHotspot *> hotspots = CHotspot::selectedHotspots;
	vector<CNode *> nodes = CNode::getNodes();
	if( hotspots.empty()
		|| nodes.empty() )
		return;

	static bool print = false;
	if( currentTime == 0 
		|| print )
	{
		flash_cout << "########  < " << currentTime << " >  DUTY CYCLE UPDATE            " << endl ;
		print = false;
	}

	CHotspot::UpdateAtHotspotForNodes(currentTime);

//	int atHotspotCount = 0;
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		//update duty cycle
		if( (*inode)->useHotspotDutyCycle()
			&& ( ! (*inode)->isAtHotspot() ) )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " leaves Hotspot )              " ;			
			(*inode)->resetDutyCycle();
		}
		else if( (*inode)->useDefaultDutyCycle()
				 && (*inode)->isAtHotspot() )
		{
			flash_cout << "####  ( Node " << (*inode)->getID() << " enters Hotspot )               " ;
			(*inode)->raiseDutyCycle();
		}
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

	if( ! ( ( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0 
		      && currentTime >= CHotspotSelect::STARTTIME_HOSPOT_SELECT )
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

	if( ! CMacProtocol::UpdateNodeStatus(currentTime) )
		return false;

	CHotspotSelect::CollectNewPositions(currentTime);

	CHotspotSelect::HotspotSelect(currentTime);

	UpdateDutyCycleForNodes(currentTime);

	CMacProtocol::CommunicateWithNeighbor(currentTime);

	return true;
}

