#include "MANode.h"

// TODO: read from xml instead of constant initial value
int CMANode::COUNT_ID = 0;  //从1开始，数值等于当前实例总数
vector<CMANode *> CMANode::MANodes;
vector<CMANode *> CMANode::freeMANodes;

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

void CMANode::updateLocation(int time)
{
	int interval = time - this->time;
	if(waitingTime == 0)
		waitingTime = -1;
	if(waitingTime > 0)
	{
		//如果路线过期，立即结束waiting
		if( route.isOverdue() )
		{
			waitingTime = 0;
			this->setTime(time);
			return;
		}
		//waiting阶段还未结束
		else if(interval <= waitingTime)
		{
			waitingTime -= interval;
			this->setTime(time);
			return;
		}
		//waiting阶段将结束，继续移动
		else
		{
			interval -= waitingTime;
			waitingTime = 0;
		}
	}
	if(interval == 0)
		return;
	atHotspot = nullptr;
	//路线过期，立即返回sink
	if( route.isOverdue() )
	{
		this->moveTo( *static_cast<CBasicEntity *>(CSink::getSink()) , interval, SPEED);
		return ;
	}
	//在路线上正常移动
	else
	{
		CBasicEntity *toPoint = route.getToPoint();
		bool arrival = this->moveTo(*toPoint , interval, SPEED);

		//如果已到达目的地
		if(arrival)
		{
			//若目的地的类型是hotspot
			if( toPoint != CSink::getSink() )
			{
				this->atHotspot = static_cast<CHotspot *>(toPoint);

				if( atHotspot->getTypeHotspotCandidate() > 3 )
				{
					cout << endl << "Error @ CMANode::updateLocation : atHotspot is corrupted !" << endl;
					_PAUSE_;
				}
			}
			route.updateToPoint();
			return ;
		}
	}
	//更新时间戳
	this->setTime(time);
}