#include "Epidemic.h"
#include "Node.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Sink.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "HDC.h"

int Epidemic::MAX_QUEUE_SIZE = CNode::BUFFER_CAPACITY;
int Epidemic::SPOKEN_MEMORY = 0;

void Epidemic::UpdateNodeStatus(int currentTime)
{
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
	{
		(*inode)->updateStatus(currentTime);
	}
	//调用下层协议HDC，判断是否位于热点区域，更新占空比
	if( DO_HDC )
		CHDC::UpdateDutyCycleForNodes(currentTime);
}

void Epidemic::GenerateData(int currentTime)
{
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
	{
		(*inode)->generateData(currentTime);
	}
}

void Epidemic::SendData(int currentTime)
{
	//本地将所有node按照x坐标排序
	vector<CNode *> nodes = CNode::getNodes();
	CPreprocessor::mergeSort(nodes);

	//判断工作状态，向sink投递数据，节点间通信
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	{
		//如果处于休眠状态，直接跳过
		if( ! (*inode)->isListening() )
			continue;

		if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= TRANS_RANGE )
		{
			//deliver data to sink
			cout << endl << "####  [ Node " << (*inode)->getID() << " delivers " << (*inode)->getBufferSize() << " data to Sink ]" << endl;
			CSink::getSink()->receiveData( (*inode)->deliverAllData(), currentTime );
		}

		//scan other nodes and forward data
		for(vector<CNode *>::iterator jnode = nodes.begin(); jnode != nodes.end(); jnode++)
		{
			if( inode == jnode )
				continue;
			if( (*jnode)->getX() + TRANS_RANGE < (*inode)->getX() )
				continue;
			if( (*inode)->getX() + TRANS_RANGE < (*jnode)->getX() )
				break;
			if( CBasicEntity::getDistance( **inode, **jnode ) > TRANS_RANGE )
				continue;

			//if spoken recently, skip
			if( (*inode)->hasSpokenRecently( (*jnode), currentTime) )
				continue;

			//init by node with smaller id
			CNode *smaller, *greater = NULL;
			smaller = (*inode)->getID() < (*jnode)->getID() ? *inode : *jnode ;
			greater = (*inode)->getID() > (*jnode)->getID() ? *inode : *jnode ;

			bool succeed = false;
			//forward data
			succeed = greater->receiveData(
				smaller->sendData(
				smaller->receiveRequestList(
				greater->sendRequestList( 
				greater->receiveSummaryVector( 
				smaller->sendSummaryVector() ) ) ) ) , currentTime) ;

			if( succeed )
			{
				succeed = smaller->receiveData(
					greater->sendData(
					greater->receiveRequestList(
					smaller->sendRequestList( 
					smaller->receiveSummaryVector( 
					greater->sendSummaryVector() ) ) ) ) , currentTime) ;
			
				if( succeed )
				{
					smaller->addToSpokenCache(greater, currentTime);
					greater->addToSpokenCache(smaller, currentTime);			
					cout << endl << "####  [ Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communicating ]" << endl;
				}
				else
					cout << endl << "####  [ Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communicating ]" << endl;
			}


		}

	}

	//更新所有节点的buffer状态记录
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); inode++)
	{
		(*inode)->recordBufferStatus();
	}

	cout << "####  [ Delivery Ratio ]  " << CData::getDataArrivalCount() / (double)CData::getDataCount() << endl;

}

void Epidemic::PrintInfo(int currentTime)
{
	//Energy Consumption、节点buffer状态 ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0 
		&& currentTime >= startTimeForHotspotSelection )
	{

		//平均能耗
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			energy_consumption << logInfo;
			energy_consumption << "#Time" << TAB << "#AvgEnergyConsumption" << endl;
		}
		energy_consumption << currentTime << TAB << ( CData::getAverageEnergyConsumption() * 100 ) << endl;
		energy_consumption.close();

		//每个节点buffer状态的历史平均值
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			buffer << logInfo;
			buffer << "#Time" << TAB << "#AvgBufferStateInHistoryOfEachNode" << endl;
		}
		buffer << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
			 buffer << (*inode)->getAverageBufferSize() << TAB;
		buffer << endl;
		buffer.close();

		//数据投递率-900（用于debug）
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();
		
	}

	//数据投递率、数据投递时延
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//数据投递率-100（用于绘制曲线）
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//数据投递延迟
		ofstream delay("delay.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delay << logInfo;
			delay << "#Time" << TAB << "#AvgDelay" << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//每个节点的当前buffer状态
		ofstream node("buffer-node.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			node << logInfo;
			node << "#Time" << TAB << "#BufferStateOfEachNode" << endl;
		}
		node << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
			node << (*inode)->getBufferSize() << "  " ;
		node << endl;
		node.close();

	}

	//数据投递率、数据投递时延
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//数据投递率-100（用于绘制曲线）
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delivery_ratio << logInfo;
			delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//数据投递延迟
		ofstream delay("delay.txt", ios::app);
		if(currentTime == startTimeForHotspotSelection)
		{
			delay << logInfo;
			delay << "#Time" << TAB << "#AvgDelay" << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();
	}

	////最终debug输出
	if( currentTime == RUNTIME )
	{
		debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB ;
		//debugInfo << CData::getDeliveryAtHotspotPercent() << TAB ;
		debugInfo << logInfo.replace(0, 1, "");
	}

}

bool Epidemic::Operate(int currentTime)
{
	////Node Number Test:
	//if( TEST_DYNAMIC_NUM_NODE )
	//{
	//	if(currentTime % SLOT_CHANGE_NUM_NODE == 0 && currentTime > 0)
	//	{
	//		ChangeNodeNumber();
	//	}
	//}

	if( ! CNode::hasNodes(currentTime) )
		return false;

	if( currentTime % SLOT_MOBILITYMODEL == 0 )
	{
		cout << endl << "########  [ " << currentTime << " ]  NODE LOCATION UPDATE" << endl;
		UpdateNodeStatus(currentTime);
	}

	if( currentTime % SLOT_DATA_GENERATE == 0 )
	{
		cout << endl << "########  [ " << currentTime << " ]  DATA GENERATION" << endl;
		GenerateData(currentTime);
	}

	if( currentTime % SLOT_DATA_SEND == 0 )
	{
		cout << endl << "########  [ " << currentTime << " ]  DATA DELIVERY" << endl;
		SendData(currentTime);
	}

	if( currentTime % SLOT_RECORD_INFO == 0 )
		PrintInfo(currentTime);
}