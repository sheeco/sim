#pragma once

#ifndef __HOTSPOT_H__
#define __HOTSPOT_H__

#include <iostream>
#include "Configuration.h"
#include "Position.h"
#include "Node.h"


//�洢hostspot��Ϣ����
class CHotspot : 
	virtual public CBasicEntity, public CGeoEntity
{
	friend class CHotspotSelect;
	friend class CPostSelect;
	friend class CNodeRepair;
	//friend class HAR;
	//friend class CHDC;


protected:

	typedef enum EnumHotspotType {_new_hotspot, _old_hotspot, _merge_hotspot } EnumHotspotType;


	static map<int, CHotspot*> atHotspots;

	static int encounterAtHotspot;
	static int visiterAtHotspot;
	//�ȵ��������п��ܵ�������ֻ�����ݼ����ȵ�ѡȡ�йأ�

private:
	
	//int birth;  //hotspot��ID��birth������ʼposition��ID
	vector<CPosition *> coveredPositions;  //�����б�
	vector<int> coveredNodes;  //���ǵ�node�б�hotspotѡȡ�������ֶ�����generateCoveredNodes����
	double heat;
	map<int, int> countsDelivery;  //�洢���ȵ��ϵ�Ͷ�ݼ��������ε��ȵ�Ӧ��ÿһ�����ڵ�Ͷ�ݼ�������ͳ��
	map<int, int> waitingTimes;  //�洢���ȵ��ϵĵȴ�ʱ��: keyָʾ��ʼʱ�䣬valueָʾ�ȴ�ʱ��
	double ratio;  //���ڲ����µ�ratio���㷽��������̰��ѡȡ�ͺ���ѡȡ�������õ�

	static int COUNT_ID;

	//merge-HAR
	EnumHotspotType typeHotspotCandidate;  //���ڱ���ȵ��ѡ���ͣ�����̰��ѡȡ�����ȶȼ��㣩ʱʹ�ã����ȵ� / ���ȵ� / �鲢�ȵ�
	int age;  //���ڱ�Ǿ��ȵ��鲢�ȵ�����䣬����������

	//���ĳ��position�Ƿ����ڸ����б���
	bool ifPositionExists(CPosition* pos);

	//�Ӹ����б���ɾ��ĳ��position��ֻ��̰���㷨���õ�
	void removePosition(CPosition* pos);

	//���ĳ��node�Ƿ��ڸ����б��У�������generateCoveredNodes֮�����
	bool ifNodeExists(int inode) const;

	//���������ȵ���ص�����������ص��򷵻�0
	static double getOverlapArea(CHotspot *oldHotspot, CHotspot *newHotspot);

	void init();
	CHotspot()
	{
		init();
	}

	static CHotspot* generateHotspot(CCoordinate location, vector<CPosition*> positions, int time);


public:

	//��pos��������һ����ʼhotspot������ɴ˺�ѡhotspot�Ĺ���
	//timeӦ���ǵ�ǰtime��������pos��time
	//��merge��������
	CHotspot(CCoordinate location, int time)
	{
		init();
		this->setLocation(location);
		this->setTime(time);
		this->generateID();
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
	//merge_HAR
	inline EnumHotspotType getTypeHotspotCandidate() const
	{
		return this->typeHotspotCandidate;
	}
	inline void setCandidateType(EnumHotspotType typeHotspotCandidate)
	{
		this->typeHotspotCandidate = typeHotspotCandidate;
	}
	inline int getAge() const
	{
		return this->age;
	}
	//FIXEME: δ����
	inline void setAge(int age)
	{
		this->age = age;
		if( countsDelivery.find(this->time) == countsDelivery.end() )
			countsDelivery[this->time] = 0;
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
	
	static CHotspot* getAtHotspot(int nodeid)
	{
		if(!isAtHotspot(nodeid))
			return nullptr;
		else
			return atHotspots[nodeid];
	}

	static void setAtHotspot(int nodeid, CHotspot* atHotspot)
	{
		atHotspots[nodeid] = atHotspot;
	}

	static bool isAtHotspot(int nodeid)
	{
		if(atHotspots.find(nodeid) == atHotspots.end())
			return false;
		else
			return atHotspots[nodeid] != nullptr;
	}
	static void encountAtHotspot()
	{
		++encounterAtHotspot;
	}

	static int getEncounterAtHotspot()
	{
		return encounterAtHotspot;
	}
	static double getPercentEncounterAtHotspot()
	{
		if(encounterAtHotspot == 0)
			return 0.0;
		return double(encounterAtHotspot) / double(CNode::getEncounter());
	}
	//	static double getPercentEncounterActive() 
	//	{
	//		if(encounterActive == 0)
	//			return 0.0;
	//		return double(encounterActive) / double(encounter);
	//	}

	//���ʼ���������ͳ�ƽڵ�λ���ȵ��ڵİٷֱȣ�HAR·������δ��ӵ��ã�
	static void visitAtHotspot()
	{
		++visiterAtHotspot;
	}
	static double getPercentVisiterAtHotspot()
	{
		if(visiterAtHotspot == 0)
			return 0.0;
		return double(visiterAtHotspot) / double(CNode::getVisiter());
	}
	static int getVisiterAtHotspot()
	{
		return visiterAtHotspot;
	}

	inline void recordCountDelivery(int n)
	{
		map<int, int>::iterator imap = countsDelivery.find(this->time);
		if( imap == countsDelivery.end() )
			countsDelivery[this->time] = n;
		else
			countsDelivery[this->time] = imap->second + n;
	}

	//�����һ�����ڵ�Ͷ�ݼ���
	//FIXEME: δ����
	inline int getCountDelivery()
	{
		map<int, int>::iterator imap = countsDelivery.find(this->time);
		if( imap != countsDelivery.end() )
		{
			return imap->second;
		}
		else
			throw string("CHotspot::getCountDelivery() : Cannot find delivery count for time " + STRING(this->time) + ") !");
	}
	//����( sinceTime, sinceTime + 900 )�ڼ��Ͷ�ݼ���
	inline int getCountDelivery(int sinceTime)
	{
		map<int, int>::iterator imap = countsDelivery.find(sinceTime);
		if( imap != countsDelivery.end() )
		{
			return imap->second;
		}
		else
			throw string("CHotspot::getCountDelivery(" + STRING(sinceTime) + ") : Cannot find delivery count for given arg !");
	}

	double getRatioByTypeHotspotCandidate() const;

	//�� CMANode::updateStatus �У���ÿ�εȴ�����waitingState����ʱ����
	inline void recordWaitingTime(int startTime, int duration)
	{
		if( waitingTimes.find(startTime) != waitingTimes.end() )
			throw string("CHotspot::recordWaitingTime(" + STRING(startTime) + ", " + STRING(duration) + ") : "
						 "Record (" + STRING(startTime) + ", " + STRING(waitingTimes.find(startTime)->second) + ") has already existed !");
		waitingTimes[startTime] = duration;
	}
	//���صȴ�ʱ��map������ͳ�Ƶȴ�ʱ��
	inline map<int, int> getWaitingTimes()
	{
		return waitingTimes;
	}
	string getWaitingTimesString(bool details)
	{
		stringstream sstr;
		int sum = 0;
		for( map<int, int>::iterator imap = waitingTimes.begin(); imap != waitingTimes.end();  )
		{
			if( details )
				sstr << imap->first << ":";
			sstr << imap->second;
			sum += imap->second;
			if( ++imap != waitingTimes.end() )
				sstr << ",";
			else
			{
				sstr << ";";
				break;
			}
		}
		sstr << "=" << STRING(sum);
		return sstr.str();
	}

	//�Զ�����ID�����ֶ����ã�Ϊ��ȷ���ȵ�ID��Ψһ�ԣ�������ʱ����ʱ��Ӧ���ô˺���
	inline void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}
	string format() const
	{
		return "Hotspot " + this->location.format();
	}
	string getName() const
	{
		return "Hotspot " + STRING(this->ID);
	}

	//������ifExists��RemoveFromList�Ⱥ�������Ϊ�жϲ�������
	inline static bool identical(CHotspot *x, CHotspot *y)
	{
		return x->getID() == y->getID();
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

	//Ϊ���нڵ����Ƿ�λ���ȵ������ڣ���ͳ��visiter��encounter���ȵ��������
	//visit �� encounter ������ͳ��ʱ�۽��ɹ켣�ļ�����
	static bool UpdateAtHotspotForNodes(vector<CNode *> nodes, vector<CHotspot*> hotspots, int now);
//	//Ϊ����MA�ڵ����Ƿ�λ���ȵ������ڣ�����xHAR��
//	static bool UpdateAtHotspotForMANodes(int now);
		
	/****************************************   merge-HAR   ****************************************/ 

	//���������ȵ㼯���������ȵ�pair���ص����֮�ͣ����������������ȵ㹲ͬ�ص����ص����ֵ��������μ��룬�Զ������Ƴ̶ȣ�
	static double getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots);
	//����һ���ȵ㼯���ڵ��ص���������ڸ�׼ȷ�ļ��㼯�ϼ��ص����
	static double getOverlapArea(vector<CHotspot *> hotspots);

};

#endif // __HOTSPOT_H__

