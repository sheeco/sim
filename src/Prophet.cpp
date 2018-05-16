#include "Global.h"
#include "Configuration.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "Prophet.h"
#include "SortHelper.h"
#include "PrintHelper.h"

map<int, map<int, CProphet::CDeliveryPredRecord>> CProphet::deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���

double CProphet::INIT_PRED;
double CProphet::DECAY_PRED;
double CProphet::TRANS_PRED;
bool CProphet::TRANS_STRICT_BY_PRED;


CProphet::CProphet()
{
	
}

CProphet::~CProphet()
{
	
}

void CProphet::initDeliveryPreds(int now)
{
	vector<CNode*> nodes = CNode::getAllNodes();
	for( CNode* pnode : nodes )
	{
		deliveryPreds[pnode->getID()][CSink::getSink()->getID()] = CDeliveryPredRecord(INIT_PRED, now);
		for( CNode* pWithNode : nodes )
		{
			if( pnode->getID() != pWithNode->getID() )
				deliveryPreds[pnode->getID()][pWithNode->getID()] = CDeliveryPredRecord(INIT_PRED, now);
		}
	}
}

void CProphet::decayDeliveryPreds(int forNode, int withNode, int now)
{
	CDeliveryPredRecord& record = deliveryPreds[forNode][withNode];

	if( record.timestamp == now )
		return;

	record.pred *= pow(DECAY_PRED, ( now - record.timestamp ) / getConfig<int>("trace", "interval"));
	record.timestamp = now;
}

void CProphet::DecayDeliveryPreds(CNode * node, int now)
{
	if( now % getConfig<int>("trace", "interval") != 0 )
		return;

	int sink = CSink::getSink()->getID();
	for( CNode* pnode : CNode::getAliveNodes() )
	{
		int a = pnode->getID();
		decayDeliveryPreds(a, sink, now);

		for( CNode* pWithNode : CNode::getAliveNodes() )
		{
			int b = pWithNode->getID();
			if( pnode->getID() != pWithNode->getID() )
			{
				decayDeliveryPreds(a, b, now);
			}
		}
	}
}

void CProphet::updateDeliveryPredsBetween(int a, int b, int now)
{
	map<int, CDeliveryPredRecord > &recordA = deliveryPreds[a],
		recordB = deliveryPreds[b];

	//avoid duplicate update
	if( recordA[b].timestamp == now )
		return;

	//update P(a, b) & P(b, a)
	decayDeliveryPreds(a, b, now);
	decayDeliveryPreds(b, a, now);
	double oldPred = recordA[b].pred;
	double betweenAB = oldPred + ( 1 - oldPred ) * INIT_PRED;
	recordA[b] = recordB[a] = CDeliveryPredRecord(betweenAB, now);


	int sink = CSink::getSink()->getID();
	decayDeliveryPreds(a, sink, now);
	decayDeliveryPreds(b, sink, now);
	double a2sink = recordA[sink].pred;  //copy
	double b2sink = recordB[sink].pred;

	//update P(a, sink)
	oldPred = a2sink;
	recordA[sink].pred = oldPred + ( 1 - oldPred ) * betweenAB * b2sink * TRANS_PRED;
	recordA[sink].timestamp = now;

	//update P(b, sink)
	oldPred = b2sink;
	recordB[sink].pred = oldPred + ( 1 - oldPred ) * betweenAB * a2sink * TRANS_PRED;
	recordB[sink].timestamp = now;

	for( CNode* pnode : CNode::getAliveNodes() )
	{
		if( pnode->getID() != a
		   && pnode->getID() != b )
		{
			int d = pnode->getID();
			decayDeliveryPreds(a, d, now);
			decayDeliveryPreds(b, d, now);
			double a2d = recordA[d].pred;  //copy
			double b2d = recordB[d].pred;

			//update P(a, d)
			oldPred = a2d;
			recordA[d].pred = oldPred + ( 1 - oldPred ) * betweenAB * b2d* TRANS_PRED;
			recordA[d].timestamp = now;

			//update P(b, d)
			oldPred = b2d;
			recordB[d].pred = oldPred + ( 1 - oldPred ) * betweenAB * a2d * TRANS_PRED;
			recordB[d].timestamp = now;
		}
	}
}

void CProphet::updateDeliveryPredsWithSink(int node, int now)
{
	int sink = CSink::getSink()->getID();
	if( deliveryPreds[node][sink].timestamp == now )
		return;

	decayDeliveryPreds(node, sink, now);

	double oldPred = deliveryPreds[node][sink].pred;
	deliveryPreds[node][sink].pred = oldPred + ( 1 - oldPred ) * INIT_PRED;
	deliveryPreds[node][sink].timestamp = now;
}

