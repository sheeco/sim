/***********************************************************************************************************************************

次级类 CBasicEntity ： （继承自顶级类 CEntity ）所有普通的实体类，应该继承自这个类；除非属于空间相关的实体，则应该继承自CGeoEntity

***********************************************************************************************************************************/

#pragma once

#include "GlobalParameters.h"
#include "Coordinate.h"


//包含位置坐标、时间戳、ID的基类
class CBasicEntity : 
	virtual public CEntity
{
protected:

	int ID;
	CCoordinate location; 
	int time;
	bool flag;


public:

	CBasicEntity(): 
		ID(-1), time(0), flag(false) {};

	~CBasicEntity(){};

	//setters & getters
	//手动设置ID
	inline void setID(int ID)  
	{
		this->ID = ID;
	}
	inline int getID() const
	{
		return ID;
	}
	inline void setLocation(double x, double y)
	{		
		this->location.setX(x);
		this->location.setY(y);
	}
	inline void setLocation(CCoordinate location)
	{		
		this->location = location;
	}
//	inline void setLocation(double x, double y, int time)
//	{
//		setLocation(x, y);
//		this->time = time;
//	}
	inline void setLocation(CCoordinate location, int time)
	{		
		this->location = location;
		this->time = time;
	}
	inline double getX() const
	{
		return location.getX();
	}
	inline double getY() const
	{
		return location.getY();
	}
	inline CCoordinate getLocation() const
	{
		return location;
	}
	inline void setTime(int time)
	{
		this->time = time;
	}
	inline int getTime() const
	{
		return time;
	}
	inline void setFlag(bool flag)
	{
		this->flag = flag;
	}
	inline bool getFlag() const
	{
		return flag;
	}


	//清除ID，即视为置为invalid
	inline void clear()
	{
		this->ID = -1;
	}

	//操作符重载，基于x坐标比较大小，用于position或hotspot间的排序
	inline int CBasicEntity::operator==(CBasicEntity &it) const
	//FIXME:精度问题
	{
		return (location.getX() == it.getX());
	}
	inline int CBasicEntity::operator!=(CBasicEntity &it) const
	{
		return (location.getX() != it.getX());
	}
	inline int CBasicEntity::operator<(CBasicEntity &it) const
	{
		return (location.getX() < it.getX());
	}
	inline int CBasicEntity::operator>(CBasicEntity &it) const
	{
		return (location.getX() > it.getX());
	}

	//返回两点间距离
	inline static double getDistance(CBasicEntity &m, CBasicEntity &n)
	{
		double mx, my, nx, ny;
		mx = m.getX();
		my = m.getY();
		nx = n.getX();
		ny = n.getY();
		return sqrt((mx - nx) * (mx - nx) + (my - ny) * (my - ny));
	}

	//由from向to方向移动，给定时间和速度
	//如果足够到达to位置，则返回大于等于 0 的剩余时间（精确到整数）；否则返回值小于 0
	int moveTo(CBasicEntity to, int time, double speed)
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

};

