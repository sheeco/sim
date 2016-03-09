#pragma once

#include "Hotspot.h"
#include "GeoEntity.h"


class CRoute : 
	public CGeoEntity
{
////ע�⣺CRoute������±���ʵ���ϲ���Ҳ��Ӧ�ñ�ʹ��
//
//protected:
//
//	double x;
//	double y;
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
		if(toPoint == -1)
			return nullptr;

		if( toPoint > waypoints.size() - 1 
			|| toPoint < 0  )
		{
			cout << endl << "Error @ CBasicEntity::getToPoint : toPoint exceeds the range " << endl;
			_PAUSE_;
		}
		return waypoints[toPoint];
	}
	//��toPoint����һ��
	inline void updateToPoint()
	{
		toPoint = (toPoint + 1) % waypoints.size();
	}

	//��������Ԫ�طŵ�waypoint�б�����
	void AddHotspot(CBasicEntity *hotspot)
	{
		waypoints.push_back(hotspot);
		AddToListUniquely(coveredNodes, static_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}
	//������hotspot���뵽·���и�����λ��
	void AddHotspot(int front, CBasicEntity *hotspot)
	{
		vector<CBasicEntity *>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, hotspot);
		AddToListUniquely(coveredNodes, static_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}

	//�Ը����������·������
	double getAddingDistance(int front, CHotspot *hotspot)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBasicEntity::getDistance( *waypoints[front], *waypoints[back] );
		double newDistance = CBasicEntity::getDistance( *waypoints[front], *hotspot) + CBasicEntity::getDistance(*hotspot, *waypoints[back] );
		return ( newDistance - oldDistance );
	}

	//����·�����ȣ�Ӧ����·�����Ļ����֮���ֶ������
	void updateLength();

};

