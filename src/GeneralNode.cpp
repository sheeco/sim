#include "GeneralNode.h"
#include "SortHelper.h"
#include "Configuration.h"


CFrame * CGeneralNode::sendRTS(int now)
{
	vector<CPacket*> packets;

	packets.push_back(new CCtrl(ID, now, getConfig<int>("data", "size_ctrl"), CCtrl::_rts));
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
	int window = getConfig<int>("trans", "window_trans");
	if(capacity == 0)
		throw string("CGeneralNode::getDataForTrans() capacity = 0.");
	else if(capacity < 0
			|| capacity > window)
		capacity = window;

	vector<CData> datas = this->getAllData();
	clipDataByCapacity(datas, capacity, !this->fifo);
	return datas;
}
