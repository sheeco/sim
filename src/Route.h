#pragma once

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "Hotspot.h"
#include "GeoEntity.h"


class CRoute : 
	virtual public CGeoEntity
{
////注意：CRoute类的以下变量实际上不会也不应该被使用
//
//protected:
//
//	CCoordinate location;
//	int time;


private:

	vector<pair<CBasicEntity *, int>> waypoints;  //路线经过的点，包括sink和一些hotspot，必须保证第0个元素是sink
	double length;
	//MANode正在向toPoint移动的路上
	//vector<CBasicEntity *>::iterator fromPoint;
	int toPoint;  //前往的点再数组waypoints中下标

	void init()
	{
		length = 0;
		toPoint = -1;
	}

protected:

	void setStartPoint(CBasicEntity *startPoint)
	{
		this->AddWaypoint(startPoint, 0);
		this->toPoint = startPoint->getID();
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
	//取得移动目标，即下一个point
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
	//将toPoint后移一个
	inline void updateToPoint()
	{
		toPoint = (toPoint + 1) % waypoints.size();
	}
	//将toPoint置于sink
	inline void updateToPointWithSink()
	{
		toPoint = 0;
	}

	//将给定的元素放到waypoint列表的最后
	void AddWaypoint(CBasicEntity *waypoint, int waitingTime)
	{
		waypoints.push_back(pair<CBasicEntity*, int>(waypoint, waitingTime));
	}
	//插入到路径中给定的位置
	void AddWaypoint(int front, CBasicEntity *waypoint, int waitingTime)
	{
		vector<pair<CBasicEntity *, int>>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, pair<CBasicEntity*, int>(waypoint, waitingTime));
	}

	//对给定插入计算路径增量
	double getIncreDistance(int front, CHotspot *hotspot)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBasicEntity::getDistance( *waypoints[front].first, *waypoints[back].first);
		double newDistance = CBasicEntity::getDistance( *waypoints[front].first, *hotspot) + CBasicEntity::getDistance(*hotspot, *waypoints[back].first);
		return ( newDistance - oldDistance );
	}

	//计算路径长度，应该在路径更改或更新之后手动后调用
	void updateLength();

	string format() override
	{
		stringstream sstr;
		sstr << "[";
		for( vector<pair<CBasicEntity *, int>>::const_iterator iwaypoint = waypoints.begin(); iwaypoint != waypoints.end(); )
		{
			sstr << iwaypoint->first->getLocation().format();
			++iwaypoint;
			if(iwaypoint != waypoints.end())
				sstr << ", ";
		}
		sstr << "]";
		// e.g "0.0, 1.234	234.5, 345.6 ..."
		return sstr.str();
	}

};

#endif // __ROUTE_H__
