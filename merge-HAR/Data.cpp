#include "Data.h"
#include "MANode.h"

long int CData::ID_COUNT = 0;
long int CData::ARRIVAL_COUNT = 0;
long double CData::DELAY_SUM = 0;
long int CData::OVERFLOW_COUNT = 0;

CData::~CData(void)
{
}

double CData::getAverageEnergyConsumption()
{
	if(ARRIVAL_COUNT == 0)
		return 0;
	else
		return ( CNode::getEnergyConsumption() + CMANode::getEnergyConsumption() ) / ARRIVAL_COUNT;
}
