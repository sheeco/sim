#include "Global.h"
#include "ProphetNode.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "Prophet.h"
#include "SortHelper.h"

bool CProphet::TRANS_STRICT_BY_PRED = true;

double CProphet::INIT_PRED = 0.75;  //�ο�ֵ 0.75
double CProphet::RATIO_PRED_DECAY = 0.98;  //�ο�ֵ 0.98(/s)
double CProphet::RATIO_PRED_TRANS = 0.25;  //�ο�ֵ 0.25


CProphet::CProphet()
{
	
}

CProphet::~CProphet()
{
	
}

vector<CPacket*> CProphet::receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int currentTime)
{
	vector<CPacket*> packetsToSend;

	if( typeid( gToNode ) == typeid( CSink ) )
	{
		CSink & toSink = dynamic_cast< CSink & >( gToNode );

		/*********************************************** Sink <- Node *******************************************************/

		if( typeid( gFromNode ) == typeid( CProphetNode ) )
		{
			CProphetNode & fromNode = dynamic_cast<CProphetNode &>( gFromNode );
			packetsToSend = CProphet::receivePackets(toSink, fromNode, packets, currentTime);
		}
	}

	else if( typeid( gToNode ) == typeid( CProphetNode ) )
	{
		CProphetNode & node = dynamic_cast<CProphetNode &>( gToNode );

		/*********************************************** Node <- Sink *******************************************************/

		if( typeid( gFromNode ) == typeid( CSink ) )
		{
			CSink & fromSink = dynamic_cast< CSink & >( gFromNode );
			packetsToSend = CProphet::receivePackets(node, fromSink, packets, currentTime);
		}

		/*********************************************** Node <- Node *******************************************************/

		else if( typeid( gFromNode ) == typeid( CProphetNode ) )
		{
			CProphetNode & fromNode = dynamic_cast<CProphetNode &>( gFromNode );
			packetsToSend = CProphet::receivePackets(node, fromNode, packets, currentTime);
		}
	}

	return packetsToSend;
}

vector<CPacket*> CProphet::receivePackets(CProphetNode & node, CSink* sink, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA��������Ϊ�գ�û���ʺϴ��������
	vector<CData> dataToSend;  //��vector����ܾ���������

	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid(**ipacket) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*ipacket);
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

			case CCtrl::_rts:

				node.updateDeliveryPredsWithSink(sink);

				if( node.getAllData().empty() )
				{
					return packetsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(node.getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_cts);
				// + DATA
				dataToSend = node.getDataForTrans();

				node.delayDiscovering(CRoutingProtocol::getTimeWindowTrans());
				node.delaySleep(CRoutingProtocol::getTimeWindowTrans());

				// TODO: mark skipRTS ?
				// TODO: connection established ?
				break;

			case CCtrl::_cts:

				break;

			case CCtrl::_capacity:

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_no_data:

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				node.startDiscovering();

				//�յ��յ�ACKʱ�������������ݴ���
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					node.checkDataByAck( ctrl->getACK() );

				flash_cout << "######  < " << time << " >  ( Node " << NDigitString(node.getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Sink )       " ;

				return packetsToSend;

				break;

			default:
				break;
			}
			++ipacket;
		}
		else
		{
			++ipacket;
		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
	{
		packetsToSend.push_back(ctrlToSend);
	}
	if( indexToSend != nullptr )
	{
		packetsToSend.push_back(indexToSend);
	}
	if( nodataToSend != nullptr )
	{
		packetsToSend.push_back(nodataToSend);
	}
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			packetsToSend.push_back(new CData(*idata));
	}

	return packetsToSend;
	
}

