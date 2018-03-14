#include "PostSelect.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "PrintHelper.h"
#include "Configuration.h"


double CPostSelect::ALPHA = INVALID;

double CPostSelect::getRatioForHotspot(CHotspot *hotspot) const
{
	if(maxRatio == 0)
	{
		throw string("CPostSelect::getRatioForHotspot() : maxRatio = 0");
	}
	else
	{
		//merge-HAR: ratio
		double ratioForMerge = pow( hotspot->getRatioByTypeHotspotCandidate(), hotspot->getAge() );
		//FIXME: balanced ratio for post selection
		return ratioForMerge * ( hotspot->getRatio() ) / double( maxRatio );
	}
}

void CPostSelect::updateForNewlySelectedHotspot(CHotspot * hotspot)
{
	vector<CPosition *> positions = hotspot->getCoveredPositions();
	for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
	{
		AddToListUniquely(this->coveredNodes, ( *ipos )->getNode());
		AddToListUniquely(this->coveredPositions, ( *ipos )->getID());
	}
}

vector<int> CPostSelect::findLostNodes()
{
	vector<int> lostNodes = idNodes;
	RemoveFromList(lostNodes, coveredNodes);
	return lostNodes;
}

CHotspot * CPostSelect::findBestHotspotForNode(int inode)
{
	int maxCoverCount = 0;
	CHotspot *result = nullptr;
	for(int i = unselectedHotspots.size() - 1; i >= 0; i--)
	{
		CHotspot *ihotspot = unselectedHotspots[i];
		if(unselectedHotspots[i]->getFlag())
			continue;
		int coverCount = 0;
		vector<CPosition *> positions = ihotspot->getCoveredPositions();
		for(vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos)
		{
			if(( *ipos )->getNode() == inode)
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
	vector<int> lostNodes = findLostNodes();
	return lostNodes.size() == 0;
}

CPostSelect::CPostSelect(vector<CHotspot*> selectedHotspots, vector<int> idNodes) : unselectedHotspots(selectedHotspots)
{
	this->maxRatio = 0;
	this->unselectedHotspots = CSortHelper::mergeSort(this->unselectedHotspots, CSortHelper::ascendByRatio);
	if(unselectedHotspots.size() > 0)
	{
		this->maxRatio = unselectedHotspots.at(unselectedHotspots.size() - 1)->getNCoveredPosition();
	}
}

void CPostSelect::PostSelect()
{
	for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ++ihotspot)
	{
		( *ihotspot )->setFlag(false);
		( *ihotspot )->updateStatus();
	}
	//选中所有ratio >= hs.alpha的hotspot，按照 ratio 从大到小的顺序
	for(vector<CHotspot *>::reverse_iterator riHotspot = unselectedHotspots.rbegin(); riHotspot != unselectedHotspots.rend(); )
	{
		if(this->getRatioForHotspot(*riHotspot) >= ALPHA)
		{
			( *riHotspot )->setFlag(true);
			this->updateForNewlySelectedHotspot(( *riHotspot ));

			selectedHotspots.push_back(*riHotspot);
			//将选中的热点从候选集中删除（注意：删除反向迭代器指向的元素）
			vector<CHotspot *>::iterator ihotspot = unselectedHotspots.erase(( ++riHotspot ).base());
			riHotspot = vector<CHotspot *>::reverse_iterator(ihotspot);
		}
		else
		{
			++riHotspot;
			continue;
		}
	}
	//为每个lost node选中一个cover数最大的hotspot
	vector<int> lostNodes = this->findLostNodes();
	for(vector<int>::iterator inode = lostNodes.begin(); inode != lostNodes.end(); ++inode)
	{
		CHotspot* hotspot = findBestHotspotForNode(*inode);
		if(hotspot == nullptr)
			continue;

		updateForNewlySelectedHotspot(hotspot);
		selectedHotspots.push_back(hotspot);

		//将选中的热点从候选集中删除
		for(vector<CHotspot *>::iterator ihotspot = unselectedHotspots.begin(); ihotspot != unselectedHotspots.end(); ++ihotspot)
		{
			if(( *ihotspot )->getID() == hotspot->getID())
			{
				unselectedHotspots.erase(ihotspot);
				break;
			}
		}

	}
	if(!verifyCompleted())
	{
		throw string("CPostSelect::PostSelect() : not completed");
	}
}

//执行hotspot选取，返回得到的hotspot集合

void CPostSelect::Init()
{
	ALPHA = getConfig<double>("hs", "alpha");
}

void CPostSelect::PostSelect(vector<CHotspot*>& selectedHotspots, vector<CHotspot*>& unselectedHotspots, vector<int> idNodes)
{
	Init();

	CPrintHelper::PrintDoing("POST SELECT");

	CPostSelect selector(selectedHotspots, idNodes);
	selector.PostSelect();
	selectedHotspots = selector.selectedHotspots;
	unselectedHotspots.insert(unselectedHotspots.end(), selector.unselectedHotspots.begin(), selector.unselectedHotspots.end());

	CPrintHelper::PrintDoing(STRING(selectedHotspots.size()) + " hotspots");
	CPrintHelper::PrintDone();
}
