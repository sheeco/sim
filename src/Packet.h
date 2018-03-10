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
	int MAX_HOP;

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
	inline void arriveAnotherNode(int now)
	{
		this->HOP++;
	};

	//�ж��Ƿ�����ת����HOP > 0�����������򲻷���SV��
	inline bool allowForward() const
	{
		return MAX_HOP <= 0 || HOP < MAX_HOP;
	}



};

#endif // __PACKET_H__