#pragma once

#include "GeneralData.h"
#include "Data.h"


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

public:

	typedef enum _TYPE_CTRL 
	{
		_rts, 
		_cts, 
		_ack, 
		_index  //data index ( delivery preds & summary vector )
	} _TYPE_CTRL;


private:

	_TYPE_CTRL type;
	map<int, double> pred;
	vector<int> sv;
	vector<CData> ack;  //直接传递 CData 类，方便操作，实际应传递 sv

	void init();

	CCtrl();


public:

	//RTS / CTS
	CCtrl(int node, int timeBirth, int byte, _TYPE_CTRL type);
	//ACK
	CCtrl(int node, vector<CData> datas, int timeBirth, int byte, _TYPE_CTRL type);
	//data index ( delivery preds & summary vector )
	CCtrl(int node, map<int, double> pred, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type);
	~CCtrl();

	_TYPE_CTRL getType() const
	{
		return type;
	}

	map<int, double> getPred() const
	{
		return pred;
	}

	vector<int> getSV() const
	{
		return sv;
	}

	vector<CData> getACK() const
	{
		return ack;
	}



};

