#include "Entity.h"


CCoordinate operator+ (const CCoordinate& lhs, const CCoordinate& rhs)
{
	return CCoordinate(lhs.getX() + rhs.getX(), lhs.getY() + rhs.getY());
}

CCoordinate operator- (const CCoordinate& lhs, const CCoordinate& rhs)
{
	return CCoordinate(lhs.getX() - rhs.getX(), lhs.getY() - rhs.getY());
}

CCoordinate operator* (const double ratio, const CCoordinate& hs)
{
	return CCoordinate(hs.getX() * ratio, hs.getY() * ratio);
}

CCoordinate operator* (const CCoordinate& hs, const double ratio)
{
	return CCoordinate(hs.getX() * ratio, hs.getY() * ratio);
}

CCoordinate operator/ (const CCoordinate& hs, const double ratio)
{
	return CCoordinate(hs.getX() / ratio, hs.getY() / ratio);
}


//ע�⣺��double �͵�����ִ�� == ������ʱ���ھ�������
bool operator == (const CSpatial &lt, const CSpatial &rt)
{
	return ( lt.getLocation().getX() == rt.getLocation().getX() );
}
bool operator != (const CSpatial &lt, const CSpatial &rt)
{
	return ( lt.getLocation().getX() != rt.getLocation().getX() );
}
bool operator < (const CSpatial &lt, const CSpatial &rt)
{
	return ( lt.getLocation().getX() < rt.getLocation().getX() );
}
bool operator > (const CSpatial &lt, const CSpatial &rt)
{
	return ( lt.getLocation().getX() > rt.getLocation().getX() );
}

int CBasicEntity::moveToward(CBasicEntity &toward, int interval, double speed)
{
	double fromX, fromY, toX, toY;
	fromX = this->getX();
	fromY = this->getY();
	toX = toward.getX();
	toY = toward.getY();

	double sin, cos, distance;
	distance = sqrt(( fromX - toX ) * ( fromX - toX ) + ( fromY - toY ) * ( fromY - toY ));

	//��δ����
	int timeArrival = int(distance / speed);
	if(timeArrival > interval)
	{
		cos = ( toX - fromX ) / distance;
		sin = ( toY - fromY ) / distance;
		this->setLocation(fromX + interval * speed * cos, fromY + interval * speed * sin);
		this->setTime(this->getTime() + interval);
	}
	//������
	else
	{
		this->setLocation(toX, toY);
		this->setTime(this->getTime() + timeArrival);
	}
	return ( interval - timeArrival );
}