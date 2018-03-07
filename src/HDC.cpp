#include "Global.h"
#include "Configuration.h"
#include "HDC.h"
#include "HAR.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "PrintHelper.h"

double CHDC::HOTSPOT_DUTY_RATE = 0;

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

	vector<CHotspot *> hotspots = CHotspotSelect::getSelectedHotspots();
	vector<CNode *> nodes = CNode::getAllNodes();
	if( hotspots.empty()
		|| nodes.empty() )
		return;

	CPrintHelper::PrintHeading(now, "DUTY CYCLE UPDATE");

	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		bool usingHotspotDutyCycle = isUsingHotspotDutyCycle( *inode );
		bool atHotspot = CHotspot::isAtHotspot(( *inode )->getID());
		//update duty cycle
		if( usingHotspotDutyCycle
			&& !atHotspot  )
		{
			(*inode)->resetDutyCycle();
			CPrintHelper::PrintDetail(now, "Duty cycle of " + ( *inode )->getName() + " is reset.");
		}
		else if( !usingHotspotDutyCycle
				 && atHotspot )
		{
			(*inode)->raiseDutyCycle(HOTSPOT_DUTY_RATE);
			CPrintHelper::PrintDetail(now, "Duty cycle of " + (*inode)->getName() + " is raised.");
		}
	}
}

bool CHDC::isUsingHotspotDutyCycle(CNode * node)
{
	return EQUAL(node->getDutyCycle(), HOTSPOT_DUTY_RATE);
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

bool CHDC::Init()
{
	HOTSPOT_DUTY_RATE = getConfig<double>("hdc", "hotspot_duty_rate");
	return true;
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
	CMacProtocol::CommunicateBetweenNeighbors(now);

	return true;
}

