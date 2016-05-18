#include "Hotspot.h"
#include "SortHelper.h"
#include "Node.h"
#include "HAR.h"
#include "HotspotSelect.h"
#include "Trace.h"

int CHotspot::COUNT_ID = 0;  //从1开始，数值等于当前实例总数
vector<CHotspot *> CHotspot::hotspotCandidates;
vector<CHotspot *> CHotspot::selectedHotspots;
vector<CHotspot *> CHotspot::oldSelectedHotspots;
vector<CHotspot *> CHotspot::deletedHotspots;


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
	if( HAR::TEST_BALANCED_RATIO )
	{
		ratio = coveredPositions.size() * double(CNode::getNNodes() - coveredNodes.size() + 1) / double(CNode::getNNodes());
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
		return CHotspotSelect::RATIO_MERGE_HOTSPOT;
	case _new_hotspot: 
		return CHotspotSelect::RATIO_NEW_HOTSPOT;
	case _old_hotspot: 
		return CHotspotSelect::RATIO_OLD_HOTSPOT;
	default:
		return 1;
	}
}

//返回( untilTime - 900, untilTime )期间的投递计数
int CHotspot::getCountDelivery(int untilTime)
{
	int i = ( untilTime - time ) / CHotspotSelect::SLOT_HOTSPOT_UPDATE - 1;
	if( i < 0 || i > countsDelivery.size() )
	{
		cout << endl << "Error @ CHotspot::getCountDelivery(" << untilTime << ") : " << i << " exceeds (0," << countsDelivery.size() - 1 << ") !" << endl;
		_PAUSE_;
	}
	return countsDelivery.at( i );
}

int CHotspot::getWaitingTime(int untilTime)
{
	int i = ( untilTime - time ) / CHotspotSelect::SLOT_HOTSPOT_UPDATE - 1;
	if( i < 0 || i > waitingTimes.size() )
	{
		cout << endl << "Error @ CHotspot::getCountDelivery(" << untilTime << ") : " << i << " exceeds (0," << waitingTimes.size() - 1 << ") !" << endl;
		_PAUSE_;
	}		
	return waitingTimes[ i ];
}

double CHotspot::getOverlapArea(CHotspot *oldHotspot, CHotspot *newHotspot)
{
	double distance = CBasicEntity::getDistance(*oldHotspot, *newHotspot);
	double cos = ( distance / 2 ) / CGeneralNode::RANGE_TRANS;
	double sin = sqrt( 1 - cos * cos );
	double angle = acos(cos);
	double sector = ( angle / 2 ) * CGeneralNode::RANGE_TRANS * CGeneralNode::RANGE_TRANS;
	double triangle = ( CGeneralNode::RANGE_TRANS * ( distance / 2 ) ) * sin / 2;

	return ( sector - triangle ) * 4 ;
}

bool CHotspot::UpdateAtHotspotForNodes(int currentTime)
{
	if( ! ( CCTrace::CONTINUOUS_TRACE
		    || currentTime % CCTrace::SLOT_TRACE == 0 ) )
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
			if( (*ihotspot)->getX() + CGeneralNode::RANGE_TRANS < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + CGeneralNode::RANGE_TRANS < (*ihotspot)->getX() )
				break;
			if( CBasicEntity::withinRange( **inode, **ihotspot, CGeneralNode::RANGE_TRANS ) )		
			{
				atHotspot = *ihotspot;
				break;
			}
		}

		//update atHotspot
		(*inode)->setAtHotspot(atHotspot);

		// visit 和 encounter 计数的统计
		// 时槽仅由轨迹文件决定
		if( currentTime % CCTrace::SLOT_TRACE == 0 )
		{
			CNode::visit();
			if( ( *inode )->isAtHotspot() )
				CNode::visitAtHotspot();

			for(vector<CNode *>::iterator jnode = inode; jnode != nodes.end(); ++jnode)
			{
				if( (*inode)->getX() + CGeneralNode::RANGE_TRANS < (*jnode)->getX() )
					break;
				if( CBasicEntity::withinRange( **inode, **jnode, CGeneralNode::RANGE_TRANS ) )		
				{
					CNode::encount();

					if( atHotspot != nullptr
						|| (*jnode)->isAtHotspot() )
						CNode::encountAtHotspot();
				}
			}
		}

	}

	return true;
}

//bool CHotspot::UpdateAtHotspotForMANodes(int currentTime)
//{
//	if( ! ( currentTime % SLOT_HOTSPOT_UPDATE == 0 ) )
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
//			if( (*ihotspot)->getX() + CGeneralNode::RANGE_TRANS < (*iMA)->getX() )
//				continue;
//			if( (*iMA)->getX() + CGeneralNode::RANGE_TRANS < (*ihotspot)->getX() )
//				break;
//			if( CBasicEntity::withinRange( **iMA, **ihotspot, CGeneralNode::RANGE_TRANS ) )		
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
//			(*iMA)->setWaitingTime( HAR::calculateWaitingTime(currentTime, atHotspot) );
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
				sumArea += AreaCircle(CGeneralNode::RANGE_TRANS);
				continue;
			}

			if( (*iNew)->getX() + 2 * CGeneralNode::RANGE_TRANS <= (*iOld)->getX() )
				continue;
			if( (*iOld)->getX() + 2 * CGeneralNode::RANGE_TRANS <= (*iNew)->getX() )
				break;

			if( CBasicEntity::withinRange(**iOld, **iNew, 2 * CGeneralNode::RANGE_TRANS ) )
			{
				sumArea += getOverlapArea(*iOld, *iNew);
			}
		}
	}

	return sumArea;
}

double CHotspot::getOverlapArea(vector<CHotspot *> hotspots)
{
	return getOverlapArea(hotspots, hotspots) - AreaCircle(CGeneralNode::RANGE_TRANS) * hotspots.size();
}