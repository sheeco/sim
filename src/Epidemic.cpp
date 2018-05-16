#include "Epidemic.h"
#include "Global.h"
#include "Configuration.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "PrintHelper.h"



//	//在特定时槽上发送数据
//	//注意：必须在调用UpdateNodeStatus之后调用此函数
//	static void SendData(int now);

CEpidemic::CEpidemic()
{
}

CEpidemic::~CEpidemic()
{
}

vector<CPacket*> CEpidemic::receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;

	if( typeid( gToNode ) == typeid( CSink ) )
	{
		CSink* toSink = dynamic_cast< CSink* >( &gToNode );

		/*********************************************** Sink <- Node *******************************************************/

		if( typeid( gFromNode ) == typeid( CNode ) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( &gFromNode );
			packetsToSend = CEpidemic::receivePackets(toSink, fromNode, packets, now);
		}
	}

	else if( typeid( gToNode ) == typeid( CNode ) )
	{
		CNode* node = dynamic_cast<CNode*>( &gToNode );

		/*********************************************** Node <- Sink *******************************************************/

		if( typeid( gFromNode ) == typeid( CSink ) )
		{
			CSink* fromSink = dynamic_cast< CSink* >( &gFromNode );
			packetsToSend = CEpidemic::receivePackets(node, fromSink, packets, now);
		}

		/*********************************************** Node <- Node *******************************************************/

		else if( typeid( gFromNode ) == typeid( CNode ) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( &gFromNode );
			packetsToSend = CEpidemic::receivePackets(node, fromNode, packets, now);
		}
	}

	return packetsToSend;
}

