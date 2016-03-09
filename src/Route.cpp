#include "Route.h"
#include "Sink.h"


CRoute::CRoute(CBasicEntity *sink)
{
	init();
	waypoints.push_back( sink );
	toPoint = CSink::getSink()->getID();  //��ʼ��Ϊsink
}

void CRoute::updateLength()
{
	if(waypoints.size() < 2)
	{
		cout << endl << "Error @ CRoute::updateLength() : this route is empty"<<endl;
		_PAUSE_;
	}
	length = 0;
	for(int i = 0, j = 1; i < waypoints.size(); i++, j = (j + 1) % waypoints.size())
		length += CBasicEntity::getDistance(*waypoints[i], *waypoints[j]);
	toPoint = 1;  //toPointָ��sink֮��ĵ�һ����
}

