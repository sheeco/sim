#pragma once

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "Entity.h"


class CRoute : 
	virtual public CGeoEntity
{
////ע�⣺CRoute������±���ʵ���ϲ���Ҳ��Ӧ�ñ�ʹ��
//
//protected:
//
//	CCoordinate location;
//	int time;


private:

	vector<pair<CBasicEntity *, int>> waypoints;  //·�߾����ĵ㣬����sink��һЩhotspot�����뱣֤��0��Ԫ����sink
	double length;
	//MANode������toPoint�ƶ���·��
	//vector<CBasicEntity *>::iterator fromPoint;
	int toPoint;  //ǰ���ĵ�������waypoints���±�

protected:

	void init()
	{
		length = 0;
		toPoint = -1;
	}

	void setStartPoint(CBasicEntity *startPoint)
	{
		this->waypoints.clear();
		this->AddWaypoint(startPoint, 0);
		this->toPoint = 0;
	}
	CRoute()
	{
		this->init();
	}


public:

	CRoute(CBasicEntity *startPoint)
	{
		this->init();
		this->setStartPoint(startPoint);
	}

	~CRoute(){};

	inline vector<pair<CBasicEntity *, int>> getWayPoints() const
	{
		return waypoints;
	}
	inline int getNWayPoints() const
	{
		return waypoints.size();
	}
	inline double getLength() const
	{
		return length;
	}
	//ȡ���ƶ�Ŀ�꣬����һ��point
	inline CBasicEntity* getToPoint()
	{
		if( toPoint > waypoints.size() - 1 
			|| toPoint < 0  )
		{
			throw string("CBasicEntity::getToPoint() : toPoint exceeds the index range of waypoints ");
		}
		return waypoints[toPoint].first;
	}
	inline int getWaitingTime()
	{
		if( toPoint > waypoints.size() - 1
		   || toPoint < 0 )
		{
			throw string("CBasicEntity::getToPoint() : toPoint exceeds the index range of waypoints ");
		}
		return waypoints[toPoint].second;
	}
	inline void initToPoint()
	{
		if(this->waypoints.size() < 2 )
			this->toPoint = INVALID;
		else
			this->toPoint = 1;
	}
	//��toPoint����һ��
	inline void updateToPoint()
	{
		toPoint = (toPoint + 1) % waypoints.size();
	}

	//��������Ԫ�طŵ�waypoint�б�����
	void AddWaypoint(CBasicEntity *waypoint, int waitingTime)
	{
		waypoints.push_back(pair<CBasicEntity*, int>(waypoint, waitingTime));
	}
	//���뵽·���и�����λ��
	void AddWaypoint(int front, CBasicEntity *waypoint, int waitingTime)
	{
		vector<pair<CBasicEntity *, int>>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, pair<CBasicEntity*, int>(waypoint, waitingTime));
	}

	//�Ը����������·������
	double getIncreDistance(int front, CBasicEntity *waypoint)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBasicEntity::getDistance( *waypoints[front].first, *waypoints[back].first);
		double newDistance = CBasicEntity::getDistance( *waypoints[front].first, *waypoint) + CBasicEntity::getDistance(*waypoint, *waypoints[back].first);
		return ( newDistance - oldDistance );
	}

	//����·�����ȣ�Ӧ����·�����Ļ����֮���ֶ������
	void updateLength();

	string toString() override
	{
		stringstream sstr;
		sstr << "[";
		for( vector<pair<CBasicEntity *, int>>::const_iterator iwaypoint = waypoints.begin(); iwaypoint != waypoints.end(); )
		{
			sstr << iwaypoint->first->getLocation().toString();
			++iwaypoint;
			if(iwaypoint != waypoints.end())
				sstr << " -> ";
		}
		sstr << "]";
		// e.g "0.0, 1.234	234.5, 345.6 ..."
		return sstr.str();
	}

};

#endif // __ROUTE_H__
