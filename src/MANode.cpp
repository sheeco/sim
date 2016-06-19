#include "MANode.h"
#include "HAR.h"

// TODO: read from xml instead of constant initial value
int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;
int CMANode::COUNT_ID = 0;  //��START_COUNT_ID��ʼ����ֵ���ڵ�ǰʵ������
vector<CMANode *> CMANode::MANodes;
vector<CMANode *> CMANode::freeMANodes;

int CMANode::START_COUNT_ID = 0;  //ID����ʼֵ�����ںʹ������ڵ�������
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
//	//�����������������Buffer�п���ʱ�Ž�������
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
//			buffer.erase(buffer.begin());  //���buffer������ɾ�������һ��Data
//		buffer.push_back(datas[i]);
//	}
//	energyConsumption += num * (CONSUMPTION_BYTE_SEND * 4 + CONSUMPTION_BYTE_RECEIVE * 3);
//	return true;
//}

// TODO: need test
void CMANode::updateStatus(int time)
{
	if( this->time < 0 )
		this->time = time;
	int interval = time - this->time;

	//updateTimerOccupied(time);

	//����ʱ���

	//·�߹��ڻ򻺴���������������sink
	if( route.isOverdue()
		|| ( MODE_RECEIVE == _selfish
		&& buffer.size() >= capacityBuffer ) )
	{
		waitingWindow = waitingState = 0;
		route.updateToPointWithSink();
	}

	//���ڵȴ���
	if( waitingWindow > 0 )
	{
		//�ȴ���������		
		if( (waitingState + interval) >= waitingWindow )
		{
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
	bool arrival = this->moveTo(*toPoint , interval, SPEED);

	//����ѵ���Ŀ�ĵ�
	if(arrival)
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
	packets.push_back( new CCtrl(ID, currentTime, SIZE_CTRL, CCtrl::_rts) );
	if( MODE_RECEIVE == _selfish 
		&& ( ! buffer.empty() ) )
		packets.push_back( new CCtrl(ID, capacityBuffer - buffer.size(), currentTime, SIZE_CTRL, CCtrl::_capacity) );

	CFrame* frame = new CFrame(*this, packets);

	return frame;
}

vector<CData> CMANode::bufferData(int time, vector<CData> datas)
{
	if( isAtHotspot() )
		this->getAtHotspot()->addDeliveryCount(datas.size());

	vector<CData> ack = datas;
	RemoveFromList( datas, this->buffer );
	for(auto idata = datas.begin(); idata != datas.end(); ++idata)
	{
		this->buffer.push_back(*idata);
	}
	
	return ack;
}

void CMANode::checkDataByAck(vector<CData> ack)
{
	RemoveFromList(buffer, ack);
}
