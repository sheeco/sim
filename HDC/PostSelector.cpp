#include "PostSelector.h"

CPostSelector::CPostSelector(vector<CHotspot *> hotspotCandidates)
{
	this->maxRatio = 0;
	hotspotCandidates = CPreprocessor::mergeSort(hotspotCandidates, CPreprocessor::ascendByRatio);
	if(hotspotCandidates.size() > 0)
	{
		this->maxRatio = hotspotCandidates.at(hotspotCandidates.size() - 1)->getNCoveredPosition();
	}
	this->hotspotCandidates = hotspotCandidates;
}


CPostSelector::~CPostSelector(void)
{
}

double CPostSelector::getRatioForHotspot(CHotspot *hotspot)
{
	if(maxRatio == 0)
	{
		cout<<"Error @ CPostSelector::getRatioForHotspot() : maxRatio = 0"<<endl;
		_PAUSE;
		return -1;
	}
	else
	{
		//merge-HAR: ratio
		double ratioForMerge = pow( hotspot->getCoByCandidateType(), hotspot->getAge() );
		//FIXME: balanced ratio for post selection
		return ratioForMerge * ( hotspot->getRatio() ) / (double)( maxRatio );
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
	for(vector<int>::iterator i = CNode::getIdNodes().begin(); i != CNode::getIdNodes().end(); i++)
	{
		if(! ifExists(this->coveredNodes, *i))
			this->lostNodes.push_back( *i );
	}
}

CHotspot* CPostSelector::findBestHotspotForNode(int inode)
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

	return result;
}

vector<CHotspot *> CPostSelector::assignPositionsToHotspots(vector<CHotspot *> hotspots)
{
	vector<CHotspot *> tmp_hotspots = hotspots;
	vector<CHotspot *> result_hotspots;
	while(! tmp_hotspots.empty())
	{
		tmp_hotspots = CPreprocessor::mergeSort(tmp_hotspots, CPreprocessor::ascendByRatio);
		//FIXME:������ / ƽ����
		CHotspot *selected_hotspot = tmp_hotspots.at(tmp_hotspots.size() - 1);
		if(selected_hotspot->getNCoveredPosition() == 0)
			break;
		tmp_hotspots.pop_back();
		result_hotspots.push_back(selected_hotspot);
		vector<CPosition *> positions = selected_hotspot->getCoveredPositions();
		for(vector<CHotspot *>::iterator ihotspot = tmp_hotspots.begin(); ihotspot != tmp_hotspots.end(); ihotspot++)
		{
			(*ihotspot)->removePositionList(positions);
			(*ihotspot)->updateStatus();
		}
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
	flash_cout << "####  ( POST SELECT )          " ;

	for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); ihotspot++)
	{
		(*ihotspot)->setFlag(false);
		(*ihotspot)->updateStatus();
	}
	//ѡ������ratio >= ALPHA��hotspot
	for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); )
	{
		if( selectedHotspots.size() >= MAX_NUM_HOTSPOT )
			break;

		if(this->getRatioForHotspot(*ihotspot) >= ALPHA)
		{
			(*ihotspot)->setFlag(true);
			this->includeHotspots( (*ihotspot) );
			//��ѡ�е��ȵ�Ӻ�ѡ����ɾ��
			ihotspot = hotspotCandidates.erase(ihotspot);
		}
		else
		{
			ihotspot++;
			continue;
		}
	}
	//Ϊÿ��lost nodeѡ��һ��cover������hotspot
	this->findLostNodes();
	for(vector<int>::iterator inode = lostNodes.begin(); inode != lostNodes.end(); inode++)
	{
		CHotspot* hotspot = findBestHotspotForNode(*inode);
		if(hotspot != NULL)
			includeHotspots(hotspot);
		//��ѡ�е��ȵ�Ӻ�ѡ����ɾ��
		for(vector<CHotspot *>::iterator ihotspot = hotspotCandidates.begin(); ihotspot != hotspotCandidates.end(); ihotspot++)
		{
			if( (*ihotspot)->getID() == hotspot->getID() )
			{
				hotspotCandidates.erase(ihotspot);
				break;
			}
		}

	}
	if(! verifyCompleted())
	{
		cout<<"Error @ CPostSelector::PostSelect() : not completed"<<endl;
		_PAUSE;
	}

	//����ÿ��position��Ψһһ���ȵ㣬����������ѡȡ����hotspot��cover��node���Ա�ʹ��
	selectedHotspots = this->assignPositionsToHotspots(selectedHotspots);

	//��δѡ�еĺ�ѡ�ȵ�Ż�ȫ�ֺ�ѡ��
	CHotspot::hotspotCandidates.insert( CHotspot::hotspotCandidates.end(), hotspotCandidates.begin(), hotspotCandidates.end() );
	return selectedHotspots;
}

int CPostSelector::getNCoveredPositions()
{
	return coveredPositions.size();
}