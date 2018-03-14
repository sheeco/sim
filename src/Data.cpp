#include "Data.h"
#include "Configuration.h"


CPacket::CPacket()
{
	CPacket::init();
}

int CPacket::getSumSize(vector<CPacket*> packets)
{
	int size = 0;
	for(auto ipacket = packets.begin(); ipacket != packets.end(); ++ipacket)
	{
		size += ( *ipacket )->getSize();
	}
	return size;
}

void CPacket::init()
{
	this->node = -1;
	this->timeBirth = 0;
	this->size = 0;
	this->HOP = 0;
	this->MAX_HOP = INVALID;
}


int CData::COUNT_ID = 0;
int CData::COUNT_ARRIVAL = 0;
double CData::SUM_DELAY = 0;
double CData::SUM_HOP = 0;
int CData::COUNT_DELIVERY_AT_WAYPOINT = 0;
int CData::COUNT_DELIVERY_ON_ROUTE = 0;

void CData::init()
{
	CPacket::init();
	this->timeArrival = INVALID;
	this->HOP = 0;
	this->MAX_HOP = getConfig<int>("data", "max_hop");
}

vector<CData> CData::GetItemsByID(vector<CData> list, vector<int> ids)
{
	vector<CData> result;
	for(vector<int>::iterator id = ids.begin(); id != ids.end(); ++id)
	{
		for(vector<CData>::iterator item = list.begin(); item != list.end(); ++item)
		{
			if( item->getID() == *id )
			{
				result.push_back(*item);
				break;
			}
		}
	}
	return result;
}

//���رȽϲ��������Ƚ�����ʱ�䣬����mergeSort
bool operator < (const CData lt, const CData rt)
{
	return lt.getTimeBirth() < rt.getTimeBirth();
}
bool operator > (const CData lt, const CData rt)
{
	return lt.getTimeBirth() > rt.getTimeBirth();
}

//���� == ���������Ƚ� ID������ȥ��
bool operator == (const CData lt, const CData rt)
{
	return lt.getID() == rt.getID();
}

//���ز����� == ���ڸ��� ID �ж� identical
bool operator == (int id, const CData data)
{
	return data.getID() == id;
}
bool operator == (const CData data, int id)
{
	return data.getID() == id;
}


CCtrl::CCtrl()
{
	init();
}

void CCtrl::init()
{
	CPacket::init();
	this->HOP = 0;
	this->MAX_HOP = 1;
	this->type = _rts;
	this->capacity = 0;
}

CCtrl::CCtrl(int node, int timeBirth, int byte, EnumCtrlType type)
{
	init();
	this->node = node;
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, vector<CData> datas, int timeBirth, int byte, EnumCtrlType type)
{
	init();
	this->node = node;
	if(type == _ack)
	{
		this->ack = datas;
	}
	else
		throw string("CCtrl::CCtrl(): `type` must be _ack for this constructor.");
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}

CCtrl::CCtrl(int node, int capacity, int timeBirth, int byte, EnumCtrlType type)
{
	init();
	this->node = node;
	if(type == _capacity)
	{
		this->capacity = capacity;
	}
	else
		throw string("CCtrl::CCtrl(): `type` must be _capacity for this constructor.");
	this->time = this->timeBirth = timeBirth;
	this->size = byte;
	this->type = type;
}


CFrame::CFrame() : src(nullptr), dst(nullptr)
{
	init();
}

CFrame::CFrame(CGeneralNode& src, CGeneralNode& dst, vector<CPacket*> packets) : src(&src), dst(&dst)
{
	init();
	this->packets = packets;
}

CFrame::CFrame(CGeneralNode & src, vector<CPacket*> packets) : src(&src), dst(nullptr)
{
	init();
	this->packets = packets;
}

CFrame::~CFrame()
{
	FreePointerVector(packets);
	packets.clear();
}

void CFrame::init()
{
	this->headerMac = getConfig<int>("data", "size_header_mac");
}

int CFrame::getSize() const
{
	return headerMac + CPacket::getSumSize(packets);
}
