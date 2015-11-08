#pragma once
#include "Hotspot.h"
#include "Preprocessor.h"

class CRoute
{
private:
	vector<CBase *> waypoints;  //·�߾����ĵ㣬����sink��һЩhotspot�����뱣֤��0��Ԫ����sink
	vector<int> coveredNodes;
	double length;
	//MANode������toPoint�ƶ���·��
	//vector<CBase *>::iterator fromPoint;
	int toPoint;
	bool overdue;  //�Ƿ����

public:
	CRoute(void)
	{
		length = 0;
		toPoint = 0;
		overdue = false;
	}
	CRoute(CBase *sink)
	{
		waypoints.push_back(sink);
		length = 0;
		toPoint = 0;  //��ʼ��Ϊsink
		overdue = false;
	}

	~CRoute(void)
	{}

	inline vector<CBase *> getWayPoints()
	{
		return waypoints;
	}
	inline vector<int> getCoveredNodes()
	{
		return coveredNodes;
	}

	inline CBase* getSink()
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
	//ȡ���ƶ�Ŀ�꣬����һ��point
	inline CBase* getToPoint()
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
	//��toPoint����һ��
	inline void updateToPoint()
	{
		toPoint = (toPoint + 1) % waypoints.size();
	}

	//��������Ԫ�طŵ�waypoint�б�����
	void AddPoint(CBase *hotspot)
	{
		waypoints.push_back(hotspot);
		addToListUniquely(coveredNodes, ((CHotspot *) hotspot)->getCoveredNodes());
	}
	//������hotspot���뵽·���и�����λ��
	void AddPoint(int front, CBase *hotspot)
	{
		vector<CBase *>::iterator ipoint = waypoints.begin() + front + 1;
		waypoints.insert(ipoint, hotspot);
		addToListUniquely(coveredNodes, ((CHotspot *) hotspot)->getCoveredNodes());
	}

	//�Ը����������·������
	double getAddingDistance(int front, CBase *hotspot)
	{
		int back = ( front + 1 ) % waypoints.size();
		double oldDistance =  CBase::getDistance( *waypoints[front], *waypoints[back] );
		double newDistance = CBase::getDistance( *waypoints[front], *hotspot) + CBase::getDistance(*hotspot, *waypoints[back] );
		return ( newDistance - oldDistance );
	}

	//����·�����ȣ�Ӧ����·�����Ļ����֮���ֶ������
	void updateLength();

	string toString();
};

