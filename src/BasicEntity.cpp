#include "BasicEntity.h"


vector<CBasicEntity*> CBasicEntity::GetItemsByID(vector<CBasicEntity*> list, vector<int> ids)
{
	vector<CBasicEntity*> result;
	for(vector<int>::iterator id = ids.begin(); id != ids.end(); ++id)
	{
		for(vector<CBasicEntity*>::iterator item = list.begin(); item != list.end(); ++item)
		{
			if( (*item)->getID() == *id )
			{
				result.push_back(*item);
				break;
			}
		}
	}
	return result;
}

int CBasicEntity::moveTo(CBasicEntity to, int time, double speed)
{
	double fromX, fromY, toX, toY;
	fromX = this->getX();
	fromY = this->getY();
	toX = to.getX();
	toY = to.getY();

	double sin, cos, distance;
	distance = sqrt((fromX - toX) * (fromX - toX) + (fromY - toY) * (fromY - toY));

	//尚未到达
	int timeArrival = distance / speed;
	if( timeArrival > time )
	{
		cos = (toX - fromX) / distance;
		sin = (toY - fromY) / distance;
		this->setLocation(fromX +  time * speed * cos, fromY + time * speed * sin);
		this->setTime( this->getTime() + time );
	}
	//将到达
	else
	{
		this->setLocation(toX, toY);			
		this->setTime( this->getTime() + timeArrival );
	}
	return ( time - timeArrival );
}
