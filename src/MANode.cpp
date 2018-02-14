#include "MANode.h"
#include "HAR.h"

int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;
int CMANode::COUNT_ID = 0;  //从getConfig<int>("ma", "base_id")开始，差值等于当前实例总数
vector<CMANode *> CMANode::MANodes;
vector<CMANode *> CMANode::freeMANodes;


//bool CMANode::receiveData(int time, vector<CData> datas)
//{
//	if(buffer.size() > getConfig<int>("ma", "buffer"))
//	{
//		throw string("CMANode::receiveData() : buffer overflown");
//	}
//	int num = datas.size();
//
//	//不允许溢出，即仅在Buffer有空余时才接收数据
//	if( MODE_RECEIVE == config::_selfish)
//	{
//		if(buffer.size() == getConfig<int>("ma", "buffer"))
//			return false;
//		if(datas.size() + buffer.size() > getConfig<int>("ma", "buffer"))
//			num = getConfig<int>("ma", "buffer") - buffer.size();
//	}
//	for(int i = 0; i < num; ++i)
//	{
//		if(buffer.size() == getConfig<int>("ma", "buffer"))
//			buffer.erase(buffer.begin());  //如果buffer已满，删除最早的一个Data
//		buffer.push_back(datas[i]);
//	}
//	energyConsumption += num * (getConfig<double>("trans", "consumption_byte_send") * 4 + getConfig<double>("trans", "consumption_byte_receive") * 3);
//	return true;
//}

void CMANode::updateStatus(int time)
{
	if( this->time < 0 )
		this->time = time;
	int interval = time - this->time;

	//updateTimerOccupied(time);

	//更新时间戳

	//路线过期或缓存已满，立即返回sink
	if( route.isOverdue()
		|| ( getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_selfish
		&& buffer.size() >= capacityBuffer ) )
	{
		//记录一次未填满窗口的等待；可能录入(t, 0)，即说明由于路线过期而跳过等待
		if( isAtHotspot() )
			getAtHotspot()->recordWaitingTime(time - waitingState, waitingState);

		waitingWindow = waitingState = 0;
		route.updateToPointWithSink();
	}

	//处于等待中
	if( waitingWindow > 0 )
	{
		//等待即将结束		
		if( (waitingState + interval) >= waitingWindow )
		{
			//记录一次满窗口的等待
			getAtHotspot()->recordWaitingTime(time - waitingWindow, waitingWindow);

			interval = waitingState + interval - waitingWindow;  //等待结束后，剩余的将用于移动的时间
			waitingWindow = waitingState = 0;  //等待结束，将时间窗重置
		}

		//等待还未结束
		else
		{
			waitingState += interval;
			interval = 0;  //不移动
		}
	}
	if( interval == 0 )
	{
		this->setTime(time);
		return;
	}
	
	//开始在路线上移动
	this->setTime( time - interval );  //将时间置于等待结束，移动即将开始的时间点
	atHotspot = nullptr;  //离开热点

	CBasicEntity *toPoint = route.getToPoint();
	int timeLeftAfterArrival = this->moveTo(*toPoint , interval, getConfig<int>("ma", "speed"));

	//如果已到达目的地
	if( timeLeftAfterArrival >= 0 )
	{

		//若目的地的类型是 hotspot
		if( dynamic_cast<CHotspot *>( toPoint ) != nullptr )
		{
			this->atHotspot = dynamic_cast<CHotspot *>(toPoint);
			waitingWindow = int( HAR::calculateWaitingTime(time, this->atHotspot) );
			waitingState = 0;  //重新开始等待

			route.updateToPoint();
		}
		//若目的地的类型是 sink
		else if( dynamic_cast<CSink *>( toPoint ) != nullptr )
		{
			if( route.isOverdue() )
			{
			    if( CSink::hasMoreNewRoutes() )
					this->route = CSink::popRoute();
				else
					this->turnFree();
			}
			else
				route.updateToPoint();
		}
	}
	return;

}

CFrame* CMANode::sendRTSWithCapacity(int currentTime) 
{
	vector<CPacket*> packets;
	packets.push_back( new CCtrl(ID, currentTime, getConfig<int>("data", "size_ctrl"), CCtrl::_rts) );
	if( getConfig<CConfiguration::EnumRelayScheme>("ma", "scheme_relay") == config::_selfish 
		&& ( ! buffer.empty() ) )
		packets.push_back( new CCtrl(ID, capacityBuffer - buffer.size(), currentTime, getConfig<int>("data", "size_ctrl"), CCtrl::_capacity) );

	CFrame* frame = new CFrame(*this, packets);

	return frame;
}

vector<CData> CMANode::bufferData(int time, vector<CData> datas)
{
	vector<CData> ack = datas;
	RemoveFromList( datas, this->buffer );

	bool atHotspot = isAtHotspot();
	if( atHotspot )
	{
		this->getAtHotspot()->recordCountDelivery(datas.size());
		CData::deliverAtHotspot(datas.size());
	}
	else
		CData::deliverOnRoute(datas.size());

	for( auto idata = datas.begin(); idata != datas.end(); ++idata )
	{
		////认为到达 MA 节点即到达 sink
		//idata->arriveSink(time);
		this->buffer.push_back(*idata);
	}
	
	return ack;
}

void CMANode::checkDataByAck(vector<CData> ack)
{
	RemoveFromList(buffer, ack);
}
