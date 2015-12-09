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
	CBasicEntity()
	{
		ID = -1;
		x = 0;
		y = 0;
		time = -1;
		flag = false;
	}

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
	inline double getX()
	{
		return x;
	}
	inline double getY()
	{
		return y;
	}
	inline int getID()
	{
		return ID;
	}
	inline int getTime()
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
	inline int CBasicEntity::operator==(CBasicEntity it)  //FIXME:精度问题
	{
		return (this->x == it.getX());
	}
	inline int CBasicEntity::operator!=(CBasicEntity it)
	{
		return (this->x != it.getX());
	}
	inline int CBasicEntity::operator<(CBasicEntity it)
	{
		return (this->x < it.getX());
	}
	inline int CBasicEntity::operator>(CBasicEntity it)
	{
		return (this->x > it.getX());
	}
	inline void setFlag(bool flag)
	{
		this->flag = flag;
	}
	inline bool getFlag()
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

	//由from向to方向移动，给定时间和速度
	static void moveTo(CBasicEntity &from, CBasicEntity to, int time, double speed)
	{
		double fromX, fromY, toX, toY;
		fromX = from.getX();
		fromY = from.getY();
		toX = to.getX();
		toY = to.getY();

		double sin, cos, distance;
		distance = sqrt((fromX - toX) * (fromX - toX) + (fromY - toY) * (fromY - toY));
		cos = (toX - fromX) / distance;
		sin = (toY - fromY) / distance;
		from.setX(fromX +  time * speed * cos);
		from.setY(fromY + time * speed * sin);
	}

	//重载操作符==用于根据ID判断identical
	bool operator == (int id)
	{
		return this->ID == id;
	}

};

