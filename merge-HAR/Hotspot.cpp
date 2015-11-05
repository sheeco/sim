#include "Hotspot.h"

long int CHotspot::ID_COUNT = 0;

bool CHotspot::ifPositionExists(CPosition* pos)
{
	if(coveredPositions.empty())
		return false;

	vector<CPosition *>::iterator ipos;
	for(ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ipos++)
	{
		if(*ipos == pos)
			return true;
	}
	return false;
}

void CHotspot::removePosition(CPosition* pos)
{
	if(! ifPositionExists(pos))
		return;

	vector<CPosition *>::iterator ipos;
	for(ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ipos++)
	{
		if(*ipos == pos)
		{
			coveredPositions.erase(ipos);
			nCoveredPosition--;
			break;
		}
	}
	if(nCoveredPosition < 0)
	{
		cout << "Error: CHotspot::removePosition() nCoveredPosition < 0" << endl;
		_PAUSE;
	}
	//recalculateCenter();  //���Զ�����
}

void CHotspot::removePositionList(vector<CPosition *> positions)
{
	if(positions.empty())
		return;

	vector<CPosition *>::iterator ipos;
	for(ipos = positions.begin(); ipos != positions.end(); ipos++)
	{
		removePosition(*ipos);
	}		
}

void CHotspot::addPosition(CPosition* pos)
{
	if(ifPositionExists(pos))
		return;

	coveredPositions.push_back(pos);
	nCoveredPosition++;

	//recalculateCenter();  //���Զ�����
}

void CHotspot::recalculateCenter()
{
	if(coveredPositions.empty())
		return;

	double sum_x = 0;
	double sum_y = 0;
	vector<CPosition *>::iterator ipos;
	for(ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ipos++)
	{
		sum_x += (*ipos)->getX();
		sum_y += (*ipos)->getY();
	}

	this->x = sum_x / nCoveredPosition;
	this->y = sum_y / nCoveredPosition;
}

int CHotspot::getNCoveredPositionsForNode(int inode)
{
	int count = 0;
	for(vector<CPosition *>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ipos++)
	{
		if((*ipos)->getNode() == inode)
			count++;
	}
	return count;
}

bool CHotspot::ifNodeExists(int inode)
{
	return ( ifExists(coveredNodes, inode) );
}

void CHotspot::generateCoveredNodes()
{
	this->coveredNodes.clear();
	for(vector<CPosition *>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ipos++)
	{
		addToListUniquely(coveredNodes, (*ipos)->getNode());
	}
}

string CHotspot::toString(bool withDetails)
{
	ostringstream os;
	os << this->time << TAB << this->age << TAB << this->ID << TAB << this->x << TAB << this->y << TAB << nCoveredPosition << TAB ;
	if(withDetails)
	{
		for(int i = 0; i < nCoveredPosition; i++)
			os << coveredPositions[i]->getID() << TAB ;
	}
	return os.str();
}
