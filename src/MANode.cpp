#include "MANode.h"
#include "HAR.h"

int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;
int CMANode::COUNT_ID = 0;  //��getConfig<int>("ma", "base_id")��ʼ����ֵ���ڵ�ǰʵ������


//bool CMANode::receiveData(int time, vector<CData> datas)
//{
//	if(buffer.size() > getConfig<int>("ma", "buffer"))
//	{
//		throw string("CMANode::receiveData() : buffer overflown");
//	}
//	int num = datas.size();
//
//	//�����������������Buffer�п���ʱ�Ž�������
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
//			buffer.erase(buffer.begin());  //���buffer������ɾ�������һ��Data
//		buffer.push_back(datas[i]);
//	}
//	energyConsumption += num * (getConfig<double>("trans", "consumption_byte_send") * 4 + getConfig<double>("trans", "consumption_byte_receive") * 3);
//	return true;
//}
