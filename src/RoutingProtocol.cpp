#include "RoutingProtocol.h"
#include "Configuration.h"
#include "Node.h"
#include "SMac.h"
#include "HDC.h"
#include "HotspotSelect.h"
#include "PrintHelper.h"


CRoutingProtocol::CRoutingProtocol() {}

vector<CData> CRoutingProtocol::getDataForTrans(CGeneralNode* node, int capacity, bool FIFO)
{
	if( capacity <= 0
	   || capacity > getConfig<int>("trans", "window_trans") )
		capacity = getConfig<int>("trans", "window_trans");
	vector<CData> datas = node->getAllData();
	CNode::removeDataByCapacity(datas, capacity, ! FIFO);
	return datas;
}

void CRoutingProtocol::PrintInfo(int currentTime)
{
	switch( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") )
	{
		case config::_smac:
			CSMac::PrintInfo(currentTime);
			break;
		case config::_hdc:
			CHDC::PrintInfo(currentTime);
			break;
		default:
			break;
	}

	if( ! ( currentTime % getConfig<int>("log", "slot_log") == 0
			|| currentTime == getConfig<int>("simulation", "runtime") ) )
		return;

	//Ͷ���ʡ��ӳ١��ڵ�buffer״̬ͳ�� ...
	if( currentTime % getConfig<int>("hs", "slot_hotspot_update")  == 0
		|| currentTime == getConfig<int>("simulation", "runtime") )
	{
		//����Ͷ���ʣ�����debug��
		ofstream delivery_ratio( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_delivery_ratio_brief"), ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << getConfig<string>("log", "info_log") << endl ;
			delivery_ratio << getConfig<string>("log", "info_delivery_ratio_brief") << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_delay"), ios::app);
		if(currentTime == 0)
		{
			delay << endl << getConfig<string>("log", "info_log") << endl ;
			delay << getConfig<string>("log", "info_delay") << endl;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//����Ͷ������
		ofstream hop( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hop"), ios::app);
		if(currentTime == 0)
		{
			hop << endl << getConfig<string>("log", "info_log") << endl ;
			hop << getConfig<string>("log", "info_hop") << endl;
		}
		hop << currentTime << TAB << CData::getAverageHOP() << endl;
		hop.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer_statistics"), ios::app);
		if(currentTime == 0)
		{
			buffer << endl << getConfig<string>("log", "info_log") << endl ;
			buffer << getConfig<string>("log", "info_buffer_statistics") << endl;
		}
		buffer << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << NDigitFloat( (*inode)->getAverageSizeBuffer(), 1) << TAB;
		}
		buffer << endl;
		buffer.close();

	}

	//����Ͷ���ʡ��ڵ�buffer״̬
	if(currentTime % getConfig<int>("log", "slot_log") == 0
		|| currentTime == getConfig<int>("simulation", "runtime"))
	{
		//����Ͷ���ʣ����ڻ������ߣ�
		ofstream delivery_ratio( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_delivery_ratio_detail"), ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << getConfig<string>("log", "info_log") << endl ;
			delivery_ratio << getConfig<string>("log", "info_delivery_ratio_detail") << endl;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream buffer( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer"), ios::app);
		if(currentTime == 0)
		{
			buffer << endl << getConfig<string>("log", "info_log") << endl ;
			buffer << getConfig<string>("log", "info_buffer") << endl;
		}
		buffer << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << (*inode)->getSizeBuffer() << "  " ;

			(*inode)->recordBufferStatus();
		}
		buffer << endl;
		buffer.close();

	}

}

void CRoutingProtocol::PrintFinal(int currentTime)
{
	ofstream final( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_final"), ios::app);
	if( CNode::finiteEnergy() )
		final << CData::getCountDelivery() << TAB ;
	else
		final << CData::getDeliveryRatio() << TAB ;
	final << CData::getAverageDelay() << TAB << CData::getAverageHOP() << TAB ;
	final.close();

	switch( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") )
	{
		case config::_smac:
			CSMac::PrintFinal(currentTime);
			break;
		case config::_hdc:
			CHDC::PrintFinal(currentTime);
			break;
		default:
			break;
	}

	CPrintHelper::PrintNewLine();
	CPrintHelper::PrintHeading(currentTime - 1, "SIMULATION FINISHED");
	CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
}