#include "GeneralNode.h"
#include "SortHelper.h"
#include "Configuration.h"


CFrame * CGeneralNode::sendRTS(int now)
{
	vector<CPacket*> packets;

	packets.push_back(new CCtrl(ID, now, CCtrl::_rts));
	CFrame* frame = new CFrame(*this, packets);

	return frame;
}

void CGeneralNode::Overhear(int now)
{
	consumeEnergy(getConfig<double>("trans", "consumption_byte_receive") * getConfig<int>("data", "size_header_mac"), now);
}

int CGeneralNode::pushIntoBuffer(vector<CData> datas, int now)
{
	int ndata = this->getBufferSize();
	for( CData data : datas )
	{
		data.arriveAnotherNode(now);
		this->buffer = CSortHelper::insertIntoSortedList(this->buffer, data, CSortHelper::ascendByTimeBirth, CSortHelper::descendByTimeBirth);
	}
	return this->getBufferSize() - ndata;
}

vector<CData> CGeneralNode::getDataForTrans(int capacity)
{
	int window = getConfig<int>("trans", "size_window");
	if(capacity == 0)
		throw string("CGeneralNode::getDataForTrans() capacity = 0.");
	else if(capacity < 0
			|| capacity > window)
		capacity = window;

	vector<CData> datas = this->getAllData();
	clipDataByCapacity(datas, capacity, !this->fifo);
	return datas;
}

//选择对方没有的数据用于发送

vector<CData> CGeneralNode::getDataForTrans(vector<CData> except, int capacity)
{
	if( capacity == 0 )
		return vector<CData>();

	vector<CData> mine = this->buffer;
	int window = getConfig<int>("trans", "size_window");
	if( capacity == 0 )
		throw string("CGeneralNode::getDataForTrans() capacity = 0.");
	else if( capacity < 0
			|| capacity > window )
		capacity = window;

	RemoveFromList(mine, except);
	CNode::clipDataByCapacity(mine, capacity, !fifo);
	return mine;
}
