/***********************************************************************************************************************************

�μ��� CBasicEntity �� ���̳��Զ����� CEntity ��������ͨ��ʵ���࣬Ӧ�ü̳�������ࣻ�������ڿռ���ص�ʵ�壬��Ӧ�ü̳���CGeoEntity

***********************************************************************************************************************************/

#pragma once

#ifndef __BASIC_ENTITY_H__
#define __BASIC_ENTITY_H__

#include "Global.h"
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
		ID(-1), time(-1), flag(false) {};

	virtual ~CBasicEntity() = 0
	{};

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

	inline static bool withinRange(CBasicEntity &m, CBasicEntity &n, double range)
	{
		return ( getDistance(m, n) <= range );
	}

	//static vector<CBasicEntity*> GetItemsByID(vector<CBasicEntity*> list, vector<int> ids);

	//��from��to�����ƶ�������ʱ����ٶȣ�������ʱ���������ΪĿ�ĵػ��ܵ������Զ��
	//����㹻����toλ�ã��򷵻ش��ڵ��� 0 ��ʣ��ʱ�䣨��ȷ�������������򷵻�ֵС�� 0
	int moveTo(CBasicEntity &to, int interval, double speed);

	//���������أ�����x����Ƚϴ�С������position��hotspot�������
	friend bool operator == (const CBasicEntity &lt, const CBasicEntity &rt);
	friend bool operator != (const CBasicEntity &lt, const CBasicEntity &rt);
	friend bool operator < (const CBasicEntity &lt, const CBasicEntity &rt);
	friend bool operator > (const CBasicEntity &lt, const CBasicEntity &rt);
	
	virtual void updateStatus() {};
	virtual void updateStatus(int currentTime) {};

};
	
//CBasicEntity::~CBasicEntity(){};

#endif // __BASIC_ENTITY_H__
