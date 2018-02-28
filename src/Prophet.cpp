#include "Global.h"
#include "Configuration.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "Prophet.h"
#include "SortHelper.h"
#include "PrintHelper.h"


CProphet::CProphet()
{
	
}

CProphet::~CProphet()
{
	
}

void CProphet::initDeliveryPreds(CNode * node)
{
	map<int, double> deliveryPreds = node->getDeliveryPreds();
	if( ! deliveryPreds.empty() )
		return;

	vector<CNode*> nodes = CNode::getNodes();
	for( int id = 0; id <= nodes.size(); ++id )
	{
		if( id != node->getID() )
			deliveryPreds[id] = getConfig<double>("prophet", "init_pred");
	}
	node->setDeliveryPreds(deliveryPreds);
}

void CProphet::decayDeliveryPreds(CNode * node, int now)
{
	map<int, double> deliveryPreds = node->getDeliveryPreds();
	for( map<int, double>::iterator imap = deliveryPreds.begin(); imap != deliveryPreds.end(); ++imap )
		deliveryPreds[imap->first] = imap->second * pow(getConfig<double>("prophet", "decay_pred"), ( now - node->getTime() ) / getConfig<int>("trace", "interval"));
	node->setDeliveryPreds(deliveryPreds);
}

void CProphet::updateDeliveryPredsWith(CNode * node, int fromNode, map<int, double> preds)
{
	map<int, double> deliveryPreds = node->getDeliveryPreds();
	double oldPred = 0, transPred = 0, dstPred = 0;
	if( deliveryPreds.find(fromNode) == deliveryPreds.end() )
		deliveryPreds[fromNode] = getConfig<double>("prophet", "init_pred");

	oldPred = deliveryPreds[fromNode];
	deliveryPreds[fromNode] = transPred = oldPred + ( 1 - oldPred ) * getConfig<double>("prophet", "init_pred");

	for( map<int, double>::iterator imap = preds.begin(); imap != preds.end(); ++imap )
	{
		int dst = imap->first;
		if( dst == node->getID() )
			continue;
		if( deliveryPreds.find(fromNode) == deliveryPreds.end() )
			deliveryPreds[fromNode] = getConfig<double>("prophet", "init_pred");

		oldPred = deliveryPreds[dst];
		dstPred = imap->second;
		deliveryPreds[dst] = oldPred + ( 1 - oldPred ) * transPred * dstPred * getConfig<double>("prophet", "trans_pred");
	}
	node->setDeliveryPreds(deliveryPreds);
}

void CProphet::updateDeliveryPredsWithSink(CNode * node, CSink * sink)
{
	map<int, double> deliveryPreds = node->getDeliveryPreds();
	double oldPred = deliveryPreds[sink->getID()];
	deliveryPreds[sink->getID()] = oldPred + ( 1 - oldPred ) * getConfig<double>("prophet", "init_pred");
	node->setDeliveryPreds(deliveryPreds);
}

bool CProphet::shouldForward(CNode* node, map<int, double> dstPred)
{
	double predNode = node->getDeliveryPreds().find(CSink::getSink()->getID())->second;
	double predDst = dstPred.find(CSink::getSink()->getID())->second;

	if( predNode == predDst )
	{
		if( getConfig<bool>("prophet", "trans_strict_by_pred") )
			return false;
		else
			return Bet(0.5);
			//return true;
	}
	else
		return ( predDst > predNode );
}

//vector<CData> CProphet::getDataForTrans(CNode* node)
//{
//	vector<CData> datas = node->getAllData();
//
//	if( getConfig<int>("trans", "window_trans") > 0 )
//		CNode::removeDataByCapacity(datas, getConfig<int>("trans", "window_trans"), false);
//
//	return datas;
//}

vector<CData> CProphet::bufferData(CNode* node, vector<CData> datas, int time)
{
	if( datas.empty() )
		return vector<CData>();

	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
		idata->arriveAnotherNode(time);

	node->pushIntoBuffer(datas);
	vector<CData> ack = datas;

	vector<CData> overflow = node->updateBufferStatus(time);

	RemoveFromList(ack, overflow);

	return ack;
}

vector<CPacket*> CProphet::receivePackets(CGeneralNode & gToNode, CGeneralNode & gFromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;

	if( typeid( gToNode ) == typeid( CSink ) )
	{
		CSink* toSink = dynamic_cast< CSink* >( &gToNode );

		/*********************************************** Sink <- Node *******************************************************/

		if( typeid( gFromNode ) == typeid( CNode ) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( &gFromNode );
			packetsToSend = CProphet::receivePackets(toSink, fromNode, packets, now);
		}
	}

	else if( typeid( gToNode ) == typeid( CNode ) )
	{
		CNode* node = dynamic_cast<CNode*>( &gToNode );

		/*********************************************** Node <- Sink *******************************************************/

		if( typeid( gFromNode ) == typeid( CSink ) )
		{
			CSink* fromSink = dynamic_cast< CSink* >( &gFromNode );
			packetsToSend = CProphet::receivePackets(node, fromSink, packets, now);
		}

		/*********************************************** Node <- Node *******************************************************/

		else if( typeid( gFromNode ) == typeid( CNode ) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( &gFromNode );
			packetsToSend = CProphet::receivePackets(node, fromNode, packets, now);
		}
	}

	return packetsToSend;
}

