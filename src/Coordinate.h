#pragma once

#include "Entity.h"


class CCoordinate :
	public CEntity
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

};

