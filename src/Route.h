#pragma once

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "Hotspot.h"
#include "GeoEntity.h"


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

	vector<CBasicEntity *> waypoints;  //·�߾����ĵ㣬����sink��һЩhotspot�����뱣֤��0��Ԫ����sink
	vector<int> coveredNodes;
	double length;
	//MANode������toPoint�ƶ���·��
	//vector<CBasicEntity *>::iterator fromPoint;
	int toPoint;  //ǰ���ĵ�������waypoints���±�
	bool overdue;  //�Ƿ����

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
	//ȡ���ƶ�Ŀ�꣬����һ��point
	inline CBasicEntity* getToPoint()
	{
		if( toPoint > waypoints.size() - 1 
			|| toPoint < 0  )
		{
			throw string("CBasicEntity::getToPoint() : toPoint exceeds the index range of waypoints ");
		}
		return waypoints[toPoint];
	}
	//��toPoint����һ��
	inline void updateToPoint()
	{
		toPoint = (toPoint + 1) % waypoints.size();
	}
	//��toPoint����sink
	inline void updateToPointWithSink()
	{
		toPoint = 0;
	}

	//��������Ԫ�طŵ�waypoint�б�����
	void AddHotspot(CBasicEntity *hotspot)
	{
		waypoints.push_back(hotspot);
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}
	//������hotspot���뵽·���и�����λ��
	void AddHotspot(int front, CBasicEntity *hotspot)
	{
		vector<CBasicEntity *>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, hotspot);
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}

	//�Ը����������·������
	double getIncreDistance(int front, CHotspot *hotspot)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBasicEntity::getDistance( *waypoints[front], *waypoints[back] );
		double newDistance = CBasicEntity::getDistance( *waypoints[front], *hotspot) + CBasicEntity::getDistance(*hotspot, *waypoints[back] );
		return ( newDistance - oldDistance );
	}

	//����·�����ȣ�Ӧ����·�����Ļ����֮���ֶ������
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
