/***********************************************************************************************************************************

������ CEntity �� ����ʵ�����ʵ����Ļ��࣬Ӧ�����ռ̳��������

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

�μ��� CDecorator �� ����װ����Ļ���

***********************************************************************************************************************************/

class CDecorator :
	virtual public CEntity
{
public:

	CDecorator() {};
	virtual ~CDecorator() = 0 {};

};

/***********************************************************************************************************************************

�μ��� CGeoEntity �� ר���ڿռ���ص�ʵ�壬���пռ���ص�ʵ�壬Ӧ�ü̳��������

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

װ���� CUnique �� װ���ж�һ�޶� ID ��ʵ��

***********************************************************************************************************************************/

class CUnique :
	virtual public CDecorator
{
protected:

	int ID;

	CUnique() : ID(INVALID)
	{
	};

	//�Զ�����ID�����ֶ�����
	inline void generateID(int& generator)
	{
		if(ID != INVALID)
			throw string("CUnique::generateID(): ID has already been set.");
		this->ID = ++generator;
	}
	virtual void generateID() = 0;
	//�ֶ�����ID
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

CCoordinate �� ��άλ������

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
	//������������
	inline static double getDistance(CCoordinate m, CCoordinate n)
	{
		double mx, my, nx, ny;
		mx = m.getX();
		my = m.getY();
		nx = n.getX();
		ny = n.getY();
		return sqrt(( mx - nx ) * ( mx - nx ) + ( my - ny ) * ( my - ny ));
	}

	// ��������������
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
	//������������
	inline static double getDistance(CSpatial &m, CSpatial &n)
	{
		return CCoordinate::getDistance(m.getLocation(), n.getLocation());
	}

	inline static bool withinRange(CSpatial &m, CSpatial &n, double range)
	{
		return ( getDistance(m, n) <= range );
	}

	//���������أ�����x����Ƚϴ�С������position��hotspot�������
	friend bool operator == (const CSpatial &lt, const CSpatial &rt);
	friend bool operator != (const CSpatial &lt, const CSpatial &rt);
	friend bool operator < (const CSpatial &lt, const CSpatial &rt);
	friend bool operator > (const CSpatial &lt, const CSpatial &rt);

};

/***********************************************************************************************************************************

�μ��� CBasicEntity �� ������ͨ��ʵ���࣬Ӧ�ü̳�������ࣻ
�������ڿռ���ص�ʵ�壬��Ӧ�ü̳���CGeoEntity

***********************************************************************************************************************************/

//����λ�����ꡢʱ�����ID�Ļ���
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

	//��from��to�����ƶ�������ʱ����ٶȣ�������ʱ���������ΪĿ�ĵػ��ܵ������Զ��
	//����㹻����toλ�ã��򷵻ش��ڵ��� 0 ��ʣ��ʱ�䣨��ȷ�������������򷵻�ֵС�� 0
	int moveToward(CBasicEntity &toward, int interval, double speed);


};


#endif // __ENTITY_H__
