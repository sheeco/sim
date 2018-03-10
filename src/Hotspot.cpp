#include "Hotspot.h"
#include "Configuration.h"
#include "SortHelper.h"
#include "Node.h"
#include "HAR.h"
#include "HotspotSelect.h"

int CHotspot::COUNT_ID = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������

map<int, CHotspot*> CHotspot::atHotspots;

int CHotspot::encounterAtHotspot = 0;
int CHotspot::visiterAtHotspot = 0;


bool CHotspot::ifPositionExists(CPosition* pos)
{
	if(coveredPositions.empty())
		return false;

	vector<CPosition *>::iterator ipos;
	for(ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
	{
		if(*ipos == pos)
			return true;
	}
	return false;
}

void CHotspot::removePosition(CPosition* pos)
{
	if( ! ifPositionExists(pos))
		return;

	vector<CPosition *>::iterator ipos;
	for(ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
	{
		if(*ipos == pos)
		{
			coveredPositions.erase(ipos);
			break;
		}
	}
}

void CHotspot::removePositionList(vector<CPosition *> positions)
{
	if(positions.empty())
		return;

	vector<CPosition *>::iterator ipos;
	for(ipos = positions.begin(); ipos != positions.end(); ++ipos)
	{
		removePosition(*ipos);
	}		
}

void CHotspot::addPosition(CPosition* pos)
{
	if(ifPositionExists(pos))
		return;

	coveredPositions.push_back(pos);
}

void CHotspot::recalculateCenter()
{
	if(coveredPositions.empty())
		return;

	double sum_x = 0;
	double sum_y = 0;
	vector<CPosition *>::iterator ipos;
	for(ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
	{
		sum_x += (*ipos)->getX();
		sum_y += (*ipos)->getY();
	}

	this->setLocation( sum_x / coveredPositions.size(), sum_y / coveredPositions.size() );
}

int CHotspot::getNCoveredPositionsForNode(int inode)
{
	int count = 0;
	for(vector<CPosition *>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
	{
		if((*ipos)->getNode() == inode)
			++count;
	}
	return count;
}

void CHotspot::generateCoveredNodes()
{
	this->coveredNodes.clear();
	for(vector<CPosition *>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
	{
		AddToListUniquely(coveredNodes, (*ipos)->getNode());
	}
}

//Ϊ���нڵ����Ƿ�λ���ȵ������ڣ���ͳ��visiter��encounter���ȵ��������
//visit �� encounter ������ͳ��ʱ�۽��ɹ켣�ļ�����

bool CHotspot::UpdateAtHotspotForNodes(vector<CNode*> nodes, vector<CHotspot*> hotspots, int now)
{
	if(!( getConfig<bool>("trace", "continuous")
		 || now % getConfig<int>("trace", "interval") == 0 ))
		return false;

	if(hotspots.empty()
	   || nodes.empty())
		return false;

	nodes = CSortHelper::mergeSort(nodes);
	hotspots = CSortHelper::mergeSort(hotspots, CSortHelper::ascendByLocationX);

	int countAtHotspot = 0;
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		CNode* pnode = *inode;
		CHotspot *newAtHotspot = nullptr;
		CHotspot *oldAtHotspot = getAtHotspot(pnode->getID());

		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
		{
			if(( *ihotspot )->getX() + getConfig<int>("trans", "range_trans") < pnode->getX())
				continue;
			if(pnode->getX() + getConfig<int>("trans", "range_trans") < ( *ihotspot )->getX())
				break;
			if(CBasicEntity::withinRange(**inode, **ihotspot, getConfig<int>("trans", "range_trans")))
			{
				newAtHotspot = *ihotspot;
				break;
			}
		}

		if(oldAtHotspot != nullptr
		   && newAtHotspot != oldAtHotspot)
			CPrintHelper::PrintDetail(now, pnode->getName() + " leaves " + oldAtHotspot->getName() + ".");
		if(newAtHotspot != nullptr
		   && newAtHotspot != oldAtHotspot)
			CPrintHelper::PrintDetail(now, pnode->getName() + " enters " + newAtHotspot->getName() + ".");
		if(newAtHotspot != nullptr)
			++countAtHotspot;

		//update atHotspot
		setAtWaypoint(pnode->getID(), newAtHotspot);

		// visit �� encounter ������ͳ��
		// ʱ�۽��ɹ켣�ļ�����
		if(now % getConfig<int>("trace", "interval") == 0)
		{
			CNode::visit();
			if(newAtHotspot != nullptr)
				visitAtHotspot();

			for(vector<CNode *>::iterator jnode = inode; jnode != nodes.end(); ++jnode)
			{
				if(pnode->getX() + getConfig<int>("trans", "range_trans") < ( *jnode )->getX())
					break;
				if(CBasicEntity::withinRange(**inode, **jnode, getConfig<int>("trans", "range_trans")))
				{
					CNode::encount();

					if(newAtHotspot != nullptr
					   || isAtWaypoint(( *jnode )->getID()))
						encountAtHotspot();
				}
			}
		}

	}

	//����̨���ʱ����һλС��
	if(( now + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0)
	{
		CPrintHelper::PrintPercentage("Encounters At Hotspots", CHotspot::getPercentEncounterAtHotspot());
	}
	CPrintHelper::PrintPercentage("Nodes At Hotspots", (double)countAtHotspot / nodes.size());

	return true;
}

double CHotspot::calculateRatio()
{
	if( getConfig<bool>("mhs", "test_balanced_ratio") )
	{
		ratio = coveredPositions.size() * double(CNode::getCountAliveNodes() - coveredNodes.size() + 1) / double(CNode::getCountAliveNodes());
		return ratio;
	}
//	else if( HAR::TEST_LEARN )
//	{
//		ratio = 0;
//		for(vector<CPosition*>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
//			ratio += (*ipos)->getWeight();
//		return ratio;
//
//	}
	else
	{
		ratio = coveredPositions.size();
		return ratio;
	}
}

double CHotspot::getRatioByTypeHotspotCandidate() const
{
	switch( this->typeHotspotCandidate )
	{
	case _merge_hotspot: 
		return getConfig<double>("mhs", "ratio_merge_hotspot");
	case _new_hotspot: 
		return getConfig<double>("mhs", "ratio_new_hotspot");
	case _old_hotspot: 
		return getConfig<double>("mhs", "ratio_old_hotspot");
	default:
		return 1;
	}
}

double CHotspot::getOverlapArea(CHotspot *oldHotspot, CHotspot *newHotspot)
{
	double distance = CBasicEntity::getDistance(*oldHotspot, *newHotspot);
	double cos = ( distance / 2 ) / getConfig<int>("trans", "range_trans");
	double sin = sqrt( 1 - cos * cos );
	double angle = acos(cos);
	double sector = ( angle / 2 ) * getConfig<int>("trans", "range_trans") * getConfig<int>("trans", "range_trans");
	double triangle = ( getConfig<int>("trans", "range_trans") * ( distance / 2 ) ) * sin / 2;

	return ( sector - triangle ) * 4 ;
}

void CHotspot::init()
{
	this->heat = 0;
	this->ratio = 0;

	//merge_HAR
	this->typeHotspotCandidate = _new_hotspot;
	this->age = 0;
}

CHotspot * CHotspot::generateHotspot(CCoordinate location, vector<CPosition*> positions, int time)
{
	CHotspot* photspot = new CHotspot(location, time);

	//������setTime֮���ʼ��
	photspot->countsDelivery[photspot->time] = 0;

	int nPositions = positions.size();
	//����flag
	for(int i = 0; i < nPositions; ++i)
		positions[i]->setFlag(false);

	bool modified;
	//ѭ����ֱ��û���µ�position������
	do
	{
		modified = false;
		//���µ�hotspot���ģ��ٴα���position
		for(int i = 0; i < nPositions; ++i)
		{
			if(positions[i]->getFlag())
				continue;
			if(positions[i]->getX() + getConfig<int>("trans", "range_trans") < photspot->getX())
				continue;
			//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
			if(photspot->getX() + getConfig<int>("trans", "range_trans") < positions[i]->getX())
				break;
			if(CBasicEntity::withinRange(*photspot, *positions[i], getConfig<int>("trans", "range_trans")))
			{
				photspot->addPosition(positions[i]);
				positions[i]->setFlag(true);
				modified = true;
			}
		}

		//���¼���hotspot������
		if(modified)
			photspot->recalculateCenter();
	} while(modified);

	return photspot;
}


double CHotspot::getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots)
{
	//Sort by Coordinate X to save time
	oldHotspots = CSortHelper::mergeSort(oldHotspots, CSortHelper::ascendByLocationX);
	newHotspots = CSortHelper::mergeSort(newHotspots, CSortHelper::ascendByLocationX);

	vector<CHotspot *>::iterator iOld, iNew;
	double sumArea = 0;
	for(iOld = oldHotspots.begin(); iOld != oldHotspots.end(); ++iOld)
	{
		for(iNew = newHotspots.begin(); iNew != newHotspots.end(); ++iNew)
		{
			if( (*iOld)->getID() == (*iNew)->getID() )
			{
				sumArea += AreaCircle( getConfig<int>("trans", "range_trans"));
				continue;
			}

			if( (*iNew)->getX() + 2 * getConfig<int>("trans", "range_trans") <= (*iOld)->getX() )
				continue;
			if( (*iOld)->getX() + 2 * getConfig<int>("trans", "range_trans") <= (*iNew)->getX() )
				break;

			if( CBasicEntity::withinRange(**iOld, **iNew, 2 * getConfig<int>("trans", "range_trans") ) )
			{
				sumArea += getOverlapArea(*iOld, *iNew);
			}
		}
	}

	return sumArea;
}

double CHotspot::getOverlapArea(vector<CHotspot *> hotspots)
{
	return getOverlapArea(hotspots, hotspots) - AreaCircle( getConfig<int>("trans", "range_trans")) * hotspots.size();
}