#pragma once

#include "Position.h"

extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

//�洢hostspot��Ϣ����
class CHotspot: public CBase
{
private:
	
	//int birth;  //hotspot��ID��birth������ʼposition��ID
	int nCoveredPosition;
	vector<CPosition *> coveredPositions;  //�����б�
	vector<int> coveredNodes;  //���ǵ�node�б�hotspotѡȡ�������ֶ�����generateCoveredNodes����
	double heat;
	static long int ID_COUNT;

	//merge-HAR
	int candidateType;  //���ڱ���ȵ��ѡ���ͣ�����̰��ѡȡ�����ȶȼ��㣩ʱʹ�ã����ȵ� / ���ȵ� / �鲢�ȵ�
	int age;  //���ڱ�Ǿ��ȵ��鲢�ȵ�����䣬����������

public:
	CHotspot()
	{
		this->nCoveredPosition = 0;
		//merge_HAR
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;
	}

	//��ĳ��Position��λ������һ��hotspot
	CHotspot(CPosition* pos, int time)  //timeӦ���ǵ�ǰtime��������pos��time
	{
		this->nCoveredPosition = 0;
		this->x = pos->getX();
		this->y = pos->getY();
		this->ID = pos->getID();
		this->time = time;
		this->heat = 0;
		//merge_HAR
		this->candidateType = TYPE_NEW_HOTSPOT;
		this->age = 0;

		addPosition(pos);
	}

	~CHotspot(){};

	//setters & getters
	inline void setNCoveredPosition(int nCoveredPosition)
	{
		this->nCoveredPosition = nCoveredPosition;
	}
	inline int getNCoveredPosition()
	{
		return nCoveredPosition;
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
	inline void setAge(int age)
	{
		this->age = age;
	}
	double getRatioByCandidateType()
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

	//�Զ�����ID�����ֶ�����
	inline void generateID()
	{
		if(this->ID != -1)
			return;
		this->ID = ID_COUNT;
		ID_COUNT++;
	}

	//���ĳ��position�Ƿ����ڸ����б���
	bool ifPositionExists(CPosition* pos);

	//�Ӹ����б���ɾ��ĳ��position��ֻ��̰���㷨���õ�
	void removePosition(CPosition* pos);

	//�Ӹ����б���ɾ�����position��ֻ��̰���㷨���õ�
	void removePositionList(vector<CPosition *> positions);

	//��һ��position�����hotspot�ĸ����б�
	void addPosition(CPosition* pos);

	//���ݰ�����position���¼�������
	//�޸ĸ����б�֮�󲻻��Զ����ã�Ӧ������Ҫ��ʱ���ֶ�����
	void recalculateCenter();

	//���ĳ��node�Ƿ��ڸ����б��У�������generateCoveredNodes֮�����
	bool ifNodeExists(int inode);

	//ȷ�����ǵ�node�б���hotspotѡȡ�������ֶ�����
	void generateCoveredNodes();
	inline int getNCoveredNodes()
	{
		return coveredNodes.size();
	}
	inline vector<int> getCoveredNodes()
	{
		return coveredNodes;
	}
	int getNCoveredPositionsForNode(int inode);

	string toString(bool withDetails);

};


