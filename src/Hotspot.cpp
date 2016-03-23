#include "Hotspot.h"
#include "SortHelper.h"
#include "Node.h"
#include "HAR.h"

int CHotspot::COUNT_ID = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������
vector<CHotspot *> CHotspot::hotspotCandidates;
vector<CHotspot *> CHotspot::selectedHotspots;
vector<CHotspot *> CHotspot::oldSelectedHotspots;
vector<CHotspot *> CHotspot::deletedHotspots;

int CHotspot::SLOT_POSITION_UPDATE = 100;  //������Ϣ�ռ���slot
int CHotspot::SLOT_HOTSPOT_UPDATE = 900;  //�����ȵ�ͷ����slot
int CHotspot::TIME_HOSPOT_SELECT_START = SLOT_HOTSPOT_UPDATE;  //no MA node at first
double CHotspot::RATIO_MERGE_HOTSPOT = 1.0;
double CHotspot::RATIO_NEW_HOTSPOT = 1.0;
double CHotspot::RATIO_OLD_HOTSPOT = 1.0;


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

			if( CBasicEntity::getDistance(**iOld, **iNew) < 2 * CGeneralNode::RANGE_TRANS)
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