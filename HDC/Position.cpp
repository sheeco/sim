#include "Position.h"

int CPosition::ID_COUNT = 0;  //��1��ʼ����ֵ����position����
int CPosition::nPositions = 0;
vector<CPosition *> CPosition::positions;
vector<CPosition *> CPosition::deletedPositions;