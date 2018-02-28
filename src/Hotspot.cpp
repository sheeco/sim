#include "Hotspot.h"
#include "Configuration.h"
#include "SortHelper.h"
#include "Node.h"
#include "HAR.h"
#include "HotspotSelect.h"

int CHotspot::COUNT_ID = 0;  //从1开始，数值等于当前实例总数
vector<CHotspot *> CHotspot::hotspotCandidates;
vector<CHotspot *> CHotspot::selectedHotspots;
map<int, vector<CHotspot *>> CHotspot::oldSelectedHotspots;
//vector<CHotspot *> CHotspot::deletedHotspots;


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

bool CHotspot::ifNodeExists(int inode) const
{
	return ( IfExists(coveredNodes, inode) );
}

void CHotspot::generateCoveredNodes()
{
	this->coveredNodes.clear();
	for(vector<CPosition *>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
	{
		AddToListUniquely(coveredNodes, (*ipos)->getNode());
	}
}

double CHotspot::calculateRatio()
{
	if( getConfig<bool>("mhs", "test_balanced_ratio") )
	{
		ratio = coveredPositions.size() * double(CNode::getNodeCount() - coveredNodes.size() + 1) / double(CNode::getNodeCount());
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

bool CHotspot::UpdateAtHotspotForNodes(int now)
{
	if( ! ( getConfig<bool>("trace", "continuous")
		    || now % getConfig<int>("trace", "interval") == 0 ) )
		return false;

	vector<CHotspot *> hotspots = selectedHotspots;
	vector<CNode *> nodes = CNode::getNodes();
	if( hotspots.empty()
		|| nodes.empty() )
		return false;

	nodes = CSortHelper::mergeSort( nodes );
	hotspots = CSortHelper::mergeSort( hotspots, CSortHelper::ascendByLocationX );

	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		CHotspot *atHotspot = nullptr;

		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
		{
			if( (*ihotspot)->getX() + getConfig<int>("trans", "range_trans") < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + getConfig<int>("trans", "range_trans") < (*ihotspot)->getX() )
				break;
			if( CBasicEntity::withinRange( **inode, **ihotspot, getConfig<int>("trans", "range_trans") ) )		
			{
				atHotspot = *ihotspot;
				break;
			}
		}

		//update atHotspot
		(*inode)->setAtWaypoint(atHotspot);

		// visit 和 encounter 计数的统计
		// 时槽仅由轨迹文件决定
		if( now % getConfig<int>("trace", "interval") == 0 )
		{
			CNode::visit();
			if( ( *inode )->isAtWaypoint() )
				CNode::visitAtHotspot();

			for(vector<CNode *>::iterator jnode = inode; jnode != nodes.end(); ++jnode)
			{
				if( (*inode)->getX() + getConfig<int>("trans", "range_trans") < (*jnode)->getX() )
					break;
				if( CBasicEntity::withinRange( **inode, **jnode, getConfig<int>("trans", "range_trans") ) )		
				{
					CNode::encount();

					if( atHotspot != nullptr
						|| (*jnode)->isAtWaypoint() )
						CNode::encountAtWaypoint();
				}
			}
		}

	}

	return true;
}

//bool CHotspot::UpdateAtHotspotForMANodes(int now)
//{
//	if( ! ( now % getConfig<int>("hs", "slot_hotspot_update") == 0 ) )
//		return false;
//
//	vector<CHotspot *> hotspots = selectedHotspots;
//	vector<CMANode *> mas = CMANode::getMANodes();
//	if( hotspots.empty() 
//		|| mas.empty() )
//		return false;
//
//	hotspots = CSortHelper::mergeSort( hotspots, CSortHelper::ascendByLocationX );
//
//	for(vector<CMANode *>::iterator iMA = mas.begin(); iMA != mas.end(); ++iMA)
//	{
//		CHotspot *atHotspot = nullptr;
//
//		for(vector<CHotspot *>::iterator ihotspot = hotspots.begin(); ihotspot != hotspots.end(); ++ihotspot)
//		{
//			if( (*ihotspot)->getX() + getConfig<int>("trans", "range_trans") < (*iMA)->getX() )
//				continue;
//			if( (*iMA)->getX() + getConfig<int>("trans", "range_trans") < (*ihotspot)->getX() )
//				break;
//			if( CBasicEntity::withinRange( **iMA, **ihotspot, getConfig<int>("trans", "range_trans") ) )		
//			{
//				atHotspot = *ihotspot;
//				break;
//			}
//		}
//
//		//update atHotspot
//		if( ( ! (*iMA)->isAtHotspot() )
//			&& atHotspot != nullptr )
//		{
//			(*iMA)->setAtHotspot(atHotspot);
//			(*iMA)->setWaitingTime( HAR::calculateWaitingTime(now, atHotspot) );
//		}
//	}
//
//	return true;
//}

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