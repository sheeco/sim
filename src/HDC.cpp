#include "Global.h"
#include "HDC.h"
#include "HAR.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "Trace.h"
#include "PrintHelper.h"


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
			&& ( ! (*inode)->isAtHotspot() ) )
		{
			CPrintHelper::PrintDetail(currentTime, (*inode)->toString() + " leaves hotspot");
			(*inode)->resetDutyCycle();
		}
		else if( (*inode)->useDefaultDutyCycle()
				 && (*inode)->isAtHotspot() )
		{
			CPrintHelper::PrintDetail(currentTime, ( *inode )->toString() + " enters " + ( *inode )->getAtHotspot()->toString());
			(*inode)->raiseDutyCycle();
		}
	}

	//控制台输出时保留一位小数
	if( ( currentTime + SLOT ) % SLOT_LOG == 0 )
	{
		CPrintHelper::PrintPercentage("Hotspot Encounter", CNode::getPercentEncounterAtHotspot());
		print = true;
	}
	CPrintHelper::PrintPercentage("Hot-Node", atHotspotCount);

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