vector<CPacket*> CProphet::receivePackets(CNode* node, CSink* sink, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
	vector<CData> dataToSend;  //空vector代表拒绝传输数据
	bool wait = false;

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

				updateDeliveryPredsWithSink(node, sink);

				if( node->getAllData().empty() )
				{
					return packetsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(node->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);
				// + DATA
				dataToSend = getDataForTrans(node, 0, true);

				wait = true;
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
					node->checkDataByAck( ctrl->getACK() );

				CPrintHelper::PrintCommunication(time, node->format(), sink->format(), ctrl->getACK().size());

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
	if( wait )
	{
		int timeDelay = CMacProtocol::getTransmissionDelay(packetsToSend);
		node->delayDiscovering(timeDelay);
		node->delaySleep(timeDelay);
	}
	return packetsToSend;
	
}

vector<CPacket*> CProphet::receivePackets(CSink* sink, CNode* fromNode, vector<CPacket*> packets, int time)
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

			//ACK（如果收到的数据全部被丢弃，发送空的ACK）
			ctrlToSend = new CCtrl(CSink::getSink()->getID(), ack, time, getConfig<int>("data", "size_ctrl"), CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
		packetsToSend.push_back(ctrlToSend);

	return packetsToSend;

}

vector<CPacket*> CProphet::receivePackets(CNode* node, CNode* fromNode, vector<CPacket*> packets, int time)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* capacityToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
	vector<CData> dataToSend;  //空vector代表拒绝传输数据
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
				if( node->hasSpokenRecently(dynamic_cast<CNode*>(fromNode), time) )
				{
					CPrintHelper::PrintCommunication(time, node->format(), fromNode->format(), "skip");
					return packetsToSend;
				}
				//rcv RTS from node
				else
				{
					//update preds
					updateDeliveryPredsWith(node, fromNode->getID(), ctrl->getPred());

					//CTS
					ctrlToSend = new CCtrl(node->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_cts);

					// + Capacity
					if( getConfig<CConfiguration::EnumRelayScheme>("node", "scheme_relay") == config::_selfish 
						&& node->hasData() )
						capacityToSend = new CCtrl(node->getID(), node->getDeliveryPreds(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_index);

					// + Index
					indexToSend = new CCtrl(node->getID(), node->getDeliveryPreds(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_index);
					//node->skipRTS();  //（暂未使用）
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
				//路由协议允许向该节点转发数据
//				debugNodeID = node->getID();
//				debugFromID = fromNode->getID();
				if( shouldForward(node, ctrl->getPred() ) )
				{
					if( capacity == 0 )
					{
						return packetsToSend;
					}

					dataToSend = getDataForTrans(node, 0, true);

					//但缓存为空
					if( dataToSend.empty() )
						nodataToSend = new CCtrl(node->getID(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_no_data);
					else if( capacity > 0 )
					{
						CNode::removeDataByCapacity(dataToSend, capacity, false);
					}
					
					//如果路由协议允许向该节点发送数据，对方节点就不允许发送数据，因此无需发送capacity
					if( capacityToSend != nullptr )
					{
						free(capacityToSend);
						capacityToSend = nullptr;
					}
				}
				//否则，不允许转发数据时，dataToSend留空

				//注意：如果（取决于Prophet的要求）两个节点都拒绝发送数据，此处将导致空的响应，直接结束本次数据传输
				//     否则并不结束传输，还将为对方发来的数据发送ACK；

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//加入最近邻居列表
				node->addToSpokenCache( fromNode, time );

				node->startDiscovering();

				//收到空的ACK时，结束本次数据传输
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					node->checkDataByAck( ctrl->getACK() );

				CPrintHelper::PrintCommunication(time, node->format(), fromNode->format(), ctrl->getACK().size());

				return packetsToSend;

				break;

			case CCtrl::_no_data:

				//收到NODATA，也将回复一个空的ACK，即，也将被认为数据传输成功
				//空的ACK
				ctrlToSend = new CCtrl(node->getID(), vector<CData>(), time, getConfig<int>("data", "size_ctrl"), CCtrl::_ack);
				//加入最近邻居列表
				node->addToSpokenCache( fromNode, time );
				node->startDiscovering();


				//维持数据传输的单向性：如果收到数据或NODATA，就不发送数据或NODATA（注意：前提是控制包必须在数据包之前）
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

			//维持数据传输的单向性：如果收到数据或NODATA，就不发送数据或NODATA（注意：前提是控制包必须在数据包之前）
			dataToSend.clear();
			if( nodataToSend != nullptr )
			{
				free(nodataToSend);
				nodataToSend = nullptr;
			}

			//accept data into buffer
			vector<CData> ack;
			ack = CProphet::bufferData(node, datas, time);
			//ACK（如果收到的数据全部被丢弃，发送空的ACK）
			ctrlToSend = new CCtrl(node->getID(), ack, time, getConfig<int>("data", "size_ctrl"), CCtrl::_ack);
			//加入最近邻居列表
			node->addToSpokenCache( fromNode, time );
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

bool CProphet::Init()
{
	vector<CNode*> nodes = CNode::getNodes();
	for( vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
		CProphet::initDeliveryPreds(*inode);
	return true;
}

bool CProphet::Operate(int now)
{
	bool hasNodes = true;
	if( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc )
		hasNodes = CHDC::Prepare(now);
	else if( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac )
		hasNodes = CSMac::Prepare(now);

	if( ! hasNodes )
		return false;

	if( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc )
		CHDC::Operate(now);
	else if( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac )
		CSMac::Operate(now);

	PrintInfo(now);

	return true;
}
