#pragma once

#include "Hotspot.h"
#include "Entity.h"

using namespace std;

class CRoute : 
	public CEntity
{
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
	CRoute(void)
	{
		init();
	}

	CRoute(CBasicEntity *sink)
	{
		init();
		waypoints.push_back(sink);
		toPoint = 0;  //��ʼ��Ϊsink
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
	//ȡ���ƶ�Ŀ�꣬����һ��point
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
	//��toPoint����һ��
	inline void updateToPoint()
	{
		toPoint = (toPoint + 1) % waypoints.size();
	}

	//��������Ԫ�طŵ�waypoint�б�����
	void AddPoint(CBasicEntity *hotspot)
	{
		waypoints.push_back(hotspot);
		addToListUniquely(coveredNodes, static_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}
	//������hotspot���뵽·���и�����λ��
	void AddPoint(int front, CBasicEntity *hotspot)
	{
		vector<CBasicEntity *>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, hotspot);
		addToListUniquely(coveredNodes, static_cast<CHotspot *>(hotspot)->getCoveredNodes());
	}

	//�Ը����������·������
	double getAddingDistance(int front, CBasicEntity *hotspot)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBasicEntity::getDistance( *waypoints[front], *waypoints[back] );
		double newDistance = CBasicEntity::getDistance( *waypoints[front], *hotspot) + CBasicEntity::getDistance(*hotspot, *waypoints[back] );
		return ( newDistance - oldDistance );
	}

	//����·�����ȣ�Ӧ����·�����Ļ����֮���ֶ������
	void updateLength();

};

