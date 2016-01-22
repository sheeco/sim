#include "Position.h"

int CPosition::ID_COUNT = 0;  //从1开始，数值等于position总数
int CPosition::nPositions = 0;
vector<CPosition *> CPosition::positions;
vector<CPosition *> CPosition::deletedPositions;