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

	//该数据被转发到达新的节点后应该调用的函数，将更新跳数或TTL剩余值，并更新时间戳
	//注意：数据发送方应在发送之前检查剩余HOP大于1
	inline void arriveAnotherNode(int now)
	{
		this->HOP++;
	};

	//判断是否允许转发（HOP > 0），不允许则不放入SV中
	inline bool allowForward() const
	{
		return MAX_HOP <= 0 || HOP < MAX_HOP;
	}



};

#endif // __PACKET_H__