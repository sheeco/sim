#include "RoutingProtocol.h"
#include "Node.h"
#include "SMac.h"
#include "HDC.h"

int CRoutingProtocol::SLOT_DATA_SEND = SLOT_MOBILITYMODEL;  //���ݷ���slot
bool CRoutingProtocol::TEST_HOTSPOT_SIMILARITY = true;


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

	if( ! ( currentTime % SLOT_RECORD_INFO == 0
			|| currentTime == RUNTIME ) )
		return;

	//Ͷ���ʡ��ӳ١��ڵ�buffer״̬ͳ�� ...
	if( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//����Ͷ����-900������debug��
		ofstream delivery_ratio("delivery-ratio-900.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG ;
			delivery_ratio << INFO_DELIVERY_RATIO ;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay("delay.txt", ios::app);
		if(currentTime == 0)
		{
			delay << INFO_LOG ;
			delay << INFO_DELAY ;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer("buffer-node-statistics.txt", ios::app);
		if(currentTime == 0)
		{
			buffer << INFO_LOG ;
			buffer << INFO_BUFFER_STATISTICS ;
		}
		buffer << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << (*inode)->getAverageBufferSize() << TAB;
		}
		buffer << endl;
		buffer.close();

	}

	//����Ͷ���ʡ��ڵ�buffer״̬
	if(currentTime % SLOT_RECORD_INFO == 0
		|| currentTime == RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio("delivery-ratio-100.txt", ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << INFO_LOG;
			delivery_ratio << INFO_DELIVERY_RATIO ;
		}
		delivery_ratio << currentTime << TAB << CData::getDeliveryCount() << TAB << CData::getDataCount() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream buffer("buffer-node.txt", ios::app);
		if(currentTime == 0)
		{
			buffer << INFO_LOG ;
			buffer << INFO_BUFFER ;
		}
		buffer << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
		{
			if( ! (*inode)->isAlive() )
				buffer << "-" << TAB ;
			else
				buffer << (*inode)->getBufferSize() << "  " ;
		}
		buffer << endl;
		buffer.close();

	}

}

void CRoutingProtocol::PrintFinal(int currentTime)
{
	ofstream debug(FILE_DEBUG, ios::app);
	if( CNode::finiteEnergy() )
		debug << CData::getDeliveryCount() << TAB ;
	else
		debug << CData::getDeliveryRatio() << TAB ;
	debug << CData::getAverageDelay() << TAB ;
	debug.close();

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