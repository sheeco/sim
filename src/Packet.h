#pragma once

#ifndef __PACKET_H__
#define __PACKET_H__

#include "GeneralData.h"


class CPacket :
	virtual public CGeneralData
{


protected:

	int node;  //所属node
	int timeBirth;  //生成时间
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

	//该数据被转发到达新的节点后应该调用的函数，将更新跳数或TTL剩余值，并更新时间戳
	//注意：数据发送方应在发送之前检查剩余HOP大于1
	virtual inline void arriveAnotherNode(int currentTime)
	{
		this->HOP--;
	};

	virtual inline bool allowForward() const = 0;

};

#endif // __PACKET_H__