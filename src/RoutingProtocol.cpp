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
	   || capacity > configs.trans.WINDOW_TRANS )
		capacity = configs.trans.WINDOW_TRANS;
	vector<CData> datas = node->getAllData();
	CNode::removeDataByCapacity(datas, capacity, ! FIFO);
	return datas;
}

void CRoutingProtocol::PrintInfo(int currentTime)
{
	switch( configs.MAC_PROTOCOL )
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

	if( ! ( currentTime % configs.log.SLOT_LOG == 0
			|| currentTime == configs.simulation.RUNTIME ) )
		return;

	//Ͷ���ʡ��ӳ١��ڵ�buffer״̬ͳ�� ...
	if( currentTime % configs.hs.SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == configs.simulation.RUNTIME )
	{
		//����Ͷ����-900������debug��
		ofstream delivery_ratio( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_DELIVERY_RATIO_900, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << configs.log.INFO_LOG << endl ;
			delivery_ratio << configs.log.INFO_DELIVERY_RATIO_900 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//����Ͷ���ӳ�
		ofstream delay( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_DELAY, ios::app);
		if(currentTime == 0)
		{
			delay << endl << configs.log.INFO_LOG << endl ;
			delay << configs.log.INFO_DELAY ;
		}
		delay << currentTime << TAB << CData::getAverageDelay() << endl;
		delay.close();

		//����Ͷ������
		ofstream hop( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_HOP, ios::app);
		if(currentTime == 0)
		{
			hop << endl << configs.log.INFO_LOG << endl ;
			hop << configs.log.INFO_HOP ;
		}
		hop << currentTime << TAB << CData::getAverageHOP() << endl;
		hop.close();

		//ÿ���ڵ�buffer״̬����ʷƽ��ֵ
		ofstream buffer( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_BUFFER_STATISTICS, ios::app);
		if(currentTime == 0)
		{
			buffer << endl << configs.log.INFO_LOG << endl ;
			buffer << configs.log.INFO_BUFFER_STATISTICS ;
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
	if(currentTime % configs.log.SLOT_LOG == 0
		|| currentTime == configs.simulation.RUNTIME)
	{
		//����Ͷ����-100�����ڻ������ߣ�
		ofstream delivery_ratio( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_DELIVERY_RATIO_100, ios::app);
		if(currentTime == 0)
		{
			delivery_ratio << endl << configs.log.INFO_LOG << endl ;
			delivery_ratio << configs.log.INFO_DELIVERY_RATIO_100 ;
		}
		delivery_ratio << currentTime << TAB << CData::getCountDelivery() << TAB << CData::getCountData() << TAB << CData::getDeliveryRatio() << endl;
		delivery_ratio.close();

		//ÿ���ڵ�ĵ�ǰbuffer״̬
		ofstream buffer( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_BUFFER, ios::app);
		if(currentTime == 0)
		{
			buffer << endl << configs.log.INFO_LOG << endl ;
			buffer << configs.log.INFO_BUFFER ;
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
	ofstream final( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_FINAL, ios::app);
	if( CNode::finiteEnergy() )
		final << CData::getCountDelivery() << TAB ;
	else
		final << CData::getDeliveryRatio() << TAB ;
	final << CData::getAverageDelay() << TAB << CData::getAverageHOP() << TAB ;
	final.close();

	switch( configs.MAC_PROTOCOL )
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