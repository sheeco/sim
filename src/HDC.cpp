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

void CHDC::UpdateDutyCycleForNodes(int now)
{
	if( ! ( now % getConfig<int>("trace", "interval") == 0 ) )
		return;

	vector<CHotspot *> hotspots = CHotspot::getSelectedHotspots();
	vector<CNode *> nodes = CNode::getNodes();
	if( hotspots.empty()
		|| nodes.empty() )
		return;

	static bool print = false;
	if( now == 0 
		|| print )
	{
		CPrintHelper::PrintHeading(now, "DUTY CYCLE UPDATE");
		print = false;
	}

	int atHotspotCount = 0;
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		//update duty cycle
		if( (*inode)->useHotspotDutyCycle()
			&& ( ! (*inode)->isAtWaypoint() ) )
		{
			CPrintHelper::FlashDetail(now, (*inode)->format() + " leaves hotspot");
			(*inode)->resetDutyCycle();
		}
		else if( (*inode)->useDefaultDutyCycle()
				 && (*inode)->isAtWaypoint() )
		{
			CPrintHelper::FlashDetail(now, ( *inode )->format() + " enters " + ( *inode )->getAtHotspot()->format());
			(*inode)->raiseDutyCycle();
		}
	}

	//控制台输出时保留一位小数
	if( ( now + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0 )
	{
		CPrintHelper::PrintPercentage("Hotspot Encounter", CNode::getPercentEncounterAtWaypoint());
		print = true;
	}
	CPrintHelper::PrintPercentage("Hot-Node", atHotspotCount);

}

void CHDC::PrintInfo(int now)
{
	CMacProtocol::PrintInfo(now);

	if( ! ( ( now % getConfig<int>("hs", "slot_hotspot_update") == 0 
		      && now >= getConfig<int>("hs", "starttime_hospot_select") )
			|| now == getConfig<int>("simulation", "runtime")  ) )
		return;

	CHotspotSelect::PrintInfo(now);
}

void CHDC::PrintFinal(int now)
{
	CMacProtocol::PrintFinal(now);

	CHotspotSelect::PrintFinal(now);
	
}

bool CHDC::Prepare(int now)
{
	if( !CMacProtocol::Prepare(now) )
		return false;

	UpdateDutyCycleForNodes(now);

	return true;
}

bool CHDC::Operate(int now)
{
	CMacProtocol::CommunicateWithNeighbor(now);

	return true;
}

