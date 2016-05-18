#pragma once

#ifndef __POSITION_H__
#define __POSITION_H__

#include "BasicEntity.h"
#include "GeoEntity.h"


//�洢�����ڵ��ƶ�λ�õ���
class CPosition : 
	public CBasicEntity, public CGeoEntity
{
private:

	int node;  //�����ڵ�ID
	//bool isCovered;
	double weight;
	static int COUNT_ID;


public:

//	static double CO_POSITION_DECAY;

	//���¹��о�̬�����Ǵ�ԭ����g_ϵ��ȫ�ֱ����ƶ����˴��ģ�����ԭ�������ö��������滻
	static vector<CPosition *> positions;
	static int nPositions;
//	//����Ӻ�ѡposition������ɾ����position��������
//	static vector<CPosition *> deletedPositions;

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
//	inline void decayWeight()
//	{
//		this->weight /= CO_POSITION_DECAY;
//	}
	//�Զ�����ID�����ֶ�����
	inline void generateID()
	{
		++COUNT_ID;
		this->ID = COUNT_ID;
	}


	//����positionָ��õ�����ȫ�ֱ���CPosition::positions�е��±꣬�����ȵ��ѡ���Ϲ���������ʹ��
	//Ӧ��CPosition::positions�������֮����ã����Ҵ˺����ٸı����е�Ԫ��˳��
	static int getIndexPosition(CPosition* pos);

};

#endif // __POSITION_H__
