#pragma once

#ifndef __HOTSPOT_H__
#define __HOTSPOT_H__

#include <iostream>
#include "Position.h"
#include "GeoEntity.h"
#include "GeneralNode.h"


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

	typedef enum _TYPE_HOTSPOT {_new_hotspot, _old_hotspot, _merge_hotspot } _TYPE_HOTSPOT;

	// TODO: current public static attr should be converted to protected amap
	//���¹��о�̬�����Ǵ�ԭ����g_ϵ��ȫ�ֱ����ƶ����˴��ģ�����ԭ�������ö��������滻
	static vector<CHotspot *> hotspotCandidates;
	static vector<CHotspot *> selectedHotspots;
	//��һ��̰��ѡȡ���յõ����ȵ㼯�ϣ�����
	//ע�⣺merge�����õ������hotspotӦ��ʹ��CHotspot::hotspotCandidates�е�ʵ���޸ĵõ������ɱ�����CHotspot::oldSelectedHotspots��ʵ�����κ����ã���Ϊ��merge�����󽫱�free
	static map<int, vector<CHotspot *>> oldSelectedHotspots;
	//static vector<CHotspot *> deletedHotspots;


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
	_TYPE_HOTSPOT typeHotspotCandidate;  //���ڱ���ȵ��ѡ���ͣ�����̰��ѡȡ�����ȶȼ��㣩ʱʹ�ã����ȵ� / ���ȵ� / �鲢�ȵ�
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
		this->ratio = 0;		
		
		//merge_HAR
		this->typeHotspotCandidate = _new_hotspot;
		this->age = 0;
	}

	void generateHotspot(CCoordinate location, int time)
	{
		init();
		this->setLocation(location);
		this->setTime(time);
		//������setTime֮���ʼ��
		this->countsDelivery[this->time] = 0;
		this->generateID();

		//����flag
		for(int i = 0; i < CPosition::nPositions; ++i)
			CPosition::positions[i]->setFlag(false);

		bool modified;
		//ѭ����ֱ��û���µ�position������
		do
		{
			modified = false;
			//���µ�hotspot���ģ��ٴα���position
			for(int i = 0; i < CPosition::nPositions; ++i)
			{
				if( CPosition::positions[i]->getFlag() )
					continue;
				if( CPosition::positions[i]->getX() + CGeneralNode::RANGE_TRANS < this->getX() )
					continue;
				//��ˮƽ�����ѳ���range�������ֱ��ֹͣ����
				if( this->getX() + CGeneralNode::RANGE_TRANS < CPosition::positions[i]->getX() )
					break;
				if( CBasicEntity::withinRange(*this, *CPosition::positions[i], CGeneralNode::RANGE_TRANS ) )
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
	//merge_HAR
	inline _TYPE_HOTSPOT getTypeHotspotCandidate() const
	{
		return this->typeHotspotCandidate;
	}
	inline void setCandidateType(_TYPE_HOTSPOT typeHotspotCandidate)
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

	static vector<CHotspot*> getSelectedHotspots()
	{
		return selectedHotspots;
	}
	static vector<CHotspot*> getSelectedHotspots(int forTime)
	{
		if( !selectedHotspots.empty()
		   && forTime == selectedHotspots[0]->getTime() )
			return selectedHotspots;
		else if( oldSelectedHotspots.find(forTime) != oldSelectedHotspots.end() )
			return oldSelectedHotspots[forTime];
		else
			throw string("CHotspot::getSelectedHotspots(" + STRING(forTime) + ") : Cannot find selected hotspots for given time !");
	}

	//Ϊ���нڵ����Ƿ�λ���ȵ������ڣ���ͳ��visiter��encounter���ȵ��������
	//visit �� encounter ������ͳ��ʱ�۽��ɹ켣�ļ�����
	static bool UpdateAtHotspotForNodes(int currentTime);
//	//Ϊ����MA�ڵ����Ƿ�λ���ȵ������ڣ�����xHAR��
//	static bool UpdateAtHotspotForMANodes(int currentTime);
		
	/****************************************   merge-HAR   ****************************************/ 

	//���������ȵ㼯���������ȵ�pair���ص����֮�ͣ����������������ȵ㹲ͬ�ص����ص����ֵ��������μ��룬�Զ������Ƴ̶ȣ�
	static double getOverlapArea(vector<CHotspot *> oldHotspots, vector<CHotspot *> newHotspots);
	//����һ���ȵ㼯���ڵ��ص���������ڸ�׼ȷ�ļ��㼯�ϼ��ص����
	static double getOverlapArea(vector<CHotspot *> hotspots);

};

#endif // __HOTSPOT_H__

