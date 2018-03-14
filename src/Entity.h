/***********************************************************************************************************************************

顶级类 CEntity ： 所有实体类或实体类的基类，应该最终继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Global.h"


class CEntity
{
public:

	CEntity(){};
	virtual ~CEntity() = 0
	{};

	virtual string toString()
	{
		return format(true);
	};
	virtual string format(bool brief)
	{
		return "";
	};

};

/***********************************************************************************************************************************

次级类 CDecorator ： 所有装饰类的基类

***********************************************************************************************************************************/

class CDecorator :
	virtual public CEntity
{
public:

	CDecorator() {};
	virtual ~CDecorator() = 0 {};

};

/***********************************************************************************************************************************

次级类 CGeoEntity ： 专用于空间相关的实体，所有空间相关的实体，应该继承自这个类

***********************************************************************************************************************************/

class CGeoEntity :
	virtual public CEntity
{
public:

	CGeoEntity() {};
	virtual ~CGeoEntity() = 0
	{};

};

/***********************************************************************************************************************************

装饰类 CUnique ： 装饰有独一无二 ID 的实体

***********************************************************************************************************************************/

class CUnique :
	virtual public CDecorator
{
protected:

	int ID;

	CUnique() : ID(INVALID)
	{
	};

	//自动生成ID，需手动调用
	inline void generateID(int& generator)
	{
		if(ID != INVALID)
			throw string("CUnique::generateID(): ID has already been set.");
		this->ID = ++generator;
	}
	virtual void generateID() = 0;
	//手动设置ID
	inline void setID(int ID)
	{
		this->ID = ID;
	}

public:

	virtual ~CUnique() = 0
	{
	};
	inline int getID() const
	{
		return ID;
	}
};

/***********************************************************************************************************************************

CCoordinate ： 二维位置坐标

***********************************************************************************************************************************/

class CCoordinate :
	virtual public CGeoEntity
{
protected:

	double x;
	double y;


public:

	CCoordinate() :
		x(0), y(0)
	{
	};

	CCoordinate(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	~CCoordinate()
	{
	};

	inline double getX() const
	{
		return x;
	}
	inline double getY() const
	{
		return y;
	}
	inline void setX(double x)
	{
		this->x = x;
	}
	inline void setY(double y)
	{
		this->y = y;
	}
	inline string toString() const
	{
		string xstr = STRING(this->x);
		string ystr = STRING(this->y);
		return "(" + xstr + ", " + ystr + ")";
	}
	//返回两点间距离
	inline static double getDistance(CCoordinate m, CCoordinate n)
	{
		double mx, my, nx, ny;
		mx = m.getX();
		my = m.getY();
		nx = n.getX();
		ny = n.getY();
		return sqrt(( mx - nx ) * ( mx - nx ) + ( my - ny ) * ( my - ny ));
	}

	// 算术操作符重载
	friend CCoordinate operator+ (const CCoordinate& lhs, const CCoordinate& rhs);
	friend CCoordinate operator- (const CCoordinate& lhs, const CCoordinate& rhs);
	friend CCoordinate operator* (const CCoordinate& lhs, const double ratio);
	friend CCoordinate operator* (const double ratio, const CCoordinate& lhs);
	friend CCoordinate operator/ (const CCoordinate& lhs, const double ratio);

};

class CSpatial :
	virtual public CDecorator
{
protected:

	CCoordinate location;

public:
	CSpatial() { };
	virtual ~CSpatial() = 0 { };

	inline void setLocation(double x, double y)
	{
		this->setLocation(CCoordinate(x, y));
	}
	inline void setLocation(CCoordinate location)
	{
		this->location = location;
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
	//返回两点间距离
	inline static double getDistance(CSpatial &m, CSpatial &n)
	{
		return CCoordinate::getDistance(m.getLocation(), n.getLocation());
	}

	inline static bool withinRange(CSpatial &m, CSpatial &n, double range)
	{
		return ( getDistance(m, n) <= range );
	}

	//操作符重载，基于x坐标比较大小，用于position或hotspot间的排序
	friend bool operator == (const CSpatial &lt, const CSpatial &rt);
	friend bool operator != (const CSpatial &lt, const CSpatial &rt);
	friend bool operator < (const CSpatial &lt, const CSpatial &rt);
	friend bool operator > (const CSpatial &lt, const CSpatial &rt);

};

/***********************************************************************************************************************************

次级类 CBasicEntity ： 所有普通的实体类，应该继承自这个类；
除非属于空间相关的实体，则应该继承自CGeoEntity

***********************************************************************************************************************************/

//包含位置坐标、时间戳、ID的基类
class CBasicEntity :
	virtual public CEntity, virtual public CSpatial
{
protected:

	int time;
	bool flag;


public:

	CBasicEntity() :
		time(INVALID), flag(false)
	{
	};

	virtual ~CBasicEntity() = 0
	{
	};

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
	//inline void setLocation(CCoordinate location, int time)
	//{
	//	this->location = location;
	//	this->time = time;
	//}

	//由from向to方向移动，给定时间和速度，将更新时间戳和坐标为目的地或能到达的最远点
	//如果足够到达to位置，则返回大于等于 0 的剩余时间（精确到整数）；否则返回值小于 0
	int moveToward(CBasicEntity &toward, int interval, double speed);


};


#endif // __ENTITY_H__
