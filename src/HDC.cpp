#include "Global.h"
#include "HDC.h"
#include "HAR.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "Trace.h"

double CHDC::HOTSPOT_DUTY_CYCLE = 0;


CHDC::CHDC()
{
	
}

CHDC::~CHDC()
{
	
}

void CHDC::raiseDutyCycle(CDutyCycle* node)
{
	if( node->ifUseDutyCycle(HOTSPOT_DUTY_CYCLE) )
		return;

	node->setDutyCycle(HOTSPOT_DUTY_CYCLE);
}

void CHDC::resetDutyCycle(CDutyCycle* node)
{
	if( node->ifUseDutyCycle(CDutyCycle::DEFAULT_DUTY_CYCLE) )
		return;

	node->setDutyCycle(CDutyCycle::DEFAULT_DUTY_CYCLE);
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
		flash_cout << "####  < " << currentTime << " >  DUTY CYCLE UPDATE            " << endl ;
		print = false;
	}

//	int atHotspotCount = 0;
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		//update duty cycle
		if( (*inode)->ifUseDutyCycle(HOTSPOT_DUTY_CYCLE)
			&& ( ! (*inode)->isAtHotspot() ) )
		{
			flash_cout << "######  ( Node " << (*inode)->getID() << " leaves Hotspot )              " ;			
			resetDutyCycle(*inode);
		}
		else if( (*inode)->ifUseDutyCycle(CDutyCycle::DEFAULT_DUTY_CYCLE)
				 && (*inode)->isAtHotspot() )
		{
			flash_cout << "######  ( Node " << (*inode)->getID() << " enters Hotspot )               " ;
			raiseDutyCycle(*inode);
		}
	}

	//控制台输出时保留一位小数
	if( ( currentTime + SLOT ) % SLOT_LOG == 0 )
	{
		double encounterRatio = NDigitFloat( CNode::getPercentEncounterAtHotspot() * 100, 1);
		flash_cout << "######  [ Hotspot Encounter ]  " << encounterRatio << " %                                           " << endl << endl;
		print = true;
	}
	//flash_cout << "######  [ At Hotspot ]  " << atHotspotCount << " / " << CNode::getNodes().size() << "                              " ;

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

bool CHDC::Prepare(int currentTime)
{
	if( !CMacProtocol::Prepare(currentTime) )
		return false;

	UpdateDutyCycleForNodes(currentTime);

	return true;
}

bool CHDC::Operate(int currentTime)
{
	CMacProtocol::CommunicateWithNeighbor(currentTime);

	return true;
}

