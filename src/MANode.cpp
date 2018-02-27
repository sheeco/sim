#include "MANode.h"
#include "HAR.h"

int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;
int CMANode::COUNT_ID = 0;  //从getConfig<int>("ma", "base_id")开始，差值等于当前实例总数


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
