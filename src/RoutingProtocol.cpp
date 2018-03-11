#include "RoutingProtocol.h"
#include "Configuration.h"
#include "Node.h"
#include "SMac.h"
#include "HDC.h"
#include "HotspotSelect.h"
#include "PrintHelper.h"


CRoutingProtocol::CRoutingProtocol() {}

void CRoutingProtocol::PrintInfo(int now)
{
	PrintInfo(CNode::getAllNodes(), now);
}

void CRoutingProtocol::PrintInfo(vector<CNode*> allNodes, int now)
{
	switch( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") )
	{
		case config::_smac:
			CSMac::PrintInfo(now);
			break;
		case config::_hdc:
			CHDC::PrintInfo(now);
			break;
		default:
			break;
	}

	if( ! ( now % getConfig<int>("log", "slot_log") == 0
			|| now == getConfig<int>("simulation", "runtime") ) )
		return;

	//Ͷ���ʡ��ӳ١��ڵ�buffer״̬ͳ�� ...
	if( now % getConfig<int>("hs", "slot_hotspot_update")  == 0
		|| now == getConfig<int>("simulation", "runtime") )
	{
		//����Ͷ���ʣ�����debug��
		ofstream delivery_ratio( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_delivery_ratio_brief"), ios::app);
		if(now == 0)
		{
			delivery_ratio << endl << getConfig<string>("log", "info_log") << endl ;
			delivery_ratio << getConfig<string>("log", "info_delivery_ratio_brief") << endl;
		}
		delivery_ratio << now << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_delay"), ios::app);
		if(now == 0)
		{
			delay << endl << getConfig<string>("log", "info_log") << endl ;
			delay << getConfig<string>("log", "info_delay") << endl;
		}
		delay << now << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//����Ͷ������
		ofstream hop( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_hop"), ios::app);
		if(now == 0)
		{
			hop << endl << getConfig<string>("log", "info_log") << endl ;
			hop << getConfig<string>("log", "info_hop") << endl;
		}
		hop << now << TAB << CData::getAverageHOP() << endl;
		hop.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer_statistics"), ios::app);
		if(now == 0)
		{
			buffer << endl << getConfig<string>("log", "info_log") << endl ;
			buffer << getConfig<string>("log", "info_buffer_statistics") << endl;
		}
		buffer << now << TAB;
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
	if(now % getConfig<int>("log", "slot_log") == 0
		|| now == getConfig<int>("simulation", "runtime"))
	{
		//����Ͷ���ʣ����ڻ������ߣ�
		ofstream delivery_ratio( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_delivery_ratio_detail"), ios::app);
		if(now == 0)
		{
			delivery_ratio << endl << getConfig<string>("log", "info_log") << endl ;
			delivery_ratio << getConfig<string>("log", "info_delivery_ratio_detail") << endl;
		}
		delivery_ratio << now << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream buffer( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_buffer"), ios::app);
		if(now == 0)
		{
			buffer << endl << getConfig<string>("log", "info_log") << endl ;
			buffer << getConfig<string>("log", "info_buffer") << endl;
		}
		buffer << now << TAB;
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << (*inode)->getBufferSize() << "  " ;

			(*inode)->recordBufferStatus();
		}
		buffer << endl;
		buffer.close();

	}

}

void CRoutingProtocol::PrintFinal(int now)
{
	
	switch( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") )
	{
		case config::_smac:
			CSMac::PrintFinal(now);
			break;
		case config::_hdc:
			CHDC::PrintFinal(now);
			break;
		default:
			break;
	}

	CPrintHelper::PrintNewLine();
	CPrintHelper::PrintHeading(now - 1, "SIMULATION FINISHED");
	CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
}