/***********************************************************************************************************************************

�μ��� CBasicEntity �� ���̳��Զ����� CEntity ��������ͨ��ʵ���࣬Ӧ�ü̳�������ࣻ�������ڿռ���ص�ʵ�壬��Ӧ�ü̳���CGeoEntity

***********************************************************************************************************************************/

#pragma once

#include "GlobalParameters.h"
#include "Coordinate.h"


//����λ�����ꡢʱ�����ID�Ļ���
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
	//�ֶ�����ID
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


	//���ID������Ϊ��Ϊinvalid
	inline void clear()
	{
		this->ID = -1;
	}

	//���������أ�����x����Ƚϴ�С������position��hotspot�������
	inline int CBasicEntity::operator==(CBasicEntity &it) const
	//FIXME:��������
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

	//������������
	inline static double getDistance(CBasicEntity &m, CBasicEntity &n)
	{
		double mx, my, nx, ny;
		mx = m.getX();
		my = m.getY();
		nx = n.getX();
		ny = n.getY();
		return sqrt((mx - nx) * (mx - nx) + (my - ny) * (my - ny));
	}

	//��from��to�����ƶ�������ʱ����ٶ�
	//����㹻����toλ�ã��򷵻ش��ڵ��� 0 ��ʣ��ʱ�䣨��ȷ�������������򷵻�ֵС�� 0
	int moveTo(CBasicEntity to, int time, double speed)
	{
		double fromX, fromY, toX, toY;
		fromX = this->getX();
		fromY = this->getY();
		toX = to.getX();
		toY = to.getY();

		double sin, cos, distance;
		distance = sqrt((fromX - toX) * (fromX - toX) + (fromY - toY) * (fromY - toY));

		//��δ����
		int timeArrival = distance / speed;
		if( timeArrival > time )
		{
			cos = (toX - fromX) / distance;
			sin = (toY - fromY) / distance;
			this->setLocation(fromX +  time * speed * cos, fromY + time * speed * sin);
			this->setTime( this->getTime() + time );
		}
		//������
		else
		{
			this->setLocation(toX, toY);			
			this->setTime( this->getTime() + timeArrival );
		}
		return ( time - timeArrival );
	}

};

