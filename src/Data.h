#pragma once

#ifndef __DATA_H__
#define __DATA_H__

#include "Entity.h"


class CGeneralData :
	virtual public CBasicEntity
{
//protected:

//	CCoordinate location;  //δʹ��
//	int time;  //��data���һ��״̬���µ�ʱ���������У�飬��ʼֵӦ����timeBirth
//	bool flag;

public:

	CGeneralData(){};
	virtual ~CGeneralData() = 0
	{};

};


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
	{
	};

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


class CData :
	virtual public CPacket, virtual public CUnique
{
private:

	int timeArrival;  //����sink��ʱ��

	//����ͳ��Ͷ���ʺ�ʱ�ӵľ�̬����
	static int COUNT_ID;  //��ֵ����data������
	static int COUNT_ARRIVAL;  //��������ݼ���
	static double SUM_DELAY;  //ʱ�ӼӺͣ����ڼ���ƽ��ʱ��
	static double SUM_HOP;  //�����Ӻͣ����ڼ���ƽ������

	static int COUNT_DELIVERY_AT_WAYPOINT;  //���ȵ㴦�õ�Ͷ�ݵ����ݼ���
	static int COUNT_DELIVERY_ON_ROUTE;  //��·���ϵõ�Ͷ�ݵ����ݼ���

	CData()
	{
		CData::init();
	};

	//�Զ�����ID
	//ID = node_id * 10 000 000 + data_counter ��������SV��ʶ��Data��Դ
	inline void generateID() override
	{
		CUnique::generateID(COUNT_ID);
	}


protected:

	void init();


public:

	CData(int node, int timeBirth, int byte)
	{
		CData::init();
		this->node = node;
		this->time = this->timeBirth = timeBirth;
		this->size = byte;
		this->generateID();
	}

	~CData()
	{
	};

	static void deliverAtWaypoint(int n)
	{
		COUNT_DELIVERY_AT_WAYPOINT += n;
	}
	static void deliverOnRoute(int n)
	{
		COUNT_DELIVERY_ON_ROUTE += n;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���COUNT_ARRIVAL�ģ�����������;
	static int getCountDeliveryAtWaypoint()
	{
		return COUNT_DELIVERY_AT_WAYPOINT;
	}
	//�ú���Ӧ����MA��·������ʱ�������ͳ�ƽ��
	//ע�⣺�������������ͳ�Ʒ�����MA�����������ֵ�ļӺ����Ǵ��ڵ���COUNT_ARRIVAL�ģ�����������;
	static int getCountDeliveryTotal()
	{
		return COUNT_DELIVERY_AT_WAYPOINT + COUNT_DELIVERY_ON_ROUTE;
	}
	static double getPercentDeliveryAtWaypoint()
	{
		if(COUNT_DELIVERY_AT_WAYPOINT == 0)
			return 0.0;
		return double(COUNT_DELIVERY_AT_WAYPOINT) / double(COUNT_DELIVERY_AT_WAYPOINT + COUNT_DELIVERY_ON_ROUTE);
	}

	//setters & getters
	inline int getTimeArrival() const
	{
		return timeArrival;
	}

	inline void arriveSink(int timeArrival)
	{
		this->timeArrival = timeArrival;
		this->time = timeArrival;
		++COUNT_ARRIVAL;
		SUM_DELAY += timeArrival - timeBirth;
		SUM_HOP += HOP;
	}

	//ͳ������
	static int getCountData()
	{
		return COUNT_ID;
	}
	static int getCountDelivery()
	{
		return COUNT_ARRIVAL;
	}
	static double getDeliveryRatio()
	{
		if(COUNT_ID == 0)
			return 0;
		else
			return double(COUNT_ARRIVAL) / double(COUNT_ID);
	}
	static double getAverageDelay()
	{
		if(COUNT_ARRIVAL == 0)
			return 0;
		return SUM_DELAY / COUNT_ARRIVAL;
	}
	static double getAverageHOP()
	{
		if(COUNT_ARRIVAL == 0)
			return 0;
		return SUM_HOP / COUNT_ARRIVAL;
	}

	static vector<CData> GetItemsByID(vector<CData> list, vector<int> ids);

	//���رȽϲ��������Ƚ�����ʱ�䣬����mergeSort
	friend bool operator < (const CData lt, const CData rt);
	friend bool operator > (const CData lt, const CData rt);
	//���� == ������������ ID �ж� identical������ȥ��
	friend bool operator == (const CData lt, const CData rt);
	friend bool operator == (int id, const CData data);
	friend bool operator == (const CData data, int id);

};


class CCtrl :
	virtual public CPacket
{
public:

	typedef enum EnumCtrlType
	{
		_rts,
		_cts,
		_ack,
		_capacity,  //������ܵ�������ݸ���
		_index,   //data index ( delivery preds in Prophet / summary vetor in Epidemic )
		_no_data  //inform no data to send
	} EnumCtrlType;


private:

	EnumCtrlType type;
	int capacity;
	//	vector<int> sv;
	vector<CData> ack;  //ֱ�Ӵ��� CData �࣬���������ʵ��Ӧ���� sv

	void init();

	CCtrl();


public:

	//RTS / CTS / NO_DATA
	CCtrl(int node, int timeBirth, int byte, EnumCtrlType type);
	//ACK
	CCtrl(int node, vector<CData> datas, int timeBirth, int byte, EnumCtrlType type);
	//capacity
	CCtrl(int node, int capacity, int timeBirth, int byte, EnumCtrlType type);
	////data index ( delivery preds )
	//CCtrl(int node, map<int, double> pred, int timeBirth, int byte, EnumCtrlType type);
	////data index ( summary vector )
	//CCtrl(int node, vector<int> sv, int timeBirth, int byte, _TYPE_CTRL type);
	~CCtrl()
	{
	};

	EnumCtrlType getType() const
	{
		return type;
	}

	int getCapacity() const
	{
		return capacity;
	}

	vector<CData> getACK() const
	{
		return ack;
	}

};


class CGeneralNode;

class CFrame :
	virtual public CGeneralData
{
private:

	CGeneralNode* src;
	CGeneralNode* dst;  //Ĭ�� null, �㲥
	int headerMac;
	//ע�⣺Ԫ�ؿ���Ϊ��ָ�룬����ζ���κ�����֮ǰ��Ҫ�ж�
	vector<CPacket*> packets;

	void init();
	CFrame();


public:

	//������
	CFrame(CGeneralNode& src, CGeneralNode& dst, vector<CPacket*> packets);
	//�㲥��
	CFrame(CGeneralNode& src, vector<CPacket*> packets);

	~CFrame();

	inline CGeneralNode* getSrcNode() const
	{
		return src;
	}
	inline CGeneralNode* getDstNode() const
	{
		return dst;
	}
	inline vector<CPacket*> getPackets() const
	{
		return packets;
	}

	int getSize() const;

};


#endif // __DATA_H__
