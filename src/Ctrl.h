#pragma once

#include "GeneralData.h"


class CCtrl :
	public CGeneralData
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

protected:
	
	friend class CNode;
	typedef enum _TYPE_CTRL {_rts, _cts, _ack, _pred} _TYPE_CTRL;


private:

	int dst;  //-1: 默认广播
	_TYPE_CTRL type;
	map<int, double> pred;

	void init() override;

	CCtrl();


public:

	CCtrl(int node, int timeBirth, int byte, _TYPE_CTRL type);
	CCtrl(int node, int dst, int timeBirth, int byte, _TYPE_CTRL type);
	CCtrl(int node, int dst, int timeBirth, map<int, double> pred, int byte, _TYPE_CTRL type);
	~CCtrl();

	_TYPE_CTRL getType() const
	{
		return type;
	}

	map<int, double> getPred() const
	{
		return pred;
	}



};

