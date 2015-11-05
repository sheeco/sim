#include "PostSelector.h"

CPostSelector::CPostSelector(vector<CHotspot *> hotspotCandidates)
{
	this->maxCoverNum = 0;
	hotspotCandidates = CPreprocessor::mergeSort(hotspotCandidates, CPreprocessor::largerByNCoveredPositions);
	if(hotspotCandidates.size() > 0)
	{
		this->maxCoverNum = hotspotCandidates.at(hotspotCandidates.size() - 1)->getNCoveredPosition();
	}
	this->hotspotCandidates = hotspotCandidates;
}


CPostSelector::~CPostSelector(void)
{
}

double CPostSelector::getRatioForHotspot(CHotspot *hotspot)
{
	if(maxCoverNum == 0)
	{
		cout<<"Error: CPostSelector::getRatioForHotspot() maxCoverNum = 0"<<endl;
		_PAUSE;
		return -1;
	}
	else if(maxCoverNum == 1)
	{
		return 1;
	}
	else
	{
		//merge-HAR: ratio
		double ratioForMerge = pow( hotspot->getRatioByCandidateType(), hotspot->getAge() );
		return ratioForMerge * ( hotspot->getNCoveredPosition() - 1 ) / ( maxCoverNum - 1 );
	}
}

void  CPostSelector::includeHotspots(CHotspot *hotspot)
{
	vector<CPosition *> positions = hotspot->getCoveredPositions();
	for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ipos++)
	{
		addToListUniquely(this->coveredNodes, (*ipos)->getNode());
		addToListUniquely(this->coveredPositions, (*ipos)->getID());
	}
	this->selectedHotspots.push_back(hotspot);
}

void CPostSelector::findLostNodes()
{
	this->lostNodes.clear();
	for(int i = 0; i < NUM_NODE; i++)
	{
		if(! ifExists(this->coveredNodes, i))
			this->lostNodes.push_back(i);
	}
}

CHotspot* CPostSelector::findMaxCoverHotspotForNode(int inode)
{
	int maxCoverCount = 0;
	CHotspot *result = NULL;
	for(int i = hotspotCandidates.size() - 1; i >= 0; i--)
	{
		CHotspot *ihotspot = hotspotCandidates[i];
		if(hotspotCandidates[i]->getFlag())
			continue;
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
	//	cout<<"Error: CPostSelector::findMaxCoverHotspotForNode() result = NULL"<<endl;
	//	_PAUSE;
	//}
	return result;
}

vector<CHotspot *> CPostSelector::assignPositionsToHotspots(vector<CHotspot *> hotspots)
{
	vector<CHotspot *> tmp_hotspots = hotspots;
	vector<CHotspot *> result_hotspots;
	while(! tmp_hotspots.empty())
	{
		tmp_hotspots = CPreprocessor::mergeSort(tmp_hotspots, CPreprocessor::largerByNCoveredPositions);
		//FIXME:尽量多 / 平均？
		CHotspot *selected_hotspot = tmp_hotspots.at(tmp_hotspots.size() - 1);
		if(selected_hotspot->getNCoveredPosition() == 0)
			break;
		tmp_hotspots.pop_back();
		result_hotspots.push_back(selected_hotspot);
		vector<CPosition *> positions = selected_hotspot->getCoveredPositions();
		for(vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin(); ihotspot != tmp_hotspots.end(); ihotspot++)
		{
			(*ihotspot)->removePositionList(positions);
		}
	}
	//计算最终选取出的hotspot的cover的node，以备使用
	for(int i = 0; i < result_hotspots.size(); i++)
	{
		result_hotspots[i]->generateCoveredNodes();
		result_hotspots[i]->recalculateCenter();
	}

	return result_hotspots;
}

bool CPostSelector::verifyCompleted()
{
	findLostNodes();
	if(lostNodes.size() == 0)
		return true;
	else
		return false;
}

vector<CHotspot *> CPostSelector::PostSelect(int currentTime)
{
	for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); ihotspot++)
		(*ihotspot)->setFlag(false);
	//选中所有ratio >= ALPHA的hotspot
	for(int i = hotspotCandidates.size() - 1; i >= 0; i--)
	{
		CHotspot *ihotspot = hotspotCandidates[i];
		if(this->getRatioForHotspot(ihotspot) >= ALPHA)
		{
			ihotspot->setFlag(true);
			this->includeHotspots(ihotspot);
		}
		else
			break;
	}
	//为每个lost node选中一个cover数最大的hotspot
	this->findLostNodes();
	for(vector<int>::iterator inode = lostNodes.begin(); inode != lostNodes.end(); inode++)
	{
		if( ifExists(coveredNodes, *inode) )
			continue;
		CHotspot* hotspot = findMaxCoverHotspotForNode(*inode);
		if(hotspot != NULL)
			includeHotspots(hotspot);
	}
	if(! verifyCompleted())
	{
		cout<<"Error: CPostSelector::PostSelect() not completed"<<endl;
		_PAUSE;
	}

	selectedHotspots = this->assignPositionsToHotspots(selectedHotspots);
	//计算最终选取出的hotspot的cover的node，以备使用
	for(int i = 0; i < selectedHotspots.size(); i++)
	{
		selectedHotspots[i]->generateCoveredNodes();
		selectedHotspots[i]->recalculateCenter();
	}
	return selectedHotspots;
}

int CPostSelector::getNCoveredPositions()
{
	return coveredPositions.size();
}