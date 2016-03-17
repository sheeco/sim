#pragma once

#include "GeneralData.h"


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

protected:
	
	friend class CNode;
	typedef enum _TYPE_CTRL {_rts, _cts, _ack, _pred} _TYPE_CTRL;


private:

	int dst;  //-1: Ĭ�Ϲ㲥
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

