#pragma once

#include <iostream>
#include "Position.h"
#include "GeoEntity.h"
#include "GeneralNode.h"


//�洢hostspot��Ϣ����
class CHotspot : 
	public CBasicEntity, public CGeoEntity
{
	friend class CHotspotSelect;
	friend class CPostSelect;
	friend class CNodeRepair;
	friend class HAR;
	friend class CHDC;


protected:

	typedef enum _TYPE_HOTSPOT {_new_hotspot, _old_hotspot, _merge_hotspot } _TYPE_HOTSPOT;

	//TODO: current public static attr should be converted to protected amap
	//���¹��о�̬�����Ǵ�ԭ����g_ϵ��ȫ�ֱ����ƶ����˴��ģ�����ԭ�������ö��������滻
	static vector<CHotspot *> hotspotCandidates;
	static vector<CHotspot *> selectedHotspots;
	//��һ��̰��ѡȡ���յõ����ȵ㼯�ϣ�����
	//ע�⣺merge�����õ������hotspotӦ��ʹ��CHotspot::hotspotCandidates�е�ʵ���޸ĵõ������ɱ�����CHotspot::oldSelectedHotspots��ʵ�����κ����ã���Ϊ��merge�����󽫱�free
	static vector<CHotspot *> oldSelectedHotspots;
	static vector<CHotspot *> deletedHotspots;


private:
	
	//int birth;  //hotspot��ID��birth������ʼposition��ID
	vector<CPosition *> coveredPositions;  //�����б�
	vector<int> coveredNodes;  //���ǵ�node�б�hotspotѡȡ�������ֶ�����generateCoveredNodes����
	double heat;
	vector<int> deliveryCounts;  //�洢���ȵ��ϵ�Ͷ�ݼ��������ε��ȵ�Ӧ��ÿһ�����ڵ�Ͷ�ݼ�������ͳ��
	vector<int> waitingTimes;  //�洢���ȵ��ϵĵȴ�ʱ�䣬���ε��ȵ�Ӧ��ÿһ�����ڵ�Ͷ�ݼ�������ͳ��
	double ratio;  //���ڲ����µ�ratio���㷽��������̰��ѡȡ�ͺ���ѡȡ�������õ�

	static int ID_COUNT;

	//merge-HAR
	_TYPE_HOTSPOT candidateType;  //���ڱ���ȵ��ѡ���ͣ�����̰��ѡȡ�����ȶȼ��㣩ʱʹ�ã����ȵ� / ���ȵ� / �鲢�ȵ�
	int age;  //���ڱ�Ǿ��ȵ��鲢�ȵ�����䣬����������

	//���ĳ��position�Ƿ����ڸ����б���
	bool ifPositionExists(CPosition* pos);

	//�Ӹ����б���ɾ��ĳ��position��ֻ��̰���㷨���õ�
	void removePosition(CPosition* pos);

	//���ĳ��node�Ƿ��ڸ����б��У�������generateCoveredNodes֮�����
	bool ifNodeExists(int inode) const;

	//���������ȵ���ص�����������ص��򷵻�0
	static double getOverlapArea(CHotspot *oldHotspot, CHotspot *newHotspot);

	void init()
	{
		this->heat = 0;
		this->waitingTimes.push_back(0);
		this->ratio = 0;		
		
		//merge_HAR
		this->candidateType = _new_hotspot;
		this->age = 0;
		this->deliveryCounts.push_back(0);		
	}

	void generateHotspot(CCoordinate location, int time)
	{
		init();
		this->setLocation(location);
		this->setTime(time);
		this->generateID();

		//����flag
		for(int i = 0; i < CPosition::nPositions; i++)
			CPosition::positions[i]->setFlag(false);

		bool modified;
		//ѭ����ֱ��û���µ�position������
		do
		{
			modified = false;
			//���µ�hotspot���ģ��ٴα���position
			for(int i = 0; i < CPosition::nPositions; i++)
			{
				if(CPosition::positions[i]->getFlag())
					continue;
				if( CPosition::positions[i]->getX() + CGeneralNode::TRANS_RANGE < this->getX() )
					continue;
				//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
				if( this->getX() + CGeneralNode::TRANS_RANGE < CPosition::positions[i]->getX() )
					break;
				if(CBasicEntity::getDistance(*this, *CPosition::positions[i]) <= CGeneralNode::TRANS_RANGE)
				{
					this->addPosition(CPosition::positions[i]);
					CPosition::positions[i]->setFlag(true);
					modified = true;
				}
			}

			//���¼���hotspot������
			if(modified)
				this->recalculateCenter();
		}while(modified);
		
	}

	CHotspot()
	{
		init();
	}


public:

	static int SLOT_LOCATION_UPDATE;  //������Ϣ�ռ���slot
	static int SLOT_HOTSPOT_UPDATE;	 //�����ȵ�ͷ����slot
	static int TIME_HOSPOT_SELECT_START;  //no MA node at first

	static double RATIO_MERGE_HOTSPOT;
	static double RATIO_NEW_HOTSPOT;
	static double RATIO_OLD_HOTSPOT;

	//��pos��������һ����ʼhotspot������ɴ˺�ѡhotspot�Ĺ���
	//timeӦ���ǵ�ǰtime��������pos��time
	CHotspot(CPosition* pos, int time)
	{
		init();
		addPosition(pos);
		generateHotspot(pos->getLocation(), time);
	}

	//��merge��������
	CHotspot(CCoordinate location, int time)
	{
		init();
		generateHotspot(location, time);
	}

	~CHotspot(){};

	//setters & getters
	inline int getNCoveredPosition() const
	{
		return coveredPositions.size();
	}
	inline vector<CPosition *> getCoveredPositions() const
	{
		return coveredPositions;
	}
	inline double getHeat() const
	{
		return heat;
	}
	inline void setHeat(double heat)
	{
		this->heat = heat;
	}
	//������ifExists��RemoveFromList�Ⱥ�������Ϊ�жϲ�������
	inline static bool identical(CHotspot *x, CHotspot *y)
	{
		return x->getID() == y->getID();
	}

	//merge_HAR
	inline _TYPE_HOTSPOT getCandidateType() const
	{
		return this->candidateType;
	}
	inline void setCandidateType(_TYPE_HOTSPOT candidateType)
	{
		this->candidateType = candidateType;
	}
	inline int getAge() const
	{
		return this->age;
	}
	//FIXEME: δ����
	inline void setAge(int age)
	{
		this->age = age;
		while( deliveryCounts.size() < age + 1 )
			deliveryCounts.push_back(0);
		while( waitingTimes.size() < age + 1 )
			waitingTimes.push_back(0);
	}
	inline int getNCoveredNodes() const
	{
		return coveredNodes.size();
	}
	inline vector<int> getCoveredNodes() const
	{
		return coveredNodes;
	}
	inline double getRatio() const
	{
		return ratio;
	}
	//�µ�ratio���㷽��������̰��ѡȡ�ͺ���ѡȡ�������õ�����ʹ��-balanced-ratioʱratio==nCoverdPosition
	//ע�⣺����֮ǰ����ȷ��coveredNodes�ѵõ�����
	double calculateRatio();

	//�������б�������ʱ���ã��������ꡢ���ǽڵ��б��ratio
	void updateStatus()
	{
		recalculateCenter();
		generateCoveredNodes();
		calculateRatio();
	}

	int getNCoveredPositionsForNode(int inode);	
	
	double getCoByCandidateType() const
	{
		switch( this->candidateType )
		{
			case _merge_hotspot: 
				return RATIO_MERGE_HOTSPOT;
			case _new_hotspot: 
				return RATIO_NEW_HOTSPOT;
			case _old_hotspot: 
				return RATIO_OLD_HOTSPOT;
			default:
				return 1;
		}
	}

	//�ӵ�ǰ��һ�ȵ������ڵ�Ͷ�ݼ������ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//FIXEME: δ����
	inline int getDeliveryCount()
	{
		if(age == 0)
			return deliveryCounts.at(0);
		else
			return deliveryCounts.at( age - 1 );
	}
	//����( untilTime - 900, untilTime )�ڼ��Ͷ�ݼ���
	inline int getDeliveryCount(int untilTime)
	{
		int i = ( untilTime - time ) / SLOT_HOTSPOT_UPDATE - 1;
		if( i < 0 || i > deliveryCounts.size() )
		{
			cout << endl << "Error @ CHotspot::getDeliveryCount(" << untilTime << ") : " << i << " exceeds (0," << deliveryCounts.size() - 1 << ") !" << endl;
			_PAUSE_;
		}
		return deliveryCounts.at( i );
	}
	inline void addDeliveryCount(int n)
	{
		deliveryCounts.at( deliveryCounts.size() - 1 ) += n;
	}
	//�ӵ�ǰ��һ�ȵ������ڵĵȴ�ʱ�䣬�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//FIXEME: δ����
	inline int getWaitingTime()
	{
		if(age == 0)
			return waitingTimes.at(0);
		else
			return waitingTimes.at( age - 1 );
	}
	//����( untilTime - 900, untilTime )�ڼ�ĵȴ�ʱ��
	inline int getWaitingTime(int untilTime)
	{
		int i = ( untilTime - time ) / SLOT_HOTSPOT_UPDATE - 1;
		if( i < 0 || i > waitingTimes.size() )
		{
			cout << endl << "Error @ CHotspot::getDeliveryCount(" << untilTime << ") : " << i << " exceeds (0," << waitingTimes.size() - 1 << ") !" << endl;
			_PAUSE_;
		}		return waitingTimes.at( i );
	}
	inline void addWaitingTime(int t)
	{
		waitingTimes.at( waitingTimes.size() - 1 ) += t;
	}

	//�Զ�����ID�����ֶ����ã�Ϊ��ȷ���ȵ�ID��Ψһ�ԣ�������ʱ����ʱ��Ӧ���ô˺���
	inline void generateID()
	{
		ID_COUNT++;
		this->ID = ID_COUNT;
	}

	//�Ӹ����б���ɾ�����position��ֻ��̰���㷨���õ�
	void removePositionList(vector<CPosition *> positions);

	//��һ��position�����hotspot�ĸ����б�
	void addPosition(CPosition* pos);

	//���ݰ�����position���¼�������
	//�޸ĸ����б�֮�󲻻��Զ����ã�Ӧ������Ҫ��ʱ���ֶ�����
	void recalculateCenter();

	//ȷ�����ǵ�node�б���hotspotѡȡ�������ֶ�����
	void generateCoveredNodes();
		
	/****************************************   merge-HAR   ****************************************/ 

	//���������ȵ㼯���������ȵ�pair���ص����֮�ͣ����������������ȵ㹲ͬ�ص����ص����ֵ��������μ��룬�Զ������Ƴ̶ȣ�
	static double getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots);
	//����һ���ȵ㼯���ڵ��ص���������ڸ�׼ȷ�ļ��㼯�ϼ��ص����
	static double getOverlapArea(vector<CHotspot *> hotspots);

};


