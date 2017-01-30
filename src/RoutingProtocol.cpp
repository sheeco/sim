#include "RoutingProtocol.h"
#include "Node.h"
#include "SMac.h"
#include "HDC.h"
#include "HotspotSelect.h"
#include "Trace.h"
#include "PrintHelper.h"

//int CRoutingProtocol::SLOT_DATA_SEND = CCTrace::SLOT_TRACE;  //���ݷ���slot
int CRoutingProtocol::WINDOW_TRANS = 0;
int CRoutingProtocol::TIME_WINDOW_TRANS = 0;


CRoutingProtocol::CRoutingProtocol() {}

int CRoutingProtocol::getTimeWindowTrans()
{
	return 0;

	//if( TIME_WINDOW_TRANS == 0 )
	//	TIME_WINDOW_TRANS = int( ceil( double( WINDOW_TRANS * CNode::SIZE_DATA + CMacProtocol::SIZE_HEADER_MAC ) / double(CNode::SPEED_TRANS) ) );
	//return TIME_WINDOW_TRANS;
}

vector<CData> CRoutingProtocol::getDataForTrans(CGeneralNode* node, int capacity, bool FIFO)
{
	if( capacity <= 0
	   || capacity > WINDOW_TRANS )
		capacity = WINDOW_TRANS;
	vector<CData> datas = node->getAllData();
	CNode::removeDataByCapacity(datas, capacity, ! FIFO);
	return datas;
}

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

	//Ͷ���ʡ��ӳ١��ڵ�buffer״̬ͳ�� ...
	if( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//����Ͷ����-900������debug��
		ofstream delivery_ratio( DIR_LOG + PATH_TIMESTAMP + FILE_DELIVERY_RATIO_900, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << INFO_LOG << endl ;
			delivery_ratio << INFO_DELIVERY_RATIO_900 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay( DIR_LOG + PATH_TIMESTAMP + FILE_DELAY, ios::app);
		if(currentTime == 0)
		{
			delay << endl << INFO_LOG << endl ;
			delay << INFO_DELAY ;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//����Ͷ������
		ofstream hop( DIR_LOG + PATH_TIMESTAMP + FILE_HOP, ios::app);
		if(currentTime == 0)
		{
			hop << endl << INFO_LOG << endl ;
			hop << INFO_HOP ;
		}
		hop << currentTime << TAB << CData::getAverageHOP() << endl;
		hop.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer( DIR_LOG + PATH_TIMESTAMP + FILE_BUFFER_STATISTICS, ios::app);
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
				buffer << NDigitFloat( (*inode)->getAverageSizeBuffer(), 1) << TAB;
		}
		buffer << endl;
		buffer.close();

	}

	//����Ͷ���ʡ��ڵ�buffer״̬
	if(currentTime % SLOT_LOG == 0
		|| currentTime == RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio( DIR_LOG + PATH_TIMESTAMP + FILE_DELIVERY_RATIO_100, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << INFO_LOG << endl ;
			delivery_ratio << INFO_DELIVERY_RATIO_100 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream buffer( DIR_LOG + PATH_TIMESTAMP + FILE_BUFFER, ios::app);
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
	ofstream final( DIR_LOG + PATH_TIMESTAMP + FILE_FINAL, ios::app);
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

	CPrintHelper::PrintNewLine();
	CPrintHelper::PrintHeading(currentTime - 1, "SIMULATION FINISHED");
	CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
}