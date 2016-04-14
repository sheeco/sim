#include "PostSelect.h"
#include "SortHelper.h"
#include "HotspotSelect.h"

double CPostSelect::ALPHA = 0.03;  //ratio for post selection


CPostSelect::CPostSelect(vector<CHotspot *> selectedHotspots, vector<CHotspot *> &unselectedHotspots) : hotspotCandidates(selectedHotspots), unselectedHotspots(unselectedHotspots)
{
	this->maxRatio = 0;
	this->hotspotCandidates = CSortHelper::mergeSort(this->hotspotCandidates, CSortHelper::ascendByRatio);
	if(hotspotCandidates.size() > 0)
	{
		this->maxRatio = hotspotCandidates.at(hotspotCandidates.size() - 1)->getNCoveredPosition();
	}
}

double CPostSelect::getRatioForHotspot(CHotspot *hotspot) const
{
	if(maxRatio == 0)
	{
		cout << endl << "Error @ CPostSelect::getRatioForHotspot() : maxRatio = 0"<<endl;
		_PAUSE_;
		return -1;
	}
	else
	{
		//merge-HAR: ratio
		double ratioForMerge = pow( hotspot->getRatioByTypeHotspotCandidate(), hotspot->getAge() );
		//FIXME: balanced ratio for post selection
		return ratioForMerge * ( hotspot->getRatio() ) / double( maxRatio );
	}
}

void  CPostSelect::includeHotspots(CHotspot *hotspot)
{
	vector<CPosition *> positions = hotspot->getCoveredPositions();
	for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
	{
		AddToListUniquely(this->coveredNodes, (*ipos)->getNode());
		AddToListUniquely(this->coveredPositions, (*ipos)->getID());
	}
	this->selectedHotspots.push_back(hotspot);
}

void CPostSelect::findLostNodes()
{
	this->lostNodes.clear();
	for(vector<int>::iterator i = CNode::getIdNodes().begin(); i != CNode::getIdNodes().end(); ++i)
	{
		if( ! IfExists(this->coveredNodes, *i))
			this->lostNodes.push_back( *i );
	}
}

CHotspot* CPostSelect::findBestHotspotForNode(int inode)
{
	int maxCoverCount = 0;
	CHotspot *result = nullptr;
	for(int i = hotspotCandidates.size() - 1; i >= 0; i--)
	{
		CHotspot *ihotspot = hotspotCandidates[i];
		if(hotspotCandidates[i]->getFlag())
			continue;
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

	return result;
}


bool CPostSelect::verifyCompleted()
{
	findLostNodes();
	if(lostNodes.size() == 0)
		return true;
	else
		return false;
}

vector<CHotspot *> CPostSelect::PostSelect(int currentTime)
{
	flash_cout << "####  ( POST SELECT )          " ;

	for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); ++ihotspot)
	{
		(*ihotspot)->setFlag(false);
		(*ihotspot)->updateStatus();
	}
	//选中所有ratio >= ALPHA的hotspot
	for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); )
	{
		if(this->getRatioForHotspot(*ihotspot) >= ALPHA)
		{
			(*ihotspot)->setFlag(true);
			this->includeHotspots( (*ihotspot) );
			//将选中的热点从候选集中删除
			ihotspot = hotspotCandidates.erase(ihotspot);
		}
		else
		{
			++ihotspot;
			continue;
		}
	}
	//为每个lost node选中一个cover数最大的hotspot
	this->findLostNodes();
	for(vector<int>::iterator inode = lostNodes.begin(); inode != lostNodes.end(); ++inode)
	{
		CHotspot* hotspot = findBestHotspotForNode(*inode);
		if(hotspot != nullptr)
			includeHotspots(hotspot);
		//将选中的热点从候选集中删除
		for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); ++ihotspot)
		{
			if( (*ihotspot)->getID() == hotspot->getID() )
			{
				hotspotCandidates.erase(ihotspot);
				break;
			}
		}

	}
	if( ! verifyCompleted())
	{
		cout << endl << "Error @ CPostSelect::PostSelect() : not completed"<<endl;
		_PAUSE_;
	}

	//将未选中的候选热点放回全局候选集
	unselectedHotspots.insert(unselectedHotspots.end(), hotspotCandidates.begin(), hotspotCandidates.end() );
	return selectedHotspots;
}

//int CPostSelect::getNCoveredPositions() const
//{
//	return coveredPositions.size();
//}
