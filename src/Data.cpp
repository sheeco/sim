#include "Data.h"
#include "Node.h"

//int CData::ID_MASK = 10000000;
int CData::COUNT_ID = 0;
int CData::COUNT_ARRIVAL = 0;
double CData::SUM_DELAY = 0;
double CData::SUM_HOP = 0;
int CData::COUNT_DELIVERY_AT_HOTSPOT = 0;
int CData::COUNT_DELIVERY_ON_ROUTE = 0;

int CData::MAX_HOP = 0;
//int CData::MAX_TTL = 0;


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

//重载比较操作符，比较生成时间，用于mergeSort
bool operator < (const CData lt, const CData rt)
{
	return lt.getTimeBirth() < rt.getTimeBirth();
}
bool operator > (const CData lt, const CData rt)
{
	return lt.getTimeBirth() > rt.getTimeBirth();
}

//重载 == 操作符，比较 ID，用于去重
bool operator == (const CData lt, const CData rt)
{
	return lt.getID() == rt.getID();
}

//重载操作符 == 用于根据 ID 判断 identical
bool operator == (int id, const CData data)
{
	return data.getID() == id;
}
bool operator == (const CData data, int id)
{
	return data.getID() == id;
}
