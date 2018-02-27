#pragma once

#ifndef __HAR_H__
#define __HAR_H__

#include "Hotspot.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "MANode.h"
#include "Route.h"
#include "RoutingProtocol.h"


class CHARRoute : 
	virtual public CRoute
{
protected:
	friend class CHARMANode;
	friend class HAR;

	vector<int> coveredNodes;
	bool overdue;  //�Ƿ����

	void init()
	{
		overdue = false;
	}
	CHARRoute()
	{
		this->init();
		this->setStartPoint(CSink::getSink());
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
	//��������Ԫ�طŵ�waypoint�б�����
	void AddHotspot(CBasicEntity *hotspot)
	{
		//TODO: rfct
		AddWaypoint(hotspot, getConfig<int>("har", "min_waiting_time"));
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>( hotspot )->getCoveredNodes());
	}
	//������hotspot���뵽·���и�����λ��
	void AddHotspot(int front, CBasicEntity *hotspot)
	{
		AddWaypoint(front, hotspot, getConfig<int>("har", "min_waiting_time"));
		AddToListUniquely(coveredNodes, dynamic_cast<CHotspot *>( hotspot )->getCoveredNodes());
	}


};

class CHARNode : 
	virtual public CNode
{
	//UNDONE:
};

class CHARMANode : 
	virtual public CMANode
{
protected:
	friend class HAR;
	friend class CMacProtocol;

	//	static double energyConsumption;
	static vector<CHARMANode *> allMANodes;
	static vector<CHARMANode *> busyMANodes;
	static vector<CHARMANode *> freeMANodes;


	CHARMANode(CHARRoute route, int time)
	{
		this->setRoute(new CHARRoute(route));
	}
	//��ʱ����
	void turnFree()
	{
		for( auto iMANode = busyMANodes.begin(); iMANode != busyMANodes.end(); ++iMANode )
		{
			if( ( *iMANode ) == this )
			{
				busyMANodes.erase(iMANode);
				break;
			}
		}
		freeMANodes.push_back(this);
	}

	static vector<CHARMANode *>& getBusyMANodes()
	{
		return busyMANodes;
	}

	static vector<CHARMANode *>& getAllMANodes()
	{
		return allMANodes;
	}

	static inline double getSumEnergyConsumption()
	{
		double sumEnergyConsumption = 0;
		for( auto iMANode = busyMANodes.begin(); iMANode != busyMANodes.end(); ++iMANode )
			sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();
		for( auto iMANode = freeMANodes.begin(); iMANode != freeMANodes.end(); ++iMANode )
			sumEnergyConsumption += ( *iMANode )->getEnergyConsumption();

		return sumEnergyConsumption;
	}
	inline CHARRoute* getHARRoute() const
	{
		return dynamic_cast< CHARRoute* >( this->route );
	}
	inline CHotspot* getAtHotspot() const
	{
		return dynamic_cast<CHotspot*>( getAtWaypoint() );
	}
	inline bool routeIsOverdue() const
	{
		return getHARRoute()->isOverdue();
	}
	inline void setRouteOverdue(bool overdue)
	{
		this->getHARRoute()->setOverdue(overdue);
	}

	void updateStatus(int time);

	vector<CData> bufferData(int time, vector<CData> datas)
	{
		vector<CData> ack = datas;
		RemoveFromList(datas, this->buffer);

		bool atPoint = isAtWaypoint();
		if( atPoint )
		{
			this->getAtHotspot()->recordCountDelivery(datas.size());
			CData::deliverAtWaypoint(datas.size());
		}
		else
			CData::deliverOnRoute(datas.size());

		for( auto idata = datas.begin(); idata != datas.end(); ++idata )
		{
			////��Ϊ���� MA �ڵ㼴���� sink
			//idata->arriveSink(time);
			this->buffer.push_back(*idata);
		}

		return ack;
	}

	//��ǰ�MA��������ʱ���ã����������õ�MA�����µ�MA
	static CHARMANode* newMANode(CHARRoute route, int time)
	{
		CHARMANode *result = nullptr;

		//�����µ�MA
		if( freeMANodes.empty() )
		{
			result = new CHARMANode(route, time);
		}
		//ʹ�����õ�MA
		else
		{
			result = freeMANodes[0];
			freeMANodes.erase(freeMANodes.begin());
			result->updateRoute(route);
		}
		busyMANodes.push_back(result);
		allMANodes.push_back(result);
		return result;
	}

};

