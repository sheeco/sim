#pragma once

#include "GeoEntity.h"


class CCoordinate :
	public CGeoEntity
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

	// ËãÊõ²Ù×÷·ûÖØÔØ
	friend CCoordinate operator+ (const CCoordinate& lhs, const CCoordinate& rhs);
	friend CCoordinate operator- (const CCoordinate& lhs, const CCoordinate& rhs);
	friend CCoordinate operator* (const CCoordinate& lhs, const double ratio);
	friend CCoordinate operator* (const double ratio, const CCoordinate& lhs);
	friend CCoordinate operator/ (const CCoordinate& lhs, const double ratio);

};

