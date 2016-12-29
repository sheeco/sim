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

	vector<CBasicEntity *> waypoints;  //路线经过的点，包括sink和一些hotspot，必须保证第0个元素是sink
	vector<int> coveredNodes;
	double length;
	//MANode正在向toPoint移动的路上
	//vector<CBasicEntity *>::iterator fromPoint;
	int toPoint;  //前往的点再数组waypoints中下标
	bool overdue;  //是否过期

	void init()
	{
		length = 0;
		toPoint = -1;
		overdue = false;		
	}


public:

	CRoute()
	{
		init();
	}

	CRoute(CBasicEntity *sink);

	~CRoute(){};

	inline vector<CBasicEntity *> getWayPoints() const
	{
		return waypoints;
	}
	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}

	inline bool isOverdue() const
	{
		return overdue;
	}
	inline void setOverdue(bool overdue)
	{
		this->overdue = overdue;
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
		return waypoints[toPoint];
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
	void AddHotspot(CBasicEntity *hotspot)
	{
		waypoints.push_back(hotspot);
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}
	//将给定hotspot插入到路径中给定的位置
	void AddHotspot(int front, CBasicEntity *hotspot)
	{
		vector<CBasicEntity *>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, hotspot);
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}

	//对给定插入计算路径增量
	double getIncreDistance(int front, CHotspot *hotspot)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBasicEntity::getDistance( *waypoints[front], *waypoints[back] );
		double newDistance = CBasicEntity::getDistance( *waypoints[front], *hotspot) + CBasicEntity::getDistance(*hotspot, *waypoints[back] );
		return ( newDistance - oldDistance );
	}

	//计算路径长度，应该在路径更改或更新之后手动后调用
	void updateLength();

	string toString() const
	{
		stringstream sstr;
		for( vector<CBasicEntity *>::const_iterator iwaypoint = waypoints.begin(); iwaypoint != waypoints.end(); iwaypoint++ )
		{
			sstr << ( *iwaypoint )->getLocation().toString() << TAB;
		}
		// e.g "0.0, 1.234	234.5, 345.6 ..."
		return sstr.str();
	}

};

#endif // __ROUTE_H__
