#include "ProphetNode.h"
#include "Prophet.h"


CProphetNode::CProphetNode(double dataRate) : CNode(dataRate)
{
}

CProphetNode::~CProphetNode()
{
}

//void CProphetNode::init()
//{
//	CDutyCycle::init();
//
//	prophetNodes.push_back(*this);
//}

void CProphetNode::initProphetNodes()
{
	if( ! checkInitialization() )
	{
		for( int i = 0; i < INIT_NUM_NODE; ++i )
		{
			double dataRate = DEFAULT_DATA_RATE;
			if( i % 5 == 0 )
				dataRate *= 5;
			CProphetNode* node = new CProphetNode(dataRate);
			node->init();
			CNode::pushBackNode(*node);
		}
		vector<CProphetNode &> prophetNodes = getProphetNodes();
		for( vector<CProphetNode &>::iterator inode = prophetNodes.begin(); inode != prophetNodes.end(); ++inode )
			(*inode).initDeliveryPreds();
	}
}

CProphetNode & CProphetNode::toProphetNode(CNode & node)
{
	try
	{
		return dynamic_cast< CProphetNode & >( node );
	}
	catch( exception ex )
	{
		throw string("CProphetNode::toProphetNode() : Unvalid type cast");
	}
}

vector<CProphetNode &>& CProphetNode::getProphetNodes()
{
	if( !checkInitialization() )
		initProphetNodes();

	vector<CProphetNode &> prophetNodes;
	for( vector<CNode &>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
		prophetNodes.push_back( toProphetNode(*inode) );
	
	return prophetNodes;
}

void CProphetNode::updateStatus(int currentTime)
{
	if( this->time == currentTime )
		return;

	if( ROUTING_PROTOCOL == _prophet
	   && ( currentTime % CCTrace::SLOT_TRACE ) == 0 )
		decayDeliveryPreds(currentTime);

	CDutyCycle::updateStatus(currentTime);
}

CFrame* CProphetNode::sendRTSWithCapacityAndPred(int currentTime)
{
	vector<CPacket*> packets;
	if( MODE_RECEIVE == _selfish
	   && ( !buffer.empty() ) )
		packets.push_back(new CCtrl(ID, capacityBuffer - buffer.size(), currentTime, SIZE_CTRL, CCtrl::_capacity));
	packets.push_back(new CCtrl(ID, currentTime, SIZE_CTRL, CCtrl::_rts));
	packets.push_back(new CCtrl(ID, deliveryPreds, currentTime, SIZE_CTRL, CCtrl::_index));
	CFrame* frame = new CFrame(*this, packets);

	return frame;
}

void CProphetNode::initDeliveryPreds()
{
	map<int, double> deliveryPreds = getDeliveryPreds();
	if( !deliveryPreds.empty() )
		return;

	vector<CNode&> nodes = CNode::getNodes();
	for( int id = 0; id <= nodes.size(); ++id )
	{
		if( id != getID() )
			deliveryPreds[id] = CProphet::INIT_PRED;
	}
	setDeliveryPreds(deliveryPreds);
}

void CProphetNode::decayDeliveryPreds(int currentTime)
{
	// TODO: refine decay ratio ?
	map<int, double> deliveryPreds = getDeliveryPreds();
	for( map<int, double>::iterator imap = deliveryPreds.begin(); imap != deliveryPreds.end(); ++imap )
		deliveryPreds[imap->first] = imap->second * pow(CProphet::RATIO_PRED_DECAY, ( currentTime - getTime() ) / CCTrace::SLOT_TRACE);
	setDeliveryPreds(deliveryPreds);
}

void CProphetNode::updateDeliveryPredsWith(int fromNode, map<int, double> preds)
{
	map<int, double> deliveryPreds = getDeliveryPreds();
	double oldPred = 0, transPred = 0, dstPred = 0;
	if( deliveryPreds.find(fromNode) == deliveryPreds.end() )
		deliveryPreds[fromNode] = CProphet::INIT_PRED;

	oldPred = deliveryPreds[fromNode];
	deliveryPreds[fromNode] = transPred = oldPred + ( 1 - oldPred ) * CProphet::INIT_PRED;

	for( map<int, double>::iterator imap = preds.begin(); imap != preds.end(); ++imap )
	{
		int dst = imap->first;
		if( dst == getID() )
			continue;
		if( deliveryPreds.find(fromNode) == deliveryPreds.end() )
			deliveryPreds[fromNode] = CProphet::INIT_PRED;

		oldPred = deliveryPreds[dst];
		dstPred = imap->second;
		deliveryPreds[dst] = oldPred + ( 1 - oldPred ) * transPred * dstPred * CProphet::RATIO_PRED_TRANS;
	}
	setDeliveryPreds(deliveryPreds);
}

void CProphetNode::updateDeliveryPredsWithSink(CSink * sink)
{
	map<int, double> deliveryPreds = getDeliveryPreds();
	double oldPred = deliveryPreds[sink->getID()];
	deliveryPreds[sink->getID()] = oldPred + ( 1 - oldPred ) * CProphet::INIT_PRED;
	setDeliveryPreds(deliveryPreds);
}

bool CProphetNode::shouldForward(map<int, double> dstPred)
{
	double predNode = getDeliveryPreds().find(CSink::getSink()->getID())->second;
	double predDst = dstPred.find(CSink::getSink()->getID())->second;

	if( predNode == predDst )
	{
		if( CProphet::TRANS_STRICT_BY_PRED )
			return false;
		else
			return Bet(0.5);
		//return true;
	}
	else
		return ( predDst > predNode );
}

vector<CData> CProphetNode::getDataForTrans()
{
	vector<CData> datas = getAllData();

	if( CRoutingProtocol::WINDOW_TRANS > 0 )
		CNode::removeDataByCapacity(datas, CRoutingProtocol::WINDOW_TRANS, false);

	return datas;
}

vector<CData> CProphetNode::bufferData(vector<CData> datas, int time)
{
	if( datas.empty() )
		return vector<CData>();

	for( vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata )
		idata->arriveAnotherNode(time);

	pushIntoBuffer(datas);
	vector<CData> ack = datas;

	vector<CData> overflow = updateBufferStatus(time);

	RemoveFromList(ack, overflow);

	return ack;
}

