/***********************************************************************************************************************************

次级类 CBasicEntity ： （继承自顶级类 CEntity ）所有普通的实体类，应该继承自这个类；除非属于空间相关的实体，则应该继承自CGeoEntity

***********************************************************************************************************************************/

#pragma once

#ifndef __BASIC_ENTITY_H__
#define __BASIC_ENTITY_H__

#include "Global.h"
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
		ID(-1), time(-1), flag(false) {};

	virtual ~CBasicEntity() = 0
	{};

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

	inline static bool withinRange(CBasicEntity &m, CBasicEntity &n, double range)
	{
		return ( getDistance(m, n) <= range );
	}

	//static vector<CBasicEntity*> GetItemsByID(vector<CBasicEntity*> list, vector<int> ids);

	//由from向to方向移动，给定时间和速度，将更新时间戳和坐标为目的地或能到达的最远点
	//如果足够到达to位置，则返回大于等于 0 的剩余时间（精确到整数）；否则返回值小于 0
	int moveTo(CBasicEntity &to, int interval, double speed);

	//操作符重载，基于x坐标比较大小，用于position或hotspot间的排序
	friend bool operator == (const CBasicEntity &lt, const CBasicEntity &rt);
	friend bool operator != (const CBasicEntity &lt, const CBasicEntity &rt);
	friend bool operator < (const CBasicEntity &lt, const CBasicEntity &rt);
	friend bool operator > (const CBasicEntity &lt, const CBasicEntity &rt);
	
	virtual void updateStatus() {};
	virtual void updateStatus(int currentTime) {};

};
	
//CBasicEntity::~CBasicEntity(){};

#endif // __BASIC_ENTITY_H__
