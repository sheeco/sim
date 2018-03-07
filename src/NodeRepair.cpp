#include "NodeRepair.h"
#include "HotspotSelect.h"
#include "Node.h"
#include "PrintHelper.h"


double CNodeRepair::LAMBDA = INVALID;
int CNodeRepair::LIFETIME_POSITION = INVALID;

int CNodeRepair::countForNode(vector<CHotspot *> hotspots, int n) const
{
	int count = 0;
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
	{
		vector<CPosition *> positions = (*ihotspot)->getCoveredPositions();	
		for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
		{
			if((*ipos)->getNode() == n)
				++count;
		}
	}
	return count;
}

CHotspot* CNodeRepair::findMaxCoverHotspotForNode(int inode)
{
	int maxCoverCount = 0;
	CHotspot *result = nullptr;
	for(int i = unselectedHotspots.size() - 1; i >= 0; i--)
	{
		CHotspot *ihotspot = unselectedHotspots[i];
		int coverCount = 0;
		vector<CPosition *> positions = ihotspot->getCoveredPositions();
		for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
		{
			if((*ipos)->getNode() == inode)
				++coverCount;
		}
		if(coverCount > maxCoverCount)
		{
			maxCoverCount = coverCount;
			result = ihotspot;
		}
	}
	//if(result == nullptr)
	//{
	//	throw string("CNodeRepair::findMaxCoverHotspotForNode() : result = nullptr");
	//}
	return result;
}

void CNodeRepair::Repair()
{
	while(!poorNodes.empty())
	{
		int inode = poorNodes.back();
		while(countForNode(selectedHotspots, inode) < LAMBDA * min(time, LIFETIME_POSITION))
		{
			CHotspot *hotspot = findMaxCoverHotspotForNode(inode);
			if(hotspot != nullptr)
			{
				selectedHotspots.push_back(hotspot);
				RemoveFromList(unselectedHotspots, hotspot, CHotspot::identical);
			}
			else
				break;
		}
		poorNodes.pop_back();
	}

}

void CNodeRepair::Repair(vector<CHotspot*>& selectedHotspots, vector<CHotspot*>& unselectedHotspots, vector<int> idNodes, int now)
{
	Init();

	CPrintHelper::PrintDoing("POOR NODE REPAIR");

	CNodeRepair repair(selectedHotspots, unselectedHotspots, idNodes, now);
	repair.Repair();
	selectedHotspots = repair.selectedHotspots;
	unselectedHotspots = repair.unselectedHotspots;

	CPrintHelper::PrintDone();
}

