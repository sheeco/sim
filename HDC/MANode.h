//#pragma once
//
//#include "Data.h"
//#include "Route.h"
//#include "Sink.h"
//#include "GeneralNode.h"
//
//using namespace std;
//
//class CMANode :
//	public CGeneralNode
//{
////protected:
////	int ID;  //MA�ڵ�ı��
////	double x;  //MA�ڵ����ڵ�x����
////	double y;  //MA�ڵ����ڵ�y����
////	int time;  //����MA�ڵ������ʱ���
////	bool flag;
//
//private:
//	vector<CData> buffer;
//	CRoute route;
//	//bool atSink;  //MA�Ƿ񵽴���Sink
//	CHotspot *atHotspot;  //MA�����hotspot
//	int waitingTime;  //�ڵ�ǰλ�õ�ʣ��waitingʱ��
//
//	static double energyConsumption;
//	static int ID_COUNT;
//
//	static int encounter;
//	static int encounterAtHotspot;
//	static int encounterOnRoute;
//
//public:
//	CMANode(void)
//	{
//	}
//
//	CMANode(CRoute route, int time)
//	{
//		this->route = route;
//		this->x = SINK_X;
//		this->y = SINK_Y;
//		this->time = time;
//		//atSink = true;
//		atHotspot = NULL;
//		waitingTime = -1;
//		generateID();
//	}
//
//	~CMANode(void){}
//
//	//�Զ�����ID�����ֶ�����
//	inline void generateID()
//	{
//		if(this->ID != -1)
//			return;
//		this->ID = ID_COUNT;
//		ID_COUNT++;
//	}
//
//	static inline double getEnergyConsumption()
//	{
//		return energyConsumption;
//	}
//	inline int getBufferSize()
//	{
//		return buffer.size();
//	}
//	inline CRoute* getRoute()
//	{
//		return &route;
//	}
//	inline void setRoute(CRoute route)
//	{
//		this->route = route;
//	}
//	inline bool routeIsOverdue()
//	{
//		return route.isOverdue();
//	}
//	inline void setRouteOverdue(bool overdue)
//	{
//		this->route.setOverdue(overdue);
//	}
//	//inline void setAtSink(bool atSink)
//	//{
//	//	this->atSink = atSink;
//	//}
//	inline void setAtHotspot(CHotspot *atHotspot)
//	{
//		this->atHotspot = atHotspot;
//	}
//	
//	//�ж�MA�Ƿ�λ��sink��
//	//inline bool isAtSink()
//	//{
//	//	return atSink;
//	//}
//
//	inline bool isAtHotspot()
//	{
//		if(atHotspot == NULL)
//			return false;
//		else
//			return true;
//	}
//	inline CHotspot* getAtHotspot()
//	{
//		return atHotspot;
//	}
//	inline void setWaitingTime(int waitingTime)
//	{
//		this->waitingTime = waitingTime;
//	}
//	inline int getWaitingTime()
//	{
//		return waitingTime;
//	}
//
//	//�ж�Buffer�Ƿ�����
//	inline bool isFull()
//	{
//		if(buffer.size() >= BUFFER_CAPACITY_MA)
//			return true;
//		else
//			return false;
//	}	
//	//buffer����ռ��С
//	inline int getBufferCapacity()
//	{
//		if(buffer.size() >= BUFFER_CAPACITY_MA)
//			return 0;
//		else
//			return ( BUFFER_CAPACITY_MA - buffer.size() );
//	}
//	//��������
//	inline static double getEncounterPercentAtHotspot()
//	{
//		return (double)encounterAtHotspot / (double)encounter;
//	}
//	inline static int getEncounter()
//	{
//		return encounter;
//	}
//	inline static int getEncounterAtHotspot()
//	{
//		return encounterAtHotspot;
//	}
//
//	//���ڼ�¼MA�ڵ���sensor����������
//	inline static void encountAtHotspot()
//	{
//		encounterAtHotspot++;
//		encounter++;
//	}
//	inline static void encountOnRoute()
//	{
//		encounterOnRoute++;
//		encounter++;
//	}
//
//	//����data
//	void receiveData(int time, vector<CData> data);
//
//	//��������data
//	vector<CData> sendAllData();
//
//	//MA�ƶ�������timeʱ�̵�λ��
//	void updateLocation(int time);
//
//};
//
