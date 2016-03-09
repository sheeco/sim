#include "MANode.h"

//TODO: read from xml instead of constant initial value
int CMANode::ID_COUNT = 0;  //��1��ʼ����ֵ���ڵ�ǰʵ������
int CMANode::SPEED = 30;
int CMANode::BUFFER_CAPACITY = 100;
vector<CMANode *> CMANode::MANodes;
vector<CMANode *> CMANode::freeMANodes;
CGeneralNode::_RECEIVE CMANode::RECEIVE_MODE = _selfish;


bool CMANode::receiveData(int time, vector<CData> datas)
{
	if(buffer.size() > BUFFER_CAPACITY)
	{
		cout << endl << "Error @ CMANode::receiveData() : buffer overflown"<<endl;
		_PAUSE_;
		return false;
	}
	int num = datas.size();

	//�����������������Buffer�п���ʱ�Ž�������
	if( RECEIVE_MODE == _selfish)
	{
		if(buffer.size() == BUFFER_CAPACITY)
			return false;
		if(datas.size() + buffer.size() > BUFFER_CAPACITY)
			num = BUFFER_CAPACITY - buffer.size();
	}
	for(int i = 0; i < num; i++)
	{
		if(buffer.size() == BUFFER_CAPACITY)
			buffer.erase(buffer.begin());  //���buffer������ɾ�������һ��Data
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
		//���·�߹��ڣ���������waiting
		if( route.isOverdue() )
		{
			waitingTime = 0;
			this->setTime(time);
			return;
		}
		//waiting�׶λ�δ����
		else if(interval <= waitingTime)
		{
			waitingTime -= interval;
			this->setTime(time);
			return;
		}
		//waiting�׶ν������������ƶ�
		else
		{
			interval -= waitingTime;
			waitingTime = 0;
		}
	}
	if(interval == 0)
		return;
	atHotspot = nullptr;
	//·�߹��ڣ���������sink
	if( route.isOverdue() )
	{
		this->moveTo( *static_cast<CBasicEntity *>(CSink::getSink()) , interval, SPEED);
		return ;
	}
	//��·���������ƶ�
	else
	{
		CBasicEntity *toPoint = route.getToPoint();
		bool arrival = this->moveTo(*toPoint , interval, SPEED);

		//����ѵ���Ŀ�ĵ�
		if(arrival)
		{
			//��Ŀ�ĵص�������hotspot
			if( toPoint != CSink::getSink() )
			{
				this->atHotspot = static_cast<CHotspot *>(toPoint);

				if( atHotspot->getCandidateType() > 3 )
				{
					cout << endl << "Error @ CMANode::updateLocation : atHotspot is corrupted !" << endl;
					_PAUSE_;
				}
			}
			route.updateToPoint();
			return ;
		}
	}
	//����ʱ���
	this->setTime(time);
}