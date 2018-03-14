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


//注意：对double 型的坐标执行 == 操作符时存在精度问题
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

	//尚未到达
	int timeArrival = int(distance / speed);
	if(timeArrival > interval)
	{
		cos = ( toX - fromX ) / distance;
		sin = ( toY - fromY ) / distance;
		this->setLocation(fromX + interval * speed * cos, fromY + interval * speed * sin);
		this->setTime(this->getTime() + interval);
	}
	//将到达
	else
	{
		this->setLocation(toX, toY);
		this->setTime(this->getTime() + timeArrival);
	}
	return ( interval - timeArrival );
}