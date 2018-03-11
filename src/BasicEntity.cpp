#include "BasicEntity.h"


//vector<CBasicEntity*> CBasicEntity::GetItemsByID(vector<CBasicEntity*> list, vector<int> ids)
//{
//	vector<CBasicEntity*> result;
//	for(vector<int>::iterator id = ids.begin(); id != ids.end(); ++id)
//	{
//		for(vector<CBasicEntity*>::iterator item = list.begin(); item != list.end(); ++item)
//		{
//			if( (*item)->getID() == *id )
//			{
//				result.push_back(*item);
//				break;
//			}
//		}
//	}
//	return result;
//}

int CBasicEntity::moveToward(CBasicEntity &toward, int interval, double speed)
{
	double fromX, fromY, toX, toY;
	fromX = this->getX();
	fromY = this->getY();
	toX = toward.getX();
	toY = toward.getY();

	double sin, cos, distance;
	distance = sqrt((fromX - toX) * (fromX - toX) + (fromY - toY) * (fromY - toY));

	//��δ����
	int timeArrival = int( distance / speed );
	if( timeArrival > interval )
	{
		cos = (toX - fromX) / distance;
		sin = (toY - fromY) / distance;
		this->setLocation(fromX +  interval * speed * cos, fromY + interval * speed * sin);
		this->setTime( this->getTime() + interval );
	}
	//������
	else
	{
		this->setLocation(toX, toY);			
		this->setTime( this->getTime() + timeArrival );
	}
	return ( interval - timeArrival );
}

//ע�⣺��double �͵�����ִ�� == ������ʱ���ھ�������
bool operator == (const CBasicEntity &lt, const CBasicEntity &rt)
{
	return ( lt.getLocation().getX() == rt.getLocation().getX() );
}
bool operator != (const CBasicEntity &lt, const CBasicEntity &rt)
{
	return ( lt.getLocation().getX() != rt.getLocation().getX() );
}
bool operator < (const CBasicEntity &lt, const CBasicEntity &rt)
{
	return ( lt.getLocation().getX() < rt.getLocation().getX() );
}
bool operator > (const CBasicEntity &lt, const CBasicEntity &rt)
{
	return ( lt.getLocation().getX() > rt.getLocation().getX() );
}