vector<CPacket*> CProphet::receivePackets(CSink* sink, CProphetNode & fromNode, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;

	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{
		if( typeid(**ipacket) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*ipacket);
			switch( ctrl->getType() )
			{
			case CCtrl::_rts:

				break;

			case CCtrl::_cts:

				break;

			case CCtrl::_capacity:

				break;

			case CCtrl::_index:

				break;

			case CCtrl::_ack:

				break;

			case CCtrl::_no_data:

				break;

			default:

				break;
			}
			++ipacket;
		}

		else if( typeid(**ipacket) == typeid(CData) )
		{
			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back( *dynamic_cast<CData*>(*ipacket) );
				++ipacket;
			} while( ipacket != packets.end() );

			//accept data into buffer
			vector<CData> ack = CSink::bufferData(time, datas);

			//ACK������յ�������ȫ�������������Ϳյ�ACK��
			ctrlToSend = new CCtrl(CSink::SINK_ID, ack, time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
		packetsToSend.push_back(ctrlToSend);

	return packetsToSend;

}

vector<CPacket*> CProphet::receivePackets(CProphetNode & node, CProphetNode & fromNode, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* capacityToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA��������Ϊ�գ�û���ʺϴ��������
	vector<CData> dataToSend;  //��vector����ܾ���������
	int capacity = -1;

//	int debugNodeID = 0;
//	int debugFromID = 0;

	for(vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid(**ipacket) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*ipacket);
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

			case CCtrl::_rts:

				//skip if has spoken recently
				if( node.hasSpokenRecently(dynamic_cast<CProphetNode &>(fromNode), time) )
				{
					flash_cout << "######  ( Node " << NDigitString(node.getID(), 2) << "  ----- skip -----  Node " << NDigitString(fromNode.getID(), 2) << " )                " ;
					return packetsToSend;
				}
				//rcv RTS from node
				else
				{
					//update preds
					node.updateDeliveryPredsWith(fromNode.getID(), ctrl->getPred());

					//CTS
					ctrlToSend = new CCtrl(node.getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_cts);

					// + Capacity
					if( CProphetNode::MODE_RECEIVE == CProphetNode::_RECEIVE::_selfish 
						&& node.hasData() )
						capacityToSend = new CCtrl(node.getID(), node.getDeliveryPreds(), time, CGeneralNode::SIZE_CTRL, CCtrl::_index);

					// + Index
					indexToSend = new CCtrl(node.getID(), node.getDeliveryPreds(), time, CGeneralNode::SIZE_CTRL, CCtrl::_index);
					//node.skipRTS();  //����δʹ�ã�
				}

				// TODO: mark skipRTS ?
				// TODO: connection established ?
				break;

				/*************************************** rcv CTS **************************************/

			case CCtrl::_cts:

				// TODO: connection established ?
				break;

				/************************************* rcv capacity ***********************************/

			case CCtrl::_capacity:

				capacity = ctrl->getCapacity();

				break;

				/****************************** rcv Data Index ( dp / sv ) ****************************/

			case CCtrl::_index:

				// + DATA / NODATA
				//·��Э��������ýڵ�ת������
//				debugNodeID = node.getID();
//				debugFromID = fromNode->getID();
				if( node.shouldForward(ctrl->getPred() ) )
				{
					if( capacity == 0 )
					{
						return packetsToSend;
					}

					dataToSend = node.getDataForTrans();

					//������Ϊ��
					if( dataToSend.empty() )
						nodataToSend = new CCtrl(node.getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_no_data);
					else if( capacity > 0 )
					{
						CProphetNode::removeDataByCapacity(dataToSend, capacity, false);
					}
					
					//���·��Э��������ýڵ㷢�����ݣ��Է��ڵ�Ͳ����������ݣ�������跢��capacity
					if( capacityToSend != nullptr )
					{
						free(capacityToSend);
						capacityToSend = nullptr;
					}
				}
				//���򣬲�����ת������ʱ��dataToSend����

				//ע�⣺�����ȡ����Prophet��Ҫ�������ڵ㶼�ܾ��������ݣ��˴������¿յ���Ӧ��ֱ�ӽ����������ݴ���
				//     ���򲢲��������䣬����Ϊ�Է����������ݷ���ACK��

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//��������ھ��б�
				node.addToSpokenCache( fromNode, time );

				node.startDiscovering();

				//�յ��յ�ACKʱ�������������ݴ���
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					node.checkDataByAck( ctrl->getACK() );

				flash_cout << "######  < " << time << " >  ( Node " << NDigitString(node.getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Node " << NDigitString(fromNode.getID(), 2) << " )                       " ;

				return packetsToSend;

				break;

			case CCtrl::_no_data:

				//�յ�NODATA��Ҳ���ظ�һ���յ�ACK������Ҳ������Ϊ���ݴ���ɹ�
				//�յ�ACK
				ctrlToSend = new CCtrl(node.getID(), vector<CData>(), time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
				//��������ھ��б�
				node.addToSpokenCache( fromNode, time );
				node.startDiscovering();


				//ά�����ݴ���ĵ����ԣ�����յ����ݻ�NODATA���Ͳ��������ݻ�NODATA��ע�⣺ǰ���ǿ��ư����������ݰ�֮ǰ��
				dataToSend.clear();
				if( nodataToSend != nullptr )
				{
					free(nodataToSend);
					nodataToSend = nullptr;
				}

				break;

			default:
				break;
			}
			++ipacket;
		}

		/******************************************* rcv Data *******************************************/

		else if( typeid(**ipacket) == typeid(CData) )
		{
			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back( *dynamic_cast<CData*>(*ipacket) );
				++ipacket;
			} while( ipacket != packets.end() );

			//ά�����ݴ���ĵ����ԣ�����յ����ݻ�NODATA���Ͳ��������ݻ�NODATA��ע�⣺ǰ���ǿ��ư����������ݰ�֮ǰ��
			dataToSend.clear();
			if( nodataToSend != nullptr )
			{
				free(nodataToSend);
				nodataToSend = nullptr;
			}

			//accept data into buffer
			vector<CData> ack;
			ack = node.bufferData(datas, time);
			//ACK������յ�������ȫ�������������Ϳյ�ACK��
			ctrlToSend = new CCtrl(node.getID(), ack, time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
			//��������ھ��б�
			node.addToSpokenCache( fromNode, time );
			node.startDiscovering();

		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
	{
		packetsToSend.push_back(ctrlToSend);
	}
	if( capacityToSend != nullptr )
	{
		packetsToSend.push_back(capacityToSend);
	}
	if( indexToSend != nullptr )
	{
		packetsToSend.push_back(indexToSend);
	}
	if( nodataToSend != nullptr )
	{
		packetsToSend.push_back(nodataToSend);
	}
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			packetsToSend.push_back(new CData(*idata));
	}

	return packetsToSend;

}

bool CProphet::Operate(int currentTime)
{
	bool hasNodes = true;
	if( MAC_PROTOCOL == _hdc )
		hasNodes = CHDC::Prepare(currentTime);
	else if( MAC_PROTOCOL == _smac )
		hasNodes = CSMac::Prepare(currentTime);

	if( ! hasNodes )
		return false;

	if( MAC_PROTOCOL == _hdc )
		CHDC::Operate(currentTime);
	else if( MAC_PROTOCOL == _smac )
		CSMac::Operate(currentTime);

	PrintInfo(currentTime);

	return true;
}
