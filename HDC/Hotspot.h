#pragma once

#include "Position.h"
#include "GeoEntity.h"
#include <iostream>

extern bool TEST_BALANCED_RATIO;
extern bool TEST_LEARN;
extern int NUM_NODE;
extern int TRANS_RANGE;
extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

using namespace std;

//�洢hostspot��Ϣ����
class CHotspot : 
	public CGeoEntity
{
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
	int candidateType;  //���ڱ���ȵ��ѡ���ͣ�����̰��ѡȡ�����ȶȼ��㣩ʱʹ�ã����ȵ� / ���ȵ� / �鲢�ȵ�
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
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;
		this->deliveryCounts.push_back(0);		
	}

	void generateHotspot(double x, double y, int time)
	{
		init();
		this->setX(x);
		this->setY(y);
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
				if( CPosition::positions[i]->getX() + TRANS_RANGE < this->getX() )
					continue;
				//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
				if( this->getX() + TRANS_RANGE < CPosition::positions[i]->getX() )
					break;
				if(CBasicEntity::getDistance(*this, *CPosition::positions[i]) <= TRANS_RANGE)
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

	//���¹��о�̬�����Ǵ�ԭ����g_ϵ��ȫ�ֱ����ƶ����˴��ģ�����ԭ�������ö��������滻
	static vector<CHotspot *> hotspotCandidates;
	static vector<CHotspot *> selectedHotspots;
	//��һ��̰��ѡȡ���յõ����ȵ㼯�ϣ�����
	//ע�⣺merge�����õ������hotspotӦ��ʹ��CHotspot::hotspotCandidates�е�ʵ���޸ĵõ������ɱ�����CHotspot::oldSelectedHotspots��ʵ�����κ����ã���Ϊ��merge�����󽫱�free
	static vector<CHotspot *> oldSelectedHotspots;
	static vector<CHotspot *> deletedHotspots;


	//��pos��������һ����ʼhotspot������ɴ˺�ѡhotspot�Ĺ���
	//timeӦ���ǵ�ǰtime��������pos��time
	CHotspot(CPosition* pos, int time)
	{
		addPosition(pos);
		generateHotspot(x, y, time);
	}

	//��merge��������
	CHotspot(double x, double y, int time)
	{
		generateHotspot(x, y, time);
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
	inline int getCandidateType() const
	{
		return this->candidateType;
	}
	inline void setCandidateType(int candidateType)
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
	double calculateRatio()
	{
		if( TEST_BALANCED_RATIO )
		{
			ratio = coveredPositions.size() * double(NUM_NODE - coveredNodes.size() + 1) / double(NUM_NODE);
			return ratio;
		}
		else if( TEST_LEARN )
		{
			ratio = 0;
			for(vector<CPosition*>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ++ipos)
				ratio += (*ipos)->getWeight();
			return ratio;

		}
		else
		{
			ratio = coveredPositions.size();
			return ratio;
		}
	}
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
			case TYPE_MERGE_HOTSPOT: 
				return RATIO_MERGE_HOTSPOT;
			case TYPE_NEW_HOTSPOT: 
				return RATIO_NEW_HOTSPOT;
			case TYPE_OLD_HOTSPOT: 
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
			_PAUSE;
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
			_PAUSE;
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


