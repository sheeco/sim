#include "Position.h"

int CPosition::ID_COUNT = 0;  //��1��ʼ����ֵ����position����
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