#pragma once

#include "Hotspot.h"
#include "Entity.h"

using namespace std;

class CRoute : 
	public CEntity
{
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
	CRoute(void)
	{
		init();
	}

	CRoute(CBasicEntity *sink)
	{
		init();
		waypoints.push_back(sink);
		toPoint = 0;  //初始化为sink
	}

	~CRoute(void)
	{}

	inline vector<CBasicEntity *> getWayPoints() const
	{
		return waypoints;
	}
	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}

	inline CBasicEntity* getSink()
	{
		if(waypoints.empty())
			return nullptr;
		else if(waypoints[0]->getID() != SINK_ID)
		{
			cout << endl << "Error @ CRoute::getSink() : waypoints[0] is not sink"<<endl;
			_PAUSE;
			return nullptr;
		}
		else
			return waypoints[0];
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
		if(toPoint == -1)
			return nullptr;

		if( toPoint > waypoints.size() - 1 
			|| toPoint < 0  )
		{
			cout << endl << "Error @ CBasicEntity::getToPoint : toPoint exceeds the range " << endl;
			_PAUSE;
		}
		return waypoints[toPoint];
	}
	//将toPoint后移一个
	inline void updateToPoint()
	{
		toPoint = (toPoint + 1) % waypoints.size();
	}

	//将给定的元素放到waypoint列表的最后
	void AddPoint(CBasicEntity *hotspot)
	{
		waypoints.push_back(hotspot);
		addToListUniquely(coveredNodes, static_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}
	//将给定hotspot插入到路径中给定的位置
	void AddPoint(int front, CBasicEntity *hotspot)
	{
		vector<CBasicEntity *>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, hotspot);
		addToListUniquely(coveredNodes, static_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}

	//对给定插入计算路径增量
	double getAddingDistance(int front, CBasicEntity *hotspot)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBasicEntity::getDistance( *waypoints[front], *waypoints[back] );
		double newDistance = CBasicEntity::getDistance( *waypoints[front], *hotspot) + CBasicEntity::getDistance(*hotspot, *waypoints[back] );
		return ( newDistance - oldDistance );
	}

	//计算路径长度，应该在路径更改或更新之后手动后调用
	void updateLength();

};

