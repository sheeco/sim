#include "Global.h"
#include "Configuration.h"
#include "HDC.h"
#include "HAR.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "PrintHelper.h"


CHDC::CHDC()
{
	
}

CHDC::~CHDC()
{
	
}

void CHDC::UpdateDutyCycleForNodes(int currentTime)
{
	if( ! ( currentTime % getConfig<int>("trace", "interval") == 0 ) )
		return;

	vector<CHotspot *> hotspots = CHotspot::getSelectedHotspots();
	vector<CNode *> nodes = CNode::getNodes();
	if( hotspots.empty()
		|| nodes.empty() )
		return;

	static bool print = false;
	if( currentTime == 0 
		|| print )
	{
		CPrintHelper::PrintHeading(currentTime, "DUTY CYCLE UPDATE");
		print = false;
	}

	int atHotspotCount = 0;
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		//update duty cycle
		if( (*inode)->useHotspotDutyCycle()
			&& ( ! (*inode)->isAtWaypoint() ) )
		{
			CPrintHelper::FlashDetail(currentTime, (*inode)->format() + " leaves hotspot");
			(*inode)->resetDutyCycle();
		}
		else if( (*inode)->useDefaultDutyCycle()
				 && (*inode)->isAtWaypoint() )
		{
			CPrintHelper::FlashDetail(currentTime, ( *inode )->format() + " enters " + ( *inode )->getAtHotspot()->format());
			(*inode)->raiseDutyCycle();
		}
	}

	//控制台输出时保留一位小数
	if( ( currentTime + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0 )
	{
		CPrintHelper::PrintPercentage("Hotspot Encounter", CNode::getPercentEncounterAtWaypoint());
		print = true;
	}
	CPrintHelper::PrintPercentage("Hot-Node", atHotspotCount);

}

void CHDC::PrintInfo(int currentTime)
{
	CMacProtocol::PrintInfo(currentTime);

	if( ! ( ( currentTime % getConfig<int>("hs", "slot_hotspot_update") == 0 
		      && currentTime >= getConfig<int>("hs", "starttime_hospot_select") )
			|| currentTime == getConfig<int>("simulation", "runtime")  ) )
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

