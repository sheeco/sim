//#include "MANode.h"
//
//double CMANode::energyConsumption = 0;
//int CMANode::ID_COUNT = 0;
//int CMANode::encounter = 0;
//int CMANode::encounterAtHotspot = 0;
//int CMANode::encounterOnRoute = 0;
//
//
//void CMANode::receiveData(int time, vector<CData> data)
//{
//	if(buffer.size() > BUFFER_CAPACITY_MA)
//	{
//		cout<<"Error @ CMANode::receiveData() : buffer overflown"<<endl;
//		_PAUSE;
//		return;
//	}
//	int num = data.size();
//	//无限大的Buffer
//	if( INFINITE_BUFFER )
//	{
//		for(int i = 0; i < num; i++)
//		{
//			buffer.push_back(data[i]);
//		}
//		energyConsumption += num * (CONSUMPTION_DATA_SEND * 4 + CONSUMPTION_DATA_RECIEVE * 3);
//		return;
//	}
//	//不允许溢出，即仅在Buffer有空余时才接收数据
//	else if( ! BUFFER_OVERFLOW_ALLOWED )
//	{
//		if(buffer.size() == BUFFER_CAPACITY_MA)
//			return;
//		if(data.size() + buffer.size() > BUFFER_CAPACITY_MA)
//			num = BUFFER_CAPACITY_MA - buffer.size();
//	}
//	for(int i = 0; i < num; i++)
//	{
//		if(buffer.size() == BUFFER_CAPACITY_MA)
//			buffer.erase(buffer.begin());  //如果buffer已满，删除最早的一个Data
//		buffer.push_back(data[i]);
//	}
//	energyConsumption += num * (CONSUMPTION_DATA_SEND * 4 + CONSUMPTION_DATA_RECIEVE * 3);
//}
//
//vector<CData>  CMANode::sendAllData()
//{
//	vector<CData>  data;
//	if(buffer.empty())
//		return data;
//	else
//	{
//		energyConsumption += buffer.size() * (CONSUMPTION_DATA_SEND * 2 + CONSUMPTION_DATA_RECIEVE * 1);
//		data = buffer;
//		buffer.clear();
//		return data;
//	}
//}
//
//void CMANode::updateLocation(int time)
//{
//	int interval = time - this->time;
//	if(waitingTime == 0)
//		waitingTime = -1;
//	if(waitingTime > 0)
//	{
//		//如果路线过期，立即结束waiting
//		if( route.isOverdue() )
//		{
//			waitingTime = 0;
//			this->setTime(time);
//			return;
//		}
//		//waiting阶段还未结束
//		else if(interval <= waitingTime)
//		{
//			waitingTime -= interval;
//			this->setTime(time);
//			return;
//		}
//		//waiting阶段将结束，继续移动
//		else
//		{
//			interval -= waitingTime;
//			waitingTime = 0;
//		}
//	}
//	if(interval == 0)
//		return;
//	atHotspot = NULL;
//	//路线过期，立即返回sink
//	if( route.isOverdue() )
//	{
//		double timeReachSink = CBasicEntity::getDistance( *(CBasicEntity *)this , *(CBasicEntity *)CSink::getSink() ) / SPEED_MANODE;
//		//若time时刻不会到达sink
//		if( interval < timeReachSink )
//		{
//			CBasicEntity::moveTo( *(CBasicEntity *)this , *(CBasicEntity *)CSink::getSink() , interval, SPEED_MANODE);
//		}
//		//将到达sink
//		else
//		{
//			//FIXME: 时间进行了四舍五入，此处将产生误差！
//			if(timeReachSink < 1)
//				timeReachSink = 1;
//			int timeArrival = ROUND( this->time + timeReachSink );
//			this->setLocation(SINK_X, SINK_Y, timeArrival);
//			return;
//		}
//	}
//	//在路线上正常移动
//	else
//	{
//		CBasicEntity *toPoint = route.getToPoint();
//		double timeReachToPoint = CBasicEntity::getDistance( *(CBasicEntity *)this , *toPoint ) / SPEED_MANODE;
//		//若time时刻不会到达toPoint
//		if( interval < timeReachToPoint )
//		{
//			CBasicEntity::moveTo( *(CBasicEntity *)this , *toPoint , interval, SPEED_MANODE);
//		}
//		//将到达toPoint
//		else
//		{
//			//FIXME: 时间进行了四舍五入，此处将产生误差！
//			if(timeReachToPoint < 1)
//				timeReachToPoint = 1;
//			int timeArrival = ROUND( this->time + timeReachToPoint );
//			this->setLocation(toPoint->getX(), toPoint->getY(), timeArrival);
//			//若到达的点是hotspot
//			if( toPoint != CSink::getSink() )
//			{
//				this->atHotspot = (CHotspot *)toPoint;
//				if( atHotspot->getCandidateType() > 3 )
//					_PAUSE;
//			}
//			route.updateToPoint();
//			return;
//		}
//	}
//	//更新时间戳
//	this->setTime(time);
//}