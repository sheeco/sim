#include "NodeRepair.h"
#include "HotspotSelect.h"
#include "Node.h"
#include "PrintHelper.h"


CNodeRepair::CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> &unselectedHotspots) : selectedHotspots(selectedHotspots), unselectedHotspots(unselectedHotspots)
{	
	//this->selectedHotspots = selectedHotspots;
	//this->unselectedHotspots = unselectedHotspots;
	//if( ! hotspotCandidates.empty())
	//{
	//	for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); ++ihotspot)
	//	{
	//		if( ! IfExists(this->selectedHotspots, *ihotspot, CHotspot::identical))
	//			unselectedHotspots.push_back(*ihotspot);
	//	}
	//}

	// 初始化为所有node
	vector<int> idNodes = CNode::getIdNodes();
	poorNodes.insert( poorNodes.begin(), idNodes.begin(), idNodes.end() );
}

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

vector<CHotspot *> CNodeRepair::RepairPoorNodes(int time)
{
	CPrintHelper::PrintDoing("POOR NODE REPAIR");

	this->time = time;
	while(! poorNodes.empty())
	{
		int inode = poorNodes[0];
		while(countForNode(selectedHotspots, inode) < getConfig<double>("ihs", "lambda") * min(time, getConfig<int>("ihs", "lifetime_position")) )
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
		poorNodes.erase(poorNodes.begin());
	}

	CPrintHelper::PrintDone();
	return selectedHotspots;
}

//CNodeRepair::~CNodeRepair()
//{
//}
