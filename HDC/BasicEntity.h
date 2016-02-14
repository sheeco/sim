#pragma once

#include "GlobalParameters.h"
#include "Entity.h"

using namespace std;

//包含位置坐标、时间戳、ID的基类
class CBasicEntity : 
	public CEntity
{
protected:
	int ID;
	double x;
	double y;
	int time;
	bool flag;

public:
	CBasicEntity(): 
		ID(0), x(0), y(0), time(0), flag(false) {};

	//setters & getters
	//手动设置ID
	inline void setID(int ID)  
	{
		this->ID = ID;
	}
	inline void setX(double x)
	{
		this->x = x;
	}
	inline void setY(double y)
	{		
		this->y = y;
	}
	inline void setTime(int time)
	{
		this->time = time;
	}
	inline double getX() const
	{
		return x;
	}
	inline double getY() const
	{
		return y;
	}
	inline int getID() const
	{
		return ID;
	}
	inline int getTime() const
	{
		return time;
	}

	inline void setLocation(double x, double y, int time)
	{
		this->x = x;
		this->y = y;
		this->time = time;
	}

	//清除ID，即视为置为invalid
	inline void clear()
	{
		this->ID = -1;
	}
	//virtual CString toString()const{};

	//操作符重载，基于x坐标比较大小，用于position或hotspot间的排序
	inline int CBasicEntity::operator==(CBasicEntity it) const
	//FIXME:精度问题
	{
		return (this->x == it.getX());
	}
	inline int CBasicEntity::operator!=(CBasicEntity it) const
	{
		return (this->x != it.getX());
	}
	inline int CBasicEntity::operator<(CBasicEntity it) const
	{
		return (this->x < it.getX());
	}
	inline int CBasicEntity::operator>(CBasicEntity it) const
	{
		return (this->x > it.getX());
	}
	inline void setFlag(bool flag)
	{
		this->flag = flag;
	}
	inline bool getFlag() const
	{
		return flag;
	}

	//返回两点间距离
	inline static double getDistance(CBasicEntity m, CBasicEntity n)
	{
		double mx, my, nx, ny;
		mx = m.getX();
		my = m.getY();
		nx = n.getX();
		ny = n.getY();
		return sqrt((mx - nx) * (mx - nx) + (my - ny) * (my - ny));
	}

	inline void moveTo(double x, double y, int t)
	{
		this->x = x;
		this->y = y;
		this->time = t;
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
			this->setX(fromX +  time * speed * cos);
			this->setY(fromY + time * speed * sin);
			this->setTime( this->getTime() + time );
		}
		//将到达
		else
		{
			this->setX(toX);
			this->setY(toY);			
			this->setTime( this->getTime() + timeArrival );
		}
		return ( time - timeArrival );
	}

};

