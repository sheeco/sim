#include "Position.h"

int CPosition::ID_COUNT = 0;  //从1开始，数值等于position总数
int CPosition::nPositions = 0;
vector<CPosition *> CPosition::positions;
vector<CPosition *> CPosition::deletedPositions;

int CPosition::getIndexOfPosition(CPosition* pos)
{
	for(int i = 0; i < nPositions; i++)
	{
		if(positions[i] == pos)
			return i;
	}

	return -1;
}