vector<CPacket*> CEpidemic::receivePackets(CNode* node, CSink* sink, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
	vector<CData> dataToSend;  //空vector代表拒绝传输数据
	bool waitForResponse = false;

	for( vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid( **ipacket ) == typeid( CCtrl ) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>( *ipacket );
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

				case CCtrl::_rts:

					if( !node->hasData() )
					{
						return packetsToSend;
					}
					//CTS
					ctrlToSend = new CCtrl(node->getID(), now, CCtrl::_cts);
					// + DATA
					dataToSend = node->getDataForTrans(INVALID);

					waitForResponse = true;
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

					node->startDiscovering();

					//收到空的ACK时，结束本次数据传输
					if( ctrl->getACK().empty() )
						return packetsToSend;
					//clear data with ack
					else
						node->dropDataByAck(ctrl->getACK());

					CPrintHelper::PrintCommunication(now, node->getName(), sink->getName(), ctrl->getACK().size());

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
	if( !dataToSend.empty() )
	{
		for( auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata )
			packetsToSend.push_back(new CData(*idata));
	}
	if( waitForResponse )
	{
		int timeDelay = CMacProtocol::getTransmissionDelay(packetsToSend);
		node->delaySleep(timeDelay);
	}
	return packetsToSend;

}

vector<CPacket*> CEpidemic::receivePackets(CSink* sink, CNode* fromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* ackToSend = nullptr;

	for( vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{
		if( typeid( **ipacket ) == typeid( CCtrl ) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>( *ipacket );
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

		else if( typeid( **ipacket ) == typeid( CData ) )
		{
			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back(*dynamic_cast<CData*>( *ipacket ));
				++ipacket;
			} while( ipacket != packets.end() );

			//accept data into buffer
			vector<CData> ack = CSink::bufferData(now, datas);

			//ACK（如果收到的数据全部被丢弃，发送空的ACK）
			ackToSend = new CCtrl(CSink::getSink()->getID(), ack, now, CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
		packetsToSend.push_back(ctrlToSend);
	if( ackToSend != nullptr )
		packetsToSend.push_back(ackToSend);

	return packetsToSend;

}

//TODO: test
vector<CPacket*> CEpidemic::receivePackets(CNode* node, CNode* fromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* capacityToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
	CCtrl* ackToSend = nullptr;
	vector<CData> dataToSend;  //空vector代表拒绝传输数据
	int capacity = INVALID;
	bool waitForResponse = false;

	for( vector<CPacket*>::iterator ipacket = packets.begin(); ipacket != packets.end(); )
	{

		/***************************************** rcv Ctrl Message *****************************************/

		if( typeid( **ipacket ) == typeid( CCtrl ) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>( *ipacket );
			switch( ctrl->getType() )
			{

				/*************************************** rcv RTS **************************************/

				case CCtrl::_rts:

					//skip if has spoken recently
					if( node->hasCommunicatedRecently(fromNode->getID(), now) )
					{
						CPrintHelper::PrintCommunicationSkipped(now, node->getName(), fromNode->getName());
						return packetsToSend;
					}
					//rcv RTS from node
					else
					{
						//CTS
						ctrlToSend = new CCtrl(node->getID(), now, CCtrl::_cts);
						// + Capacity
						capacityToSend = new CCtrl(node->getID(), node->getBufferVacancy(), now, CCtrl::_capacity);
						// + Index
						indexToSend = new CCtrl(node->getID(), now, CCtrl::_index);

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

					dataToSend = node->getDataForTrans(fromNode->getBufferHistory(), capacity);

					if( dataToSend.empty() )
						nodataToSend = new CCtrl(node->getID(), now, CCtrl::_no_data);
					else
						waitForResponse = true;

					break;

					/*************************************** rcv ACK **************************************/

				case CCtrl::_ack:

					//加入最近邻居列表
					node->updateCommunicationHistory(fromNode->getID(), now);

					node->startDiscovering();

					//收到空的ACK时，结束本次数据传输
					if( !ctrl->getACK().empty() )
					{
						CPrintHelper::PrintCommunication(now, node->getName(), fromNode->getName(), ctrl->getACK().size());
					}

					break;

				case CCtrl::_no_data:

					//CTS
					if( ctrlToSend == nullptr )
						ctrlToSend = new CCtrl(node->getID(), now, CCtrl::_cts);

					//收到NODATA，也将回复一个空的ACK，即，也将被认为数据传输成功
					//空的ACK
					ackToSend = new CCtrl(node->getID(), vector<CData>(), now, CCtrl::_ack);
					//加入最近邻居列表
					node->updateCommunicationHistory(fromNode->getID(), now);
					node->startDiscovering();

					break;

				default:
					break;
			}
			++ipacket;
		}

		/******************************************* rcv Data *******************************************/

		else if( typeid( **ipacket ) == typeid( CData ) )
		{
			//CTS
			if( ctrlToSend == nullptr )
				ctrlToSend = new CCtrl(node->getID(), now, CCtrl::_cts);

			//extract data packet
			vector<CData> datas;
			do
			{
				datas.push_back(*dynamic_cast<CData*>( *ipacket ));
				++ipacket;
			} while( ipacket != packets.end() );

			//accept data into buffer
			vector<CData> ack;
			//存入收到的数据并发送ACK
			ack = node->bufferData(now, datas);
			ackToSend = new CCtrl(node->getID(), ack, now, CCtrl::_ack);
			//加入最近邻居列表
			node->updateCommunicationHistory(fromNode->getID(), now);
			node->startDiscovering();

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
	if( ackToSend != nullptr )
	{
		packetsToSend.push_back(ackToSend);
	}
	if( nodataToSend != nullptr )
	{
		packetsToSend.push_back(nodataToSend);
	}
	if( !dataToSend.empty() )
	{
		for( auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata )
			packetsToSend.push_back(new CData(*idata));
	}
	if( waitForResponse )
	{
		int timeDelay = CMacProtocol::getTransmissionDelay(packetsToSend);
		node->delaySleep(timeDelay);
	}

	return packetsToSend;

}

void CEpidemic::CommunicateBetweenNeighbors(int now)
{
	if( now % getConfig<int>("log", "slot_log") == 0 )
	{
		if( now > 0 )
		{
			CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
		}
		CPrintHelper::PrintNewLine();
		CPrintHelper::PrintHeading(now, "DATA DELIVERY");
	}

	// Prophet: sink => nodes
	CSink* sink = CSink::getSink();
	//use default neighbor pool (only sensor nodes)
	CMacProtocol::transmitFrame(*sink, sink->sendRTS(now), now, receivePackets);

	vector<CNode*> nodes = CNode::getAllNodes();

	for( vector<CNode*>::iterator srcNode = nodes.begin(); srcNode != nodes.end(); ++srcNode )
	{
		if( ( *srcNode )->isDiscovering() )
		{
			CMacProtocol::transmitFrame(**srcNode, ( *srcNode )->sendRTSWithCapacityAndIndex(now), now, receivePackets);
			( *srcNode )->finishDiscovering();
		}
	}
}

bool CEpidemic::Init(int now)
{
	if( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc )
		CHDC::Init();
	else if( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac )
		CSMac::Init();

	return true;
}

bool CEpidemic::Operate(int now)
{
	if( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc )
		CHDC::Prepare(now);
	else if( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac )
		CSMac::Prepare(now);

	if( !CNode::UpdateNodeStatus(now) )
		return false;

	if( now < getConfig<int>("simulation", "runtime") )
		CommunicateBetweenNeighbors(now);

	PrintInfo(now);

	return true;
}
