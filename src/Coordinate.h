#pragma once

#ifndef __COORDINATE_H__
#define __COORDINATE_H__

#include "Global.h"
#include "GeoEntity.h"


class CCoordinate :
	virtual public CGeoEntity
{
protected:

	double x;
	double y;


public:
	
	CCoordinate(): 
		x(0), y(0) {};

	CCoordinate(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	~CCoordinate(){};

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
	inline string format() const
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

#endif // __COORDINATE_H__
