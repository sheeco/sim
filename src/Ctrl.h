#pragma once

#ifndef __CTRL_H__
#define __CTRL_H__

#include "Packet.h"
#include "Data.h"


class CCtrl :
	virtual public CPacket
{
//protected:

//	int ID;  //δʹ��
//	CCoordinate location;  //δʹ��
//	int time;  //��data���һ��״̬���µ�ʱ���������У�飬��ʼֵӦ����timeBirth
//	bool flag;
//	int node;  //����node
//	int timeBirth;  //����ʱ��
//	int size;  //byte
//	int HOP;

public:

	typedef enum _TYPE_CTRL 
	{
		_rts, 
		_cts, 
		_ack, 
		_capacity,  //������ܵ�������ݸ���
		_index,   //data index ( delivery preds & summary vector )
		_no_data  //inform no data to send
	} _TYPE_CTRL;


private:

	_TYPE_CTRL type;
	int capacity;
	map<int, double> pred;
//	vector<int> sv;
	vector<CData> ack;  //ֱ�Ӵ��� CData �࣬���������ʵ��Ӧ���� sv

	void init();

	CCtrl();


public:

	//RTS / CTS / NO_DATA
	CCtrl(int node, int timeBirth, int byte, _TYPE_CTRL type);
	//ACK
	CCtrl(int node, vector<CData> datas, int timeBirth, int byte, _TYPE_CTRL type);
	//capacity
	CCtrl(int node, int capacity, int timeBirth, int byte, _TYPE_CTRL type);
	//data index ( delivery preds )
	CCtrl(int node, map<int, double> pred, int timeBirth, int byte, _TYPE_CTRL type);
//	//data index ( summary vector )
//	CCtrl(int node, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type);
	~CCtrl(){};

	_TYPE_CTRL getType() const
	{
		return type;
	}

	int getCapacity() const
	{
		return capacity;
	}

	map<int, double> getPred() const
	{
		return pred;
	}

//	vector<int> getSV() const
//	{
//		return sv;
//	}
//
	vector<CData> getACK() const
	{
		return ack;
	}

	bool allowForward() const
	{
		return HOP > 0;
	};

};

#endif // __CTRL_H__
