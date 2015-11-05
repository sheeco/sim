#include "MANode.h"

double CMANode::energyConsumption = 0;
long int CMANode::ID_COUNT = 0;
int CMANode::encounter = 0;
int CMANode::encounterAtHotspots = 0;
int CMANode::encounterOnTheWay = 0;


void CMANode::receiveData(int time, vector<CData> data)
{
	if(buffer.size() > BUFFER_CAPACITY_MA)
	{
		cout<<"Error: CNode::generateData() buffer overflown"<<endl;
		_PAUSE;
		return;
	}
	int num = data.size();
	//���޴��Buffer
	if( INFINITE_BUFFER )
	{
		for(int i = 0; i < num; i++)
		{
			buffer.push_back(data[i]);
		}
		energyConsumption += num * (CONSUMPTION_DATA_SEND * 4 + CONSUMPTION_DATA_RECIEVE * 3);
		return;
	}
	//�����������������Buffer�п���ʱ�Ž�������
	else if( ! BUFFER_OVERFLOW_ALLOWED )
	{
		if(buffer.size() == BUFFER_CAPACITY_MA)
			return;
		if(data.size() + buffer.size() > BUFFER_CAPACITY_MA)
			num = BUFFER_CAPACITY_MA - buffer.size();
	}
	for(int i = 0; i < num; i++)
	{
		if(buffer.size() == BUFFER_CAPACITY_MA)
			buffer.erase(buffer.begin());  //���buffer������ɾ�������һ��Data
		buffer.push_back(data[i]);
		//FIXME: ����MA����Ϊ�����ѳɹ�Ͷ��
		//data[i].arriveSink(time);
	}
	energyConsumption += num * (CONSUMPTION_DATA_SEND * 4 + CONSUMPTION_DATA_RECIEVE * 3);
}

vector<CData>  CMANode::sendAllData()
{
	vector<CData>  data;
	if(buffer.empty())
		return data;
	else
	{
		energyConsumption += buffer.size() * (CONSUMPTION_DATA_SEND * 2 + CONSUMPTION_DATA_RECIEVE * 1);
		data = buffer;
		buffer.clear();
		return data;
	}
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
			atHotspot = NULL;
		}
	}
	if(interval == 0)
		return;
	//·�߹��ڣ���������sink
	if( route.isOverdue() )
	{
		double timeReachSink = CBase::getDistance( *(CBase *)this , *(CBase *)CSink::getSink() ) / SPEED_MANODE;
		//��timeʱ�̲��ᵽ��sink
		if( interval < timeReachSink )
		{
			CBase::moveTo( *(CBase *)this , *(CBase *)CSink::getSink() , interval, SPEED_MANODE);
		}
		//������sink
		else
		{
			//FIXME: ʱ��������������룬�˴���������
			if(timeReachSink < 1)
				timeReachSink = 1;
			int timeArrival = ROUND( this->time + timeReachSink );
			this->setLocation(SINK_X, SINK_Y, timeArrival);
			return;
		}
	}
	//��·���������ƶ�
	else
	{
		CBase *toPoint = route.getToPoint();
		double timeReachToPoint = CBase::getDistance( *(CBase *)this , *toPoint ) / SPEED_MANODE;
		//��timeʱ�̲��ᵽ��toPoint
		if( interval < timeReachToPoint )
		{
			CBase::moveTo( *(CBase *)this , *toPoint , interval, SPEED_MANODE);
		}
		//������toPoint
		else
		{
			//FIXME: ʱ��������������룬�˴���������
			if(timeReachToPoint < 1)
				timeReachToPoint = 1;
			int timeArrival = ROUND( this->time + timeReachToPoint );
			this->setLocation(toPoint->getX(), toPoint->getY(), timeArrival);
			//������ĵ���hotspot
			if( toPoint != CSink::getSink() )
			{
				this->atHotspot = (CHotspot *)toPoint;
			}
			route.updateToPoint();
			return;
		}
	}
	//����ʱ���
	this->setTime(time);
}