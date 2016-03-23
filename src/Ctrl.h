#pragma once

#include "GeneralData.h"
#include "Data.h"


class CCtrl :
	public CGeneralData
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
		_index,   //data index ( delivery preds & summary vector )
		_no_data  //inform no data to send
	} _TYPE_CTRL;


private:

	_TYPE_CTRL type;
	map<int, double> pred;
	vector<int> sv;
	vector<CData> ack;  //ֱ�Ӵ��� CData �࣬���������ʵ��Ӧ���� sv

	void init();

	CCtrl();


public:

	//RTS / CTS / NO_DATA
	CCtrl(int node, int timeBirth, int byte, _TYPE_CTRL type);
	//ACK
	CCtrl(int node, vector<CData> datas, int timeBirth, int byte, _TYPE_CTRL type);
	//data index ( delivery preds )
	CCtrl(int node, map<int, double> pred, int timeBirth, int byte, _TYPE_CTRL type);
	//data index ( summary vector )
	CCtrl(int node, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type);
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

