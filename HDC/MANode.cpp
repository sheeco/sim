#include "MANode.h"

int CMANode::ID_COUNT = 0;  //从1开始，数值等于当前实例总数
vector<CMANode *> CMANode::MANodes;
vector<CMANode *> CMANode::freeMANodes;
Mode CMANode::BUFFER_MODE = BUFFER::SELFISH;

bool CMANode::receiveData(int time, vector<CData> datas)
{
	if(buffer.size() > BUFFER_CAPACITY_MA)
	{
		cout<<"Error @ CMANode::receiveData() : buffer overflown"<<endl;
		_PAUSE;
		return false;
	}
	int num = datas.size();

	//不允许溢出，即仅在Buffer有空余时才接收数据
	if( BUFFER_MODE == BUFFER::SELFISH )
	{
		if(buffer.size() == BUFFER_CAPACITY_MA)
			return false;
		if(datas.size() + buffer.size() > BUFFER_CAPACITY_MA)
			num = BUFFER_CAPACITY_MA - buffer.size();
	}
	for(int i = 0; i < num; i++)
	{
		if(buffer.size() == BUFFER_CAPACITY_MA)
			buffer.erase(buffer.begin());  //如果buffer已满，删除最早的一个Data
		buffer.push_back(datas[i]);
	}
	energyConsumption += num * (CONSUMPTION_BYTE_SEND * 4 + CONSUMPTION_BYTE_RECIEVE * 3);
	return true;
}

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
		this->moveTo( *static_cast<CBasicEntity *>(CSink::getSink()) , interval, SPEED_MANODE);
		return ;
	}
	//在路线上正常移动
	else
	{
		CBasicEntity *toPoint = route.getToPoint();
		bool arrival = this->moveTo(*toPoint , interval, SPEED_MANODE);

		//如果已到达目的地
		if(arrival)
		{
			//若目的地的类型是hotspot
			if( toPoint != CSink::getSink() )
			{
				this->atHotspot = static_cast<CHotspot *>(toPoint);

				if( atHotspot->getCandidateType() > 3 )
				{
					cout << "Error @ CMANode::updateLocation : atHotspot is corrupted !" << endl;
					_PAUSE;
				}
			}
			route.updateToPoint();
			return ;
		}
	}
	//更新时间戳
	this->setTime(time);
}