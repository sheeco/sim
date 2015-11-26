#pragma once

#include "Position.h"

extern bool TEST_BALANCED_RATIO;
extern bool TEST_LEARN;
extern int NUM_NODE;
extern int currentTime;
extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

//�洢hostspot��Ϣ����
class CHotspot: public CBase
{
private:
	
	//int birth;  //hotspot��ID��birth������ʼposition��ID
	vector<CPosition *> coveredPositions;  //�����б�
	vector<int> coveredNodes;  //���ǵ�node�б�hotspotѡȡ�������ֶ�����generateCoveredNodes����
	double heat;
	vector<int> deliveryCounts;  //�洢���ȵ��ϵ�Ͷ�ݼ��������ε��ȵ�Ӧ��ÿһ�����ڵ�Ͷ�ݼ�������ͳ��
	vector<int> waitingTimes;  //�洢���ȵ��ϵĵȴ�ʱ�䣬���ε��ȵ�Ӧ��ÿһ�����ڵ�Ͷ�ݼ�������ͳ��
	double ratio;  //���ڲ����µ�ratio���㷽��������̰��ѡȡ�ͺ���ѡȡ�������õ�

	static long int ID_COUNT;

	//merge-HAR
	int candidateType;  //���ڱ���ȵ��ѡ���ͣ�����̰��ѡȡ�����ȶȼ��㣩ʱʹ�ã����ȵ� / ���ȵ� / �鲢�ȵ�
	int age;  //���ڱ�Ǿ��ȵ��鲢�ȵ�����䣬����������

	//���ĳ��position�Ƿ����ڸ����б���
	bool ifPositionExists(CPosition* pos);

	//�Ӹ����б���ɾ��ĳ��position��ֻ��̰���㷨���õ�
	void removePosition(CPosition* pos);


	//���ĳ��node�Ƿ��ڸ����б��У�������generateCoveredNodes֮�����
	bool ifNodeExists(int inode);	

	//���������ȵ���ص�����������ص��򷵻�0
	static double getOverlapArea(CHotspot *oldHotspot, CHotspot *newHotspot);

public:

	//���¹��о�̬�����Ǵ�ԭ����g_ϵ��ȫ�ֱ����ƶ����˴��ģ�����ԭ�������ö��������滻
	static vector<CHotspot *> hotspotCandidates;
	static vector<CHotspot *> selectedHotspots;
	//��һ��̰��ѡȡ���յõ����ȵ㼯�ϣ�����
	//ע�⣺merge�����õ������hotspotӦ��ʹ��CHotspot::hotspotCandidates�е�ʵ���޸ĵõ������ɱ�����CHotspot::oldSelectedHotspots��ʵ�����κ����ã���Ϊ��merge�����󽫱�free
	static vector<CHotspot *> oldSelectedHotspots;
	static int nHotspotCandidates;
	//
	static vector<CHotspot *> deletedHotspots;

	CHotspot()
	{
		//merge_HAR
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;
		this->deliveryCounts.push_back(0);
		this->waitingTimes.push_back(0);
		this->ratio = 0;
	}

	//��ĳ��Position��λ������һ��hotspot
	CHotspot(CPosition* pos, int time)  //timeӦ���ǵ�ǰtime��������pos��time
	{
		this->x = pos->getX();
		this->y = pos->getY();
		this->ID = -1;
		this->time = time;
		this->heat = 0;
		//merge_HAR
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;
		this->deliveryCounts.push_back(0);
		this->waitingTimes.push_back(0);
		this->ratio = 0;

		addPosition(pos);
	}

	~CHotspot(){};

	//setters & getters
	inline int getNCoveredPosition()
	{
		return coveredPositions.size();
	}
	inline vector<CPosition *> getCoveredPositions()
	{
		return coveredPositions;
	}
	inline double getHeat()
	{
		return heat;
	}
	inline void setHeat(double heat)
	{
		this->heat = heat;
	}
	inline static bool identical(CHotspot *x, CHotspot *y)
	{
		return x->getID() == y->getID();
	}

	//merge_HAR
	inline int getCandidateType()
	{
		return this->candidateType;
	}
	inline void setCandidateType(int candidateType)
	{
		this->candidateType = candidateType;
	}
	inline int getAge()
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
	inline int getNCoveredNodes()
	{
		return coveredNodes.size();
	}
	inline vector<int> getCoveredNodes()
	{
		return coveredNodes;
	}
	inline double getRatio()
	{
		return ratio;
	}
	//�µ�ratio���㷽��������̰��ѡȡ�ͺ���ѡȡ�������õ�����ʹ��-balanced-ratioʱratio==nCoverdPosition
	//ע�⣺����֮ǰ����ȷ��coveredNodes�ѵõ�����
	double calculateRatio()
	{
		if( TEST_BALANCED_RATIO )
		{
			ratio = coveredPositions.size() * ( NUM_NODE - coveredNodes.size() + 1 ) / NUM_NODE;
			return ratio;
		}
		else if( TEST_LEARN )
		{
			ratio = 0;
			for(vector<CPosition*>::iterator ipos = coveredPositions.begin(); ipos != coveredPositions.end(); ipos++)
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
	
	double getCoByCandidateType()
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
			cout << "Error: CHotspot:: getDeliveryCount(" << untilTime << ") " << i << " exceeds (0," << deliveryCounts.size() - 1 << ") !" << endl;
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
			cout << "Error: CHotspot:: getDeliveryCount(" << untilTime << ") " << i << " exceeds (0," << waitingTimes.size() - 1 << ") !" << endl;
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
		//if(this->ID != -1)
		//	return;
		this->ID = ID_COUNT;
		ID_COUNT++;
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

	//���ɰ������ȵ��ʱ�䡢���䡢ID�����ꡢcover������Ϣ���ַ���
	string toString(bool withDetails);
	
	//���������ȵ㼯���������ȵ�pair���ص����֮�ͣ����������������ȵ㹲ͬ�ص����ص����ֵ��������μ��룬�Զ������Ƴ̶ȣ�
	static double getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots);
	//����һ���ȵ㼯���ڵ��ص���������ڸ�׼ȷ�ļ��㼯�ϼ��ص����
	static double getOverlapArea(vector<CHotspot *> hotspots);

};


