#include "PostSelect.h"
#include "SortHelper.h"

double CPostSelect::ALPHA = 0.03;  //ratio for post selection


CPostSelect::CPostSelect(vector<CHotspot *> hotspotCandidates)
{
	this->maxRatio = 0;
	hotspotCandidates = CSortHelper::mergeSort(hotspotCandidates, CSortHelper::ascendByRatio);
	if(hotspotCandidates.size() > 0)
	{
		this->maxRatio = hotspotCandidates.at(hotspotCandidates.size() - 1)->getNCoveredPosition();
	}
	this->hotspotCandidates = hotspotCandidates;
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
		double ratioForMerge = pow( hotspot->getCoByCandidateType(), hotspot->getAge() );
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
				coverCount++;
		}
		if(coverCount > maxCoverCount)
		{
			maxCoverCount = coverCount;
			result = ihotspot;
		}
	}

	return result;
}

vector<CHotspot *> CPostSelect::assignPositionsToHotspots(vector<CHotspot *> hotspots) const
{
	vector<CHotspot *> temp_hotspots = hotspots;
	vector<CHotspot *> result_hotspots;
	while(! temp_hotspots.empty())
	{
		temp_hotspots = CSortHelper::mergeSort(temp_hotspots, CSortHelper::ascendByRatio);
		//FIXME:尽量多 / 平均？
		CHotspot *selected_hotspot = temp_hotspots.at(temp_hotspots.size() - 1);
		if(selected_hotspot->getNCoveredPosition() == 0)
			break;
		temp_hotspots.pop_back();
		result_hotspots.push_back(selected_hotspot);
		vector<CPosition *> positions = selected_hotspot->getCoveredPositions();
		for(vector<CHotspot *>::iterator ihotspot = temp_hotspots.begin(); ihotspot != temp_hotspots.end(); ++ihotspot)
		{
			(*ihotspot)->removePositionList(positions);
			(*ihotspot)->updateStatus();
		}
	}

	return result_hotspots;
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

	//分配每个position到唯一一个热点，并计算最终选取出的hotspot的cover的node，以备使用
	selectedHotspots = this->assignPositionsToHotspots(selectedHotspots);

	//将未选中的候选热点放回全局候选集
	CHotspot::hotspotCandidates.insert( CHotspot::hotspotCandidates.end(), hotspotCandidates.begin(), hotspotCandidates.end() );
	return selectedHotspots;
}

int CPostSelect::getNCoveredPositions() const
{
	return coveredPositions.size();
}
