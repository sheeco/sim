#include "RoutingProtocol.h"
#include "Node.h"
#include "SMac.h"
#include "HDC.h"
#include "HotspotSelect.h"
#include "CTrace.h"

int CRoutingProtocol::SLOT_DATA_SEND = CCTrace::SLOT_TRACE;  //数据发送slot


//CRoutingProtocol::CRoutingProtocol()
//{
//}
//
//CRoutingProtocol::~CRoutingProtocol()
//{
//}

CRoutingProtocol::CRoutingProtocol() {}

CRoutingProtocol::~CRoutingProtocol() {}

void CRoutingProtocol::PrintInfo(int currentTime)
{
	switch( MAC_PROTOCOL )
	{
		case _smac:
			CSMac::PrintInfo(currentTime);
			break;
		case _hdc:
			CHDC::PrintInfo(currentTime);
			break;
		default:
			break;
	}

	if( ! ( currentTime % SLOT_LOG == 0
			|| currentTime == RUNTIME ) )
		return;

	//投递率、延迟、节点buffer状态统计 ...
	if( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//数据投递率-900（用于debug）
		ofstream delivery_ratio( PATH_ROOT + PATH_LOG + FILE_DELIVERY_RATIO_900, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << INFO_LOG << endl ;
			delivery_ratio << INFO_DELIVERY_RATIO_900 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//数据投递延迟
		ofstream delay( PATH_ROOT + PATH_LOG + FILE_DELAY, ios::app);
		if(currentTime == 0)
		{
			delay << endl << INFO_LOG << endl ;
			delay << INFO_DELAY ;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//数据投递跳数
		ofstream hop( PATH_ROOT + PATH_LOG + FILE_HOP, ios::app);
		if(currentTime == 0)
		{
			hop << endl << INFO_LOG << endl ;
			hop << INFO_HOP ;
		}
		hop << currentTime << TAB << CData::getAverageHOP() << endl;
		hop.close();

		//每个节点buffer状态的历史平均值
		ofstream buffer( PATH_ROOT + PATH_LOG + FILE_BUFFER_STATISTICS, ios::app);
		if(currentTime == 0)
		{
			buffer << endl << INFO_LOG << endl ;
			buffer << INFO_BUFFER_STATISTICS ;
		}
		buffer << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << (*inode)->getAverageSizeBuffer() << TAB;
		}
		buffer << endl;
		buffer.close();

	}

	//数据投递率、节点buffer状态
	if(currentTime % SLOT_LOG == 0
		|| currentTime == RUNTIME)
	{
		//数据投递率-100（用于绘制曲线）
		ofstream delivery_ratio( PATH_ROOT + PATH_LOG + FILE_DELIVERY_RATIO_100, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << INFO_LOG << endl ;
			delivery_ratio << INFO_DELIVERY_RATIO_100 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//每个节点的当前buffer状态
		ofstream buffer( PATH_ROOT + PATH_LOG + FILE_BUFFER, ios::app);
		if(currentTime == 0)
		{
			buffer << endl << INFO_LOG << endl ;
			buffer << INFO_BUFFER ;
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
	ofstream final( PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app);
	if( CNode::finiteEnergy() )
		final << CData::getCountDelivery() << TAB ;
	else
		final << CData::getDeliveryRatio() << TAB ;
	final << CData::getAverageDelay() << TAB << CData::getAverageHOP() << TAB ;
	final.close();

	switch( MAC_PROTOCOL )
	{
		case _smac:
			CSMac::PrintFinal(currentTime);
			break;
		case _hdc:
			CHDC::PrintFinal(currentTime);
			break;
		default:
			break;
	}

}