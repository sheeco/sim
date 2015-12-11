#include "NodeRepair.h"


CNodeRepair::CNodeRepair(vector<CHotspot *> selectedHotspots, vector<CHotspot *> hotspotCandidates, int time)
{	
	this->time = time;
	this->selectedHotspots = selectedHotspots;
	if(! hotspotCandidates.empty())
	{
		for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); ihotspot++)
		{
			if(! ifExists(this->selectedHotspots, *ihotspot, CHotspot::identical))
				unselectedHotspots.push_back(*ihotspot);
		}
	}
	//初始化为所有node
	poorNodes.insert( poorNodes.begin(), CNode::getIdNodes().begin(), CNode::getIdNodes().end() );
}

int CNodeRepair::countForNode(vector<CHotspot *> hotspots, int n)
{
	int count = 0;
	for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ihotspot++)
	{
		vector<CPosition *> positions = (*ihotspot)->getCoveredPositions();	
		for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ipos++)
		{
			if((*ipos)->getNode() == n)
				count++;
		}
	}
	return count;
}

CHotspot* CNodeRepair::findMaxCoverHotspotForNode(int inode)
{
	int maxCoverCount = 0;
	CHotspot *result = NULL;
	for(int i = unselectedHotspots.size() - 1; i >= 0; i--)
	{
		CHotspot *ihotspot = unselectedHotspots[i];
		int coverCount = 0;
		vector<CPosition *> positions = ihotspot->getCoveredPositions();
		for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ipos++)
		{
			if((*ipos)->getNode() == inode)
				coverCount++;
		}
		if(coverCount > maxCoverCount)
		{
			maxCoverCount = coverCount;
			result = ihotspot;
		}
	}
	//if(result == NULL)
	//{
	//	cout<<"Error: CNodeRepair::findMaxCoverHotspotForNode() result = NULL"<<endl;
	//	_PAUSE;
	//}
	return result;
}

vector<CHotspot *> CNodeRepair::RepairPoorNodes()
{
	while(! poorNodes.empty())
	{
		int inode = poorNodes[0];
		while(countForNode(selectedHotspots, inode) < LAMBDA * min(time, MAX_MEMORY_TIME) )
		{
			CHotspot *hotspot = findMaxCoverHotspotForNode(inode);
			if(hotspot != NULL)
			{
				selectedHotspots.push_back(hotspot);
				RemoveFromList(unselectedHotspots, hotspot, CHotspot::identical);
			}
			else
				break;
		}
		poorNodes.erase(poorNodes.begin());
	}

	CHotspot::hotspotCandidates = unselectedHotspots;
	return selectedHotspots;
}

CNodeRepair::~CNodeRepair(void)
{
}
