#pragma once

#include "BasicEntity.h"
#include "GeoEntity.h"

extern double CO_POSITION_DECAY;


//�洢�����ڵ��ƶ�λ�õ���
class CPosition : 
	public CBasicEntity, public CGeoEntity
{
private:

	int node;  //�����ڵ�ID
	//bool isCovered;
	double weight;
	static int ID_COUNT;


public:

	//���¹��о�̬�����Ǵ�ԭ����g_ϵ��ȫ�ֱ����ƶ����˴��ģ�����ԭ�������ö��������滻
	static vector<CPosition *> positions;
	static int nPositions;
	//����Ӻ�ѡposition������ɾ����position��������
	static vector<CPosition *> deletedPositions;

	CPosition()
	{
		node = -1;
		//isCovered = false;
		weight = 1;
	}

	//setters & getters
	//inline void setIsCovered(bool isCovered)
	//{
	//	this->isCovered = isCovered;
	//}
	//inline bool IfIsCovered()
	//{
	//	return isCovered;
	//}

	inline int getNode() const
	{
		return node;
	}
	inline void setNode(int node)
	{
		this->node = node;
	}
	inline double getWeight() const
	{
		return weight;
	}
	inline void decayWeight()
	{
		this->weight /= CO_POSITION_DECAY;
	}
	//�Զ�����ID�����ֶ�����
	inline void generateID()
	{
		ID_COUNT++;
		this->ID = ID_COUNT;
	}


	//����positionָ��õ�����ȫ�ֱ���CPosition::positions�е��±꣬�����ȵ��ѡ���Ϲ���������ʹ��
	//Ӧ��CPosition::positions�������֮����ã����Ҵ˺����ٸı����е�Ԫ��˳��
	static int getIndexOfPosition(CPosition* pos);

};

