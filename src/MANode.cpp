#include "MANode.h"
#include "HAR.h"

int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;
int CMANode::COUNT_ID = 0;  //��configs.ma.START_COUNT_ID��ʼ����ֵ���ڵ�ǰʵ������
vector<CMANode *> CMANode::MANodes;
vector<CMANode *> CMANode::freeMANodes;


//bool CMANode::receiveData(int time, vector<CData> datas)
//{
//	if(buffer.size() > configs.ma.CAPACITY_BUFFER)
//	{
//		throw string("CMANode::receiveData() : buffer overflown");
//	}
//	int num = datas.size();
//
//	//�����������������Buffer�п���ʱ�Ž�������
//	if( MODE_RECEIVE == config::_selfish)
//	{
//		if(buffer.size() == configs.ma.CAPACITY_BUFFER)
//			return false;
//		if(datas.size() + buffer.size() > configs.ma.CAPACITY_BUFFER)
//			num = configs.ma.CAPACITY_BUFFER - buffer.size();
//	}
//	for(int i = 0; i < num; ++i)
//	{
//		if(buffer.size() == configs.ma.CAPACITY_BUFFER)
//			buffer.erase(buffer.begin());  //���buffer������ɾ�������һ��Data
//		buffer.push_back(datas[i]);
//	}
//	energyConsumption += num * (configs.trans.CONSUMPTION_BYTE_SEND * 4 + configs.trans.CONSUMPTION_BYTE_RECEIVE * 3);
//	return true;
//}

void CMANode::updateStatus(int time)
{
	if( this->time < 0 )
		this->time = time;
	int interval = time - this->time;

	//updateTimerOccupied(time);

	//����ʱ���

	//·�߹��ڻ򻺴���������������sink
	if( route.isOverdue()
		|| ( configs.ma.SCHEME_RELAY == config::_selfish
		&& buffer.size() >= capacityBuffer ) )
	{
		//��¼һ��δ�������ڵĵȴ�������¼��(t, 0)����˵������·�߹��ڶ������ȴ�
		if( isAtHotspot() )
			getAtHotspot()->recordWaitingTime(time - waitingState, waitingState);

		waitingWindow = waitingState = 0;
		route.updateToPointWithSink();
	}

	//���ڵȴ���
	if( waitingWindow > 0 )
	{
		//�ȴ���������		
		if( (waitingState + interval) >= waitingWindow )
		{
			//��¼һ�������ڵĵȴ�
			getAtHotspot()->recordWaitingTime(time - waitingWindow, waitingWindow);

			interval = waitingState + interval - waitingWindow;  //�ȴ�������ʣ��Ľ������ƶ���ʱ��
			waitingWindow = waitingState = 0;  //�ȴ���������ʱ�䴰����
		}

		//�ȴ���δ����
		else
		{
			waitingState += interval;
			interval = 0;  //���ƶ�
		}
	}
	if( interval == 0 )
	{
		this->setTime(time);
		return;
	}
	
	//��ʼ��·�����ƶ�
	this->setTime( time - interval );  //��ʱ�����ڵȴ��������ƶ�������ʼ��ʱ���
	atHotspot = nullptr;  //�뿪�ȵ�

	CBasicEntity *toPoint = route.getToPoint();
	int timeLeftAfterArrival = this->moveTo(*toPoint , interval, configs.ma.SPEED);

	//����ѵ���Ŀ�ĵ�
	if( timeLeftAfterArrival >= 0 )
	{

		//��Ŀ�ĵص������� hotspot
		if( dynamic_cast<CHotspot *>( toPoint ) != nullptr )
		{
			this->atHotspot = dynamic_cast<CHotspot *>(toPoint);
			waitingWindow = int( HAR::calculateWaitingTime(time, this->atHotspot) );
			waitingState = 0;  //���¿�ʼ�ȴ�

			route.updateToPoint();
		}
		//��Ŀ�ĵص������� sink
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
	packets.push_back( new CCtrl(ID, currentTime, configs.data.SIZE_CTRL, CCtrl::_rts) );
	if( configs.ma.SCHEME_RELAY == config::_selfish 
		&& ( ! buffer.empty() ) )
		packets.push_back( new CCtrl(ID, capacityBuffer - buffer.size(), currentTime, configs.data.SIZE_CTRL, CCtrl::_capacity) );

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
		////��Ϊ���� MA �ڵ㼴���� sink
		//idata->arriveSink(time);
		this->buffer.push_back(*idata);
	}
	
	return ack;
}

void CMANode::checkDataByAck(vector<CData> ack)
{
	RemoveFromList(buffer, ack);
}
