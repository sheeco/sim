#include "Route.h"
#include "Sink.h"


void CRoute::updateLength()
{
	if(waypoints.size() < 2)
	{
		throw string("CRoute::updateLength() : this route is empty");
	}
	length = 0;
	for(int i = 0, j = 1; i < waypoints.size(); ++i, j = (j + 1) % waypoints.size())
		length += CBasicEntity::getDistance(*waypoints[i].first, *waypoints[j].first);
	toPoint = 1;  //toPoint指向sink之后的第一个点
}

//string CRoute::toString()
//{
//	stringstream sstr;
//	for( vector<CBasicEntity*>::iterator iwaypoint = waypoints.begin(); iwaypoint != waypoints.end(); iwaypoint++ )
//	{
//		sstr << ( *iwaypoint )->getLocation().toString() << TAB;
//	}
//	// e.g "0.0, 1.234	234.5, 345.6 ..."
//	return sstr.str();
//}