bool CProphet::shouldForward(int me, int you, int now)
{
	int sink = CSink::getSink()->getID();
	decayDeliveryPreds(me, sink, now);
	decayDeliveryPreds(you, sink, now);

	double me2sink = deliveryPreds[me][sink].pred,
		you2sink = deliveryPreds[you][sink].pred;

	if( EQUAL( me2sink, you2sink ) )
	{
		if( TRANS_STRICT_BY_PRED )
			return false;
		else
			return Bet(0.5);
	}
	else
		return you2sink > me2sink;
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

vector<CPacket*> CProphet::receivePackets(CNode* node, CSink* sink, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA��������Ϊ�գ�û���ʺϴ��������
	vector<CData> dataToSend;  //��vector����ܾ���������
	bool waitForResponse = false;

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

				updateDeliveryPredsWithSink(node->getID(), now);

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

				//�յ��յ�ACKʱ�������������ݴ���
				if( ctrl->getACK().empty() )
					return packetsToSend;
				//clear data with ack
				else
					node->dropDataByAck( ctrl->getACK() );

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
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			packetsToSend.push_back(new CData(*idata));
	}
	if( waitForResponse )
	{
		int timeDelay = CMacProtocol::getTransmissionDelay(packetsToSend);
		node->delaySleep(timeDelay);
	}
	return packetsToSend;
	
}

vector<CPacket*> CProphet::receivePackets(CSink* sink, CNode* fromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* ackToSend = nullptr;

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
			vector<CData> ack = CSink::bufferData(now, datas);

			//ACK������յ�������ȫ�������������Ϳյ�ACK��
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
vector<CPacket*> CProphet::receivePackets(CNode* node, CNode* fromNode, vector<CPacket*> packets, int now)
{
	vector<CPacket*> packetsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* capacityToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA��������Ϊ�գ�û���ʺϴ��������
	CCtrl* ackToSend = nullptr;
	vector<CData> dataToSend;  //��vector����ܾ���������
	int capacity = INVALID;
	bool waitForResponse = false;

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

			// _index���ڴ����ڵ�֮���DP���£���ÿ���ڵ��֮��ֻ�����һ�Σ����_index����_rts����
			case CCtrl::_index:

				//update preds
				updateDeliveryPredsBetween(node->getID(), fromNode->getID(), now);

				// + DATA / NODATA
				//·��Э��������ýڵ�ת������
				if( shouldForward(node->getID(), fromNode->getID(), now ) )
				{
					if( capacity > 0 )
						dataToSend = node->getDataForTrans(fromNode->getBufferHistory(), capacity);

					//������Ϊ��
					if( capacity == 0 
					   || dataToSend.empty() )
						nodataToSend = new CCtrl(node->getID(), now, CCtrl::_no_data);

					//���·��Э��������ýڵ㷢�����ݣ��Է��ڵ�Ͳ����������ݣ�������跢��capacity
					else
					{
						waitForResponse = true;
						FreePointer(capacityToSend);
					}
				}
				//���򣬲�����ת������ʱ��dataToSend����

				//ע�⣺�����ȡ����Prophet��Ҫ�������ڵ㶼�ܾ��������ݣ��˴������¿յ���Ӧ��ֱ�ӽ����������ݴ���
				//     ���򲢲��������䣬����Ϊ�Է����������ݷ���ACK��

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//��������ھ��б�
				node->updateCommunicationHistory( fromNode->getID(), now );

				node->startDiscovering();

				//�յ��յ�ACKʱ�������������ݴ���
				if( ctrl->getACK().empty() )
					return packetsToSend;
				
				else

				CPrintHelper::PrintCommunication(now, node->getName(), fromNode->getName(), ctrl->getACK().size());

				return packetsToSend;

				break;

			case CCtrl::_no_data:

				//�յ�NODATA��Ҳ���ظ�һ���յ�ACK������Ҳ������Ϊ���ݴ���ɹ�
				//�յ�ACK
				ackToSend = new CCtrl(node->getID(), vector<CData>(), now, CCtrl::_ack);
				//��������ھ��б�
				node->updateCommunicationHistory( fromNode->getID(), now );
				node->startDiscovering();


				//ά�����ݴ���ĵ����ԣ�����յ����ݻ�NODATA���Ͳ��������ݻ�NODATA��ע�⣺ǰ���ǿ��ư����������ݰ�֮ǰ��
				dataToSend.clear();
				if( nodataToSend != nullptr )
				{
					FreePointer(nodataToSend);
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
				FreePointer(nodataToSend);
				nodataToSend = nullptr;
			}

			//accept data into buffer
			vector<CData> ack;
			//�����յ������ݲ�����ACK
			ack = node->bufferData(now, datas);
			ackToSend = new CCtrl(node->getID(), ack, now, CCtrl::_ack);
			//��������ھ��б�
			node->updateCommunicationHistory( fromNode->getID(), now );
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
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			packetsToSend.push_back(new CData(*idata));
	}
	if( waitForResponse )
	{
		int timeDelay = CMacProtocol::getTransmissionDelay(packetsToSend);
		node->delaySleep(timeDelay);
	}

	return packetsToSend;

}

void CProphet::CommunicateBetweenNeighbors(int now)
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

bool CProphet::Init(int now)
{
	if(getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc)
		CHDC::Init();
	else if(getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_smac)
		CSMac::Init();

	INIT_PRED = getConfig<double>("prophet", "init_pred");
	DECAY_PRED = getConfig<double>("prophet", "decay_pred");
	TRANS_PRED = getConfig<double>("prophet", "trans_pred");
	TRANS_STRICT_BY_PRED = getConfig<bool>("prophet", "trans_strict_by_pred");

	CProphet::initDeliveryPreds(now);
	return true;
}

bool CProphet::Operate(int now)
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