class HAR :
	virtual public CRoutingProtocol
{
private:

	static vector<CHotspot *> m_hotspots;
	static vector<CHARRoute> m_routes;  //��hotspot class
	//vector<CHARMANode> m_MANodes;
	static vector<CHARRoute> m_newRoutes;

	//ȡ���µ�·�߼���
	static inline void setNewRoutes(vector<CHARRoute> newRoutes)
	{
		m_newRoutes = newRoutes;
	}
	static inline vector<CHARRoute> getNewRoutes()
	{
		return m_newRoutes;
	}

	//���ڼ�������MA��������ʷƽ��ֵ
	static int SUM_MA_COST;
	static int COUNT_MA_COST;
	//���ڼ���MA��ƽ��·�㣨�ȵ㣩��������ʷƽ��ֵ
	static double SUM_WAYPOINT_PER_MA;
	static int COUNT_WAYPOINT_PER_MA;
	/** �������� **/

	//����hotspot classification
	static double getHotspotHeat(CHotspot *hotspot);
	static double getSumDataRate(vector<int> nodes);  //����ge��sum��ͬһ��node��Ӧ�ظ�����
	static double getTimeIncreForInsertion(int currentTime, CHARRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int currentTime, CHARRoute route, int front, CHotspot *hotspot);
	//��һ��route�����Ż���TSP ����ھ��㷨��
	static void OptimizeRoute(CHARRoute &route);
	//�������ͳ������
	static double calculateEDTime(int currentTime);

	static inline double getAverageMACost()
	{
		if( COUNT_MA_COST == 0 )
			return -1;
		else
			return double(SUM_MA_COST) / double(COUNT_MA_COST);
	}

	static inline double getAverageMAWaypoint()
	{
		if( COUNT_WAYPOINT_PER_MA == 0 )
			return -1;
		else
			return SUM_WAYPOINT_PER_MA / COUNT_WAYPOINT_PER_MA;
	}


	//����ѡȡ����hotspot���ϵõ�hotspot class�ļ���
	static void HotspotClassification(int currentTime);
	//�Գ����õ���hotspot class����·���Ż���TSP ����ھ��㷨��
	static void MANodeRouteDesign(int currentTime);
//	//���ض�ʱ���Ϸ�������
//	static void SendData(int currentTime);

//	//��ÿһ��̰��ѡ��֮ǰ���ã�����CHotspot::oldSelectedHotspots��Ѱ��Ͷ�ݼ���Ϊ0���ȵ�ɾ������CHotspot::deletedHotspots
//	//��ɾ�����Ӧ������position����CPosition::deletedPositions
//	static void DecayPositionsWithoutDeliveryCount(int currentTime);


public:

	HAR(){};
	~HAR(){};

	//�ж��Ƿ���δ�����ȥ����·��
	static inline bool hasMoreNewRoutes()
	{
		if( m_newRoutes.empty() )
			return false;
		else
			return true;
	}
	//�����ȵ���hasMoreNewRoutes�ж�
	static inline CHARRoute popRoute()
	{
		CHARRoute result = m_newRoutes[0];
		m_newRoutes.erase(m_newRoutes.begin());
		return result;
	}

	//����ʱ����ȵ㣬����ȴ�ʱ��
	static double calculateWaitingTime(int currentTime, CHotspot *hotspot);

	//���²�Э�鴫��Ŀ���/���ݰ�
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// sink <- MA 
	static vector<CPacket*> receivePackets(CSink* sink, CHARMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- sink 
	static vector<CPacket*> receivePackets(CHARMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time);
	// Node <- MA 
	static vector<CPacket*> receivePackets(CNode* node, CHARMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- Node 
	static vector<CPacket*> receivePackets(CHARMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

	static bool Init();
	static bool Operate(int currentTime);

};

#endif // __HAR_H__
