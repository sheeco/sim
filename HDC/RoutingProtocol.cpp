#include "RoutingProtocol.h"
#include "Node.h"

extern int RUNTIME;
extern int DATATIME;
extern string INFO_LOG;
extern ofstream debugInfo;

//CRoutingProtocol::CRoutingProtocol(void)
//{
//}
//
//
//CRoutingProtocol::~CRoutingProtocol(void)
//{
//}

void CRoutingProtocol::UpdateNodeStatus(int currentTime)
{
	if( ! currentTime % SLOT_MOBILITYMODEL == 0 )
		return;

	cout << endl << "########  < " << currentTime << " >  NODE LOCATION UPDATE" ;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
		(*inode)->updateStatus(currentTime);
}

void CRoutingProtocol::GenerateData(int currentTime)
{
	if( ! ( currentTime % SLOT_DATA_GENERATE == 0 && currentTime <= DATATIME ) )
		return;

	cout << endl << "########  < " << currentTime << " >  DATA GENERATION" ;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
		(*inode)->generateData(currentTime);
}

void CRoutingProtocol::PrintInfo(int currentTime)
{
	if( ! currentTime % SLOT_RECORD_INFO == 0 )
		return;

	//Energy Consumption���ڵ�buffer״̬ ...
	if( currentTime % SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{

		//ƽ���ܺ�
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == 0)
		{
			energy_consumption << INFO_LOG;
			energy_consumption << "#Time" << TAB << "#AvgEnergyConsumption" << endl;
		}
		energy_consumption << currentTime << TAB << CData::getAverageEnergyConsumption() / 1000 << endl;
		energy_consumption.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == 0)
		{
			buffer << INFO_LOG;
			buffer << "#Time" << TAB << "#AvgBufferStateInHistoryOfEachNode" << endl;
		}
		buffer << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
			 buffer << (*inode)->getAverageBufferSize() << TAB;
		buffer << endl;
		buffer.close();

		//����Ͷ����-900������debug��
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG;
			delivery_ratio << "#Time" << TAB << "#ArrivalCount" << TAB << "#TotalCount" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDataArrivalCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay("delay.txt", ios::app);
		if(currentTime == 0)
		{
			delay << INFO_LOG;
			delay << "#Time" << TAB << "#AvgDelay" << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

	}

	//����Ͷ���ʡ�����Ͷ��ʱ��
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG;
			delivery_ratio << "#Time" << TAB << "#DeliveryRatio" << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream node("buffer-node.txt", ios::app);
		if(currentTime == 0)
		{
			node << INFO_LOG;
			node << "#Time" << TAB << "#BufferStateOfEachNode" << endl;
		}
		node << currentTime << TAB;
		for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
			node << (*inode)->getBufferSize() << "  " ;
		node << endl;
		node.close();

	}

	////����debug���
	if( currentTime == RUNTIME )
	{
		debugInfo << CData::getDeliveryRatio() << TAB << CData::getAverageDelay() << TAB << CData::getAverageEnergyConsumption() / 1000 << TAB ;
		//debugInfo << CData::getDeliveryAtHotspotPercent() << TAB ;
		debugInfo << INFO_LOG.replace(0, 1, "");
		debugInfo.flush();
	}

}
