#pragma once

#ifndef __CTRL_H__
#define __CTRL_H__

#include "Packet.h"
#include "Data.h"


class CCtrl :
	virtual public CPacket
{
//protected:

//	int ID;  //未使用
//	CCoordinate location;  //未使用
//	int time;  //该data最后一次状态更新的时间戳，用于校验，初始值应等于timeBirth
//	bool flag;
//	int node;  //所属node
//	int timeBirth;  //生成时间
//	int size;  //byte
//	int HOP;

public:

	typedef enum EnumCtrlType 
	{
		_rts, 
		_cts, 
		_ack, 
		_capacity,  //允许接受的最大数据个数
		_index,   //data index ( delivery preds in Prophet / summary vetor in Epidemic )
		_no_data  //inform no data to send
	} EnumCtrlType;


private:

	EnumCtrlType type;
	int capacity;
//	vector<int> sv;
	vector<CData> ack;  //直接传递 CData 类，方便操作，实际应传递 sv

	void init();

	CCtrl();


public:

	//RTS / CTS / NO_DATA
	CCtrl(int node, int timeBirth, int byte, EnumCtrlType type);
	//ACK
	CCtrl(int node, vector<CData> datas, int timeBirth, int byte, EnumCtrlType type);
	//capacity
	CCtrl(int node, int capacity, int timeBirth, int byte, EnumCtrlType type);
	//data index ( delivery preds )
	CCtrl(int node, map<int, double> pred, int timeBirth, int byte, EnumCtrlType type);
//	//data index ( summary vector )
//	CCtrl(int node, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type);
	~CCtrl(){};

	EnumCtrlType getType() const
	{
		return type;
	}

	int getCapacity() const
	{
		return capacity;
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

};

#endif // __CTRL_H__
