#include "Position.h"

int CPosition::COUNT_ID = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������
int CPosition::nPositions = 0;
vector<CPosition *> CPosition::positions;
//vector<CPosition *> CPosition::deletedPositions;

//double CPosition::CO_POSITION_DECAY = 1.0;


int CPosition::getIndexPosition(CPosition* pos)
{
	for(int i = 0; i < nPositions; ++i)
	{
		if(positions[i] == pos)
			return i;
	}

	return -1;
}