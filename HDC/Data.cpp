#include "Data.h"
#include "Node.h"

int CData::ID_MASK = 10000000;
int CData::ID_COUNT = 0;
int CData::ARRIVAL_COUNT = 0;
double CData::DELAY_SUM = 0;
bool CData::MAX_HOP = 0;
bool CData::MAX_TTL = 0;
//int CData::OVERFLOW_COUNT = 0;
//int CData::DELIVERY_AT_HOTSPOT_COUNT = 0;
//int CData::DELIVERY_ON_ROUTE_COUNT = 0;

CData::~CData(void)
{
}

double CData::getAverageEnergyConsumption()
{
	if(ARRIVAL_COUNT == 0)
		return 0;
	else
		return CNode::getSumEnergyConsumption() / ARRIVAL_COUNT;
}
