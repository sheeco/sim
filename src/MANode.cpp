#include "MANode.h"
#include "HAR.h"

// TODO: read from xml instead of constant initial value
int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;
int CMANode::COUNT_ID = 0;  //从START_COUNT_ID开始，差值等于当前实例总数
vector<CMANode *> CMANode::MANodes;
vector<CMANode *> CMANode::freeMANodes;

int CMANode::START_COUNT_ID = 0;  //ID的起始值，用于和传感器节点相区分
int CMANode::SPEED = 0;
int CMANode::CAPACITY_BUFFER = 0;
CGeneralNode::_RECEIVE CMANode::MODE_RECEIVE = _selfish;


//bool CMANode::receiveData(int time, vector<CData> datas)
//{
//	if(buffer.size() > CAPACITY_BUFFER)
//	{
//		cout << endl << "Error @ CMANode::receiveData() : buffer overflown"<<endl;
//		_PAUSE_;
//		return false;
//	}
//	int num = datas.size();
//
//	//不允许溢出，即仅在Buffer有空余时才接收数据
//	if( MODE_RECEIVE == _selfish)
//	{
//		if(buffer.size() == CAPACITY_BUFFER)
//			return false;
//		if(datas.size() + buffer.size() > CAPACITY_BUFFER)
//			num = CAPACITY_BUFFER - buffer.size();
//	}
//	for(int i = 0; i < num; ++i)
//	{
//		if(buffer.size() == CAPACITY_BUFFER)
//			buffer.erase(buffer.begin());  //如果buffer已满，删除最早的一个Data
//		buffer.push_back(datas[i]);
//	}
//	energyConsumption += num * (CONSUMPTION_BYTE_SEND * 4 + CONSUMPTION_BYTE_RECEIVE * 3);
//	return true;
//}

// TODO: need test
void CMANode::updateStatus(int time)
{
	int interval = time - this->time;
	//更新时间戳

	//路线过期或缓存已满，立即返回sink
	if( route.isOverdue()
		|| ( MODE_RECEIVE == _selfish
		&& buffer.size() >= capacityBuffer ) )
	{
		waitingWindow = waitingState = 0;
		this->moveTo( *static_cast<CBasicEntity *>(CSink::getSink()) , interval, SPEED);
		return ;
	}

	//等待结束
	if( waitingWindow > 0 )
	{
		if( (waitingState + interval) >= waitingWindow )
		{
			interval = waitingState + interval - waitingWindow;
			waitingWindow = waitingState = 0;
		}

		//等待还未结束
		else
		{
			waitingState += interval;
			interval = 0;
		}
	}
	if( interval == 0 )
	{
		this->setTime(time);
		return;
	}
	else
		this->setTime( time - interval );

	//在路线上正常移动
	atHotspot = nullptr;

	CBasicEntity *toPoint = route.getToPoint();
	bool arrival = this->moveTo(*toPoint , interval, SPEED);

	//如果已到达目的地
	if(arrival)
	{
		//若目的地的类型是hotspot
		if( typeid(*toPoint) == typeid(CHotspot) )
		{
			this->atHotspot = static_cast<CHotspot *>(toPoint);
			waitingWindow = HAR::calculateWaitingTime(time, this->atHotspot);
			waitingState = 0;  //重新开始等待
		}
		else if( typeid(*toPoint) == typeid(CSink) )
		{
			if( CSink::hasMoreNewRoutes() )
				this->route = CSink::popRoute();
			else
				this->turnFree();
		}
		route.updateToPoint();
	}
	return ;

}

CPackage* CMANode::sendRTSWithCapacity(int currentTime) 
{
	vector<CGeneralData*> contents;
	contents.push_back( new CCtrl(ID, currentTime, SIZE_CTRL, CCtrl::_rts) );
	if( MODE_RECEIVE == _selfish 
		&& ( ! buffer.empty() ) )
		contents.push_back( new CCtrl(ID, capacityBuffer - buffer.size(), currentTime, SIZE_CTRL, CCtrl::_capacity) );

	CPackage* package = new CPackage(*this, CGeneralNode(), contents);

	return package;
}

vector<CData> CMANode::bufferData(int time, vector<CData> datas)
{
	vector<CData> ack = datas;
	RemoveFromList( datas, this->buffer );
	for(auto idata = datas.begin(); idata != datas.end(); ++idata)
	{
//		idata->arriveSink(time);
		this->buffer.push_back(*idata);
	}
	
	return ack;
}

void CMANode::checkDataByAck(vector<CData> ack)
{
	RemoveFromList(buffer, ack);
}
