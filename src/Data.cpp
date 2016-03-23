#include "Data.h"
#include "Node.h"

//int CData::ID_MASK = 10000000;
int CData::COUNT_ID = 0;
int CData::COUNT_ARRIVAL = 0;
double CData::SUM_DELAY = 0;
int CData::COUNT_DELIVERY_AT_HOTSPOT = 0;
int CData::COUNT_DELIVERY_ON_ROUTE = 0;

int CData::MAX_HOP = 0;
int CData::MAX_TTL = 0;


double CData::getAverageEnergyConsumption()
{
	if(COUNT_ARRIVAL == 0)
		return 0;
	else
		return CNode::getSumEnergyConsumption() / COUNT_ARRIVAL;
}

vector<CData> CData::GetItemsByID(vector<CData> list, vector<int> ids)
{
	vector<CData> result;
	for(vector<int>::iterator id = ids.begin(); id != ids.end(); ++id)
	{
		for(vector<CData>::iterator item = list.begin(); item != list.end(); ++item)
		{
			if( item->getID() == *id )
			{
				result.push_back(*item);
				break;
			}
		}
	}
	return result;
}