#pragma once

#ifndef __PACKET_H__
#define __PACKET_H__

#include "GeneralData.h"


class CPacket :
	virtual public CGeneralData
{


protected:

	int node;  //����node
	int timeBirth;  //����ʱ��
	int size;  //byte
	int HOP;

	virtual void init();


public:

	CPacket();
	virtual ~CPacket() = 0
	{};

	//setters & getters
	//	inline void setNode(int node)
	//	{
	//		this->node = node;
	//	}
	inline int getNode() const
	{
		return node;
	}
	inline int getTimeBirth() const
	{
		return timeBirth;
	}
	inline int getSize() const
	{
		return size;
	}
	static int getSumSize(vector<CPacket*> packets);

	//�����ݱ�ת�������µĽڵ��Ӧ�õ��õĺ�����������������TTLʣ��ֵ��������ʱ���
	//ע�⣺���ݷ��ͷ�Ӧ�ڷ���֮ǰ���ʣ��HOP����1
	virtual inline void arriveAnotherNode(int now)
	{
		this->HOP--;
	};

	virtual inline bool allowForward() const = 0;

};

#endif // __PACKET_H__