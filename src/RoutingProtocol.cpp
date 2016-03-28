#include "RoutingProtocol.h"
#include "Node.h"
#include "SMac.h"
#include "HDC.h"

int CRoutingProtocol::SLOT_DATA_SEND = SLOT_MOBILITYMODEL;  //���ݷ���slot


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

	//Ͷ���ʡ��ӳ١��ڵ�buffer״̬ͳ�� ...
	if( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//����Ͷ����-900������debug��
		ofstream delivery_ratio( PATH_ROOT + PATH_LOG + FILE_DELIVERY_RATIO_900, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << INFO_LOG << endl ;
			delivery_ratio << INFO_DELIVERY_RATIO_900 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay( PATH_ROOT + PATH_LOG + FILE_DELAY, ios::app);
		if(currentTime == 0)
		{
			delay << endl << INFO_LOG << endl ;
			delay << INFO_DELAY ;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//����Ͷ������
		ofstream hop( PATH_ROOT + PATH_LOG + FILE_HOP, ios::app);
		if(currentTime == 0)
		{
			hop << endl << INFO_LOG << endl ;
			hop << INFO_HOP ;
		}
		hop << currentTime << TAB << CData::getAverageHOP() << endl;
		hop.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
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

	//����Ͷ���ʡ��ڵ�buffer״̬
	if(currentTime % SLOT_LOG == 0
		|| currentTime == RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio( PATH_ROOT + PATH_LOG + FILE_DELIVERY_RATIO_100, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << INFO_LOG << endl ;
			delivery_ratio << INFO_DELIVERY_RATIO_100 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
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