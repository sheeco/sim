#include "Route.h"


void CRoute::updateLength()
{
	if(waypoints.size() <= 1)
	{
		throw string("CRoute::updateLength() : this route is empty");
	}
	length = 0;
	for(int i = 0, j = 1; i < waypoints.size(); ++i, j = (j + 1) % waypoints.size())
		length += CBasicEntity::getDistance(*waypoints[i].first, *waypoints[j].first);
	initToPoint();
}
