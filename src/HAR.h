#pragma once

#ifndef __HAR_H__
#define __HAR_H__

#include "Hotspot.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "MANode.h"
#include "Route.h"
#include "RoutingProtocol.h"
#include "SMac.h"


class CHarRoute : 
	virtual public CRoute
{
protected:
	friend class CHarMANode;
	friend class HAR;

	vector<int> coveredNodes;
	int timeCreate;
	int timeExpire;
	bool overdue;  //�Ƿ����

	void init()
	{
		CRoute::init();
		timeCreate = INVALID;
		timeExpire = INVALID;
		overdue = false;
		this->setStartPoint(CSink::getSink());
	}
	int getTimeCreation() const
	{
		return timeCreate;
	}
	int getTimeExpiration() const
	{
		return timeExpire;
	}
	CHarRoute(int timeCreate, int timeExpire)
	{
		this->init();
		this->timeCreate = timeCreate;
		this->timeExpire = timeExpire;
	}

	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}

	inline bool isOverdue(int now) const
	{
		return this->getTimeExpiration() <= now;
	}
	//��������Ԫ�طŵ�waypoint�б�����
	void AddHotspot(CBasicEntity *hotspot)
	{
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

class CHarMANode : 
	virtual public CMANode
{
protected:
	friend class HAR;
	friend class CMacProtocol;

	CHarMANode(int now)
	{
		init();
		setTime(now);
	}

	inline CHarRoute* getHARRoute() const
	{
		CHarRoute* res = dynamic_cast< CHarRoute* >( this->route );
		return res;
	}
	inline void updateRoute(CHarRoute *route, int now)
	{
		CMANode::updateRoute(route);
		CPrintHelper::PrintBrief(now, this->getName() + " is assigned with route " + route->format() + ".");
	}
	inline CHotspot* getAtHotspot() const
	{
		return dynamic_cast<CHotspot*>( getAtWaypoint() );
	}
	inline bool routeHasExpired(int now) const
	{
		if( !this->hasRoute() )
			throw string("CHarMANode::routeHasExpired(): " + this->getName() + " has no route.");
		return getHARRoute()->isOverdue(now);
	}
	vector<CData> bufferData(int time, vector<CData> datas)
	{
		vector<CData> ack = CMANode::bufferData(time, datas);

		if( isAtWaypoint() )
			this->getAtHotspot()->recordCountDelivery(ack.size());

		return ack;
	}
	void updateStatus(int time);

};

class HAR :
	virtual public CRoutingProtocol
{
private:

	static vector<CHotspot *> hotspots;
	static vector<CHarRoute *> maRoutes;  //��hotspot class
	static vector<CHarRoute *> oldRoutes;
	static int indexRoute;
	//vector<CHarMANode> m_MANodes;
	//static vector<CHarRoute> m_newRoutes;

	static map<int, double> mapDataCountRates;

	static vector<CHarMANode *> allMAs;
	static vector<CHarMANode *> busyMAs;
	static vector<CHarMANode *> freeMAs;


	static bool newMANode(int now);
	static bool newMANode(int n, int now);
	static void initNodeInfo();
	static void initMANodes(int now);
	static void turnFree(CHarMANode * ma);
	static void turnBusy(CHarMANode * ma);

	//ȡ���µ�·�߼���
	static inline void updateRoutes(vector<CHarRoute*> newRoutes);
	static inline vector<CHarRoute*> getRoutes();

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
	static double getTimeIncreForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot);
	static double calculateRatioForInsertion(int now, CHarRoute route, int front, CHotspot *hotspot);
	//��һ��route�����Ż���TSP ����ھ��㷨��
	static void OptimizeRoute(CHarRoute *route);
	//�������ͳ������
	static double calculateEDTime(int now);

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
	static void HotspotClassification(int now);
	//�Գ����õ���hotspot class����·���Ż���TSP ����ھ��㷨��
	static void MANodeRouteDesign(int now);
//	//���ض�ʱ���Ϸ�������
//	static void SendData(int now);


public:

	HAR(){};
	~HAR(){};

	static vector<CHarMANode*> getAllMAs()
	{
		return allMAs;
	}

	static inline bool hasRoutes()
	{
		return ! maRoutes.empty();
	}
	//�����ȵ���hasRoutes�ж�
	static inline CHarRoute* popRoute();

	static void atMAReturn(CHarMANode* ma, int now);

	static inline double getSumEnergyConsumption();
	//����ʱ����ȵ㣬����ȴ�ʱ��
	static double calculateWaitingTime(int now, CHotspot *hotspot)
	{
		double result = 1;
		int count_trueHotspot = 0;
		vector<int> coveredNodes = hotspot->getCoveredNodes();
		vector<int> nCoveredPositionsForNode;

		for( int i = 0; i < coveredNodes.size(); ++i )
		{
			int temp_time = now;
			double temp;

			//IHAR: Reduce Memory now
			if( getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_improved )
			{
				temp_time = min(now, getConfig<int>("ihs", "lifetime_position"));
			}

			nCoveredPositionsForNode.push_back(hotspot->getNCoveredPositionsForNode(coveredNodes[i]));
			temp = double(hotspot->getNCoveredPositionsForNode(coveredNodes[i])) / double(temp_time + CHotspotSelect::SLOT_HOTSPOT_UPDATE);

			//merge-HAR: ratio
			temp *= pow(hotspot->getRatioByTypeHotspotCandidate(), hotspot->getAge());

			if( temp >= getConfig<double>("har", "beta") )
			{
				result *= temp;
				++count_trueHotspot;
			}
		}
		//FIXME: �������true hotspot��waiting timeΪ0
		if( count_trueHotspot == 0 )
			return getConfig<int>("har", "min_waiting_time");
		double prob = exp(-1 / hotspot->getHeat());
		result = prob / result;
		result = pow(result, ( 1 / double(count_trueHotspot) ));

		return result + getConfig<int>("har", "min_waiting_time");
	}

	//�������� MA �����ꡢ�ȴ�ʱ��
	//ע�⣺��������һ���ȵ�ѡȡ֮�����
	static void UpdateMANodeStatus(int now);

	static vector<CGeneralNode*> findNeighbors(CGeneralNode& src);
	static bool transmitFrame(CGeneralNode& src, CFrame* frame, int now);
	//���²�Э�鴫��Ŀ���/���ݰ�
	static vector<CPacket*> receivePackets(CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time);
	// sink <- MA 
	static vector<CPacket*> receivePackets(CSink* sink, CHarMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- sink 
	static vector<CPacket*> receivePackets(CHarMANode* ma, CSink* fromSink, vector<CPacket*> packets, int time);
	// Node <- MA 
	static vector<CPacket*> receivePackets(CNode* node, CHarMANode* fromMA, vector<CPacket*> packets, int time);
	// MA <- Node 
	static vector<CPacket*> receivePackets(CHarMANode* ma, CNode* fromNode, vector<CPacket*> packets, int time);

	static void CommunicateBetweenNeighbors(int now);

	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int now)
	{
		if( !( now % getConfig<int>("log", "slot_log") == 0
			  || now == getConfig<int>("simulation", "runtime") ) )
			return;


		/***************************************** ·��Э���ͨ����� *********************************************/

		CRoutingProtocol::PrintInfo(now);

		/***************************************** �ȵ�ѡȡ�������� *********************************************/

		CHotspotSelect::PrintInfo(now);

		if( !( now >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT ) )
			return;

		/**************************************** HAR ·�ɵĲ������ *********************************************/

		//hotspotѡȡ�����hotspot class��Ŀ��ED��Energy Consumption��MA�ڵ�buffer״̬ ...

		if( now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
		   || now == getConfig<int>("simulation", "runtime") )
		{
			//MA�ڵ����
			ofstream ma(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma"), ios::app);
			if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
			{
				ma << endl << getConfig<string>("log", "info_log") << endl;
				ma << getConfig<string>("log", "info_ma") << endl;
			}
			ma << now << TAB << maRoutes.size() << TAB << ( double(hotspots.size()) / double(maRoutes.size()) ) << endl;
			ma.close();

			//
			ofstream ma_route(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ma_route"), ios::app);
			if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
			{
				ma_route << endl << getConfig<string>("log", "info_log") << endl;
				ma_route << getConfig<string>("log", "info_ma_route") << endl;
			}
			for( vector<CHarRoute*>::iterator iroute = maRoutes.begin(); iroute != maRoutes.end(); iroute++ )
			{
				ma_route << now << TAB << ( *iroute )->format() << endl;
			}
			ma_route.close();

			//���ڼ���MA�ڵ��������ʷƽ��ֵ��Ϣ
			SUM_MA_COST += maRoutes.size();
			++COUNT_MA_COST;
			//���ڼ���MA·�㣨�ȵ㣩ƽ����������ʷƽ��ֵ��Ϣ
			SUM_WAYPOINT_PER_MA += double(hotspots.size()) / double(maRoutes.size());
			++COUNT_WAYPOINT_PER_MA;

			//ED��ƽ��Ͷ���ӳٵ�����ֵ
			ofstream ed(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_ed"), ios::app);
			if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
			{
				ed << endl << getConfig<string>("log", "info_log") << endl;
				ed << getConfig<string>("log", "info_ed") << endl;
			}
			ed << now << TAB << calculateEDTime(now) << endl;
			ed.close();

			//�ȵ�������Ͷ�ݼ�����ͳ����Ϣ
			ofstream hotspot_statistics(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hotspot_statistics"), ios::app);
			if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
			{
				hotspot_statistics << endl << getConfig<string>("log", "info_log") << endl;
				hotspot_statistics << getConfig<string>("log", "info_hotspot_statistics") << endl;
			}
			//�� t ��ʱ��ѡ�����ȵ㣬�������ڽ����� t + 900���� t + 1800 ʱ�̲ű�ͳ�����
			vector<int> timesToPrint;
			int timeBeforeYesterday = 0;
			//���н��������������һ�ֵ��ȵ�ͳ��
			if( now == getConfig<int>("simulation", "runtime") )
			{
				timeBeforeYesterday = ( now / CHotspotSelect::SLOT_HOTSPOT_UPDATE - 1 ) * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
				int timeYesterday = timeBeforeYesterday + CHotspotSelect::SLOT_HOTSPOT_UPDATE;
				if( timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
					timesToPrint.push_back(timeBeforeYesterday);
				if( timeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
					timesToPrint.push_back(timeYesterday);
			}
			else if( now % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0 )
			{
				timeBeforeYesterday = now - 2 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;
				if( timeBeforeYesterday >= CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
					timesToPrint.push_back(timeBeforeYesterday);
			}
			if( !timesToPrint.empty() )
			{
				for( vector<int>::iterator itime = timesToPrint.begin(); itime != timesToPrint.end(); itime++ )
				{
					vector<CHotspot *> hotspotsToPrint = CHotspotSelect::getSelectedHotspots(*itime);
					hotspotsToPrint = CSortHelper::mergeSort(hotspotsToPrint, CSortHelper::descendByCountDelivery);
					for( vector<CHotspot *>::iterator it = hotspotsToPrint.begin(); it != hotspotsToPrint.end(); ++it )
						hotspot_statistics << *itime << '-' << *itime + CHotspotSelect::SLOT_HOTSPOT_UPDATE << TAB
						<< ( *it )->getID() << TAB << ( *it )->format() << TAB << ( *it )->getNCoveredPosition() << "," << ( *it )->getNCoveredNodes() << TAB
						<< ( *it )->getRatio() << TAB << ( *it )->getWaitingTimesString(true) << TAB << ( *it )->getCountDelivery(*itime) << endl;
				}
			}
			hotspot_statistics.close();
		}

		//MA Buffer
		if( now % getConfig<int>("log", "slot_log") == 0
		   || now == getConfig<int>("simulation", "runtime") )
		{
			//ÿ��MA�ĵ�ǰbuffer״̬
			ofstream buffer_ma(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer_ma"), ios::app);
			if( now == CHotspotSelect::STARTTIME_HOTSPOT_SELECT )
			{
				buffer_ma << endl << getConfig<string>("log", "info_log") << endl;
				buffer_ma << getConfig<string>("log", "info_buffer_ma") << endl;
			}
			buffer_ma << now << TAB;
			for( auto iMA = busyMAs.begin(); iMA != busyMAs.end(); ++iMA )
				buffer_ma << ( *iMA )->getBufferSize() << TAB;
			buffer_ma << endl;
			buffer_ma.close();

		}

	}
	static void PrintFinal(int now)
	{
		CRoutingProtocol::PrintFinal(now);

		CHotspotSelect::PrintFinal(now);
	}

	static bool Init(int now);
	static bool Operate(int now);

};

#endif // __HAR_H__
