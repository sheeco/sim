#pragma once

#include "Hotspot.h"
#include "Preprocessor.h"
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
	int toPoint;
	bool overdue;  //是否过期

public:
	CRoute(void)
	{
		length = 0;
		toPoint = 0;
		overdue = false;
	}
	CRoute(CBasicEntity *sink)
	{
		waypoints.push_back(sink);
		length = 0;
		toPoint = 0;  //初始化为sink
		overdue = false;
	}

	~CRoute(void)
	{}

	inline vector<CBasicEntity *> getWayPoints()
	{
		return waypoints;
	}
	inline vector<int> getCoveredNodes()
	{
		return coveredNodes;
	}

	inline CBasicEntity* getSink()
	{
		if(waypoints.empty())
			return NULL;
		else if(waypoints[0]->getID() != SINK_ID)
		{
			cout<<"Error: CRoute::getSink() waypoints[0] is not sink"<<endl;
			_PAUSE;
			return NULL;
		}
		else
			return waypoints[0];
	}

	inline bool isOverdue()
	{
		return overdue;
	}
	inline void setOverdue(bool overdue)
	{
		this->overdue = overdue;
	}
	inline int getNWayPoints()
	{
		return waypoints.size();
	}
	inline double getLength()
	{
		return length;
	}
	//取得移动目标，即下一个point
	inline CBasicEntity* getToPoint()
	{
		if( toPoint > waypoints.size() - 1 
			|| toPoint < 0 
			|| ( ( waypoints[toPoint]->getID() != SINK_ID ) && ( ( (CHotspot *)waypoints[toPoint] )->getCandidateType() > 3 ) ) )
		{
			cout << "Error: toPoint exceeds the range " << endl;
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
		addToListUniquely(coveredNodes, ((CHotspot *) hotspot)->getCoveredNodes());
	}
	//将给定hotspot插入到路径中给定的位置
	void AddPoint(int front, CBasicEntity *hotspot)
	{
		vector<CBasicEntity *>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, hotspot);
		addToListUniquely(coveredNodes, ((CHotspot *) hotspot)->getCoveredNodes());
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

	string toString();
};

