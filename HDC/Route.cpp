#include "Route.h"

void CRoute::updateLength()
{
	if(waypoints.size() < 2)
	{
		cout<<"Error: CRoute::updateLength() this route is empty"<<endl;
		_PAUSE;
	}
	length = 0;
	for(int i = 0, j = 1; i < waypoints.size(); i++, j = (j + 1) % waypoints.size())
		length += CBasicEntity::getDistance(*waypoints[i], *waypoints[j]);
	toPoint = 1;  //toPointָ��sink֮��ĵ�һ����
}

string CRoute::toString()
{
	stringstream str;
	for(vector<CBasicEntity *>::iterator ipoint = waypoints.begin(); ipoint != waypoints.end(); ipoint++)
		str << (*ipoint)->getID() << " - ";
	str << (* waypoints.begin() )->getID() << TAB;
	str << length / SPEED_MANODE << endl;

	return str.str();
}