#include "Node.h"
#include "Sink.h"
#include "MANode.h"
#include "MacProtocol.h"
#include "Prophet.h"
#include "HAR.h"
#include "HotspotSelect.h"

int CMacProtocol::transmitSuccessful = 0;
int CMacProtocol::transmit = 0;
bool CMacProtocol::SYNC_DC = true;
int CMacProtocol::SIZE_HEADER_MAC = 0;  //Mac Header Size
bool CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
int CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;  //动态节点个数测试时，节点个数发生变化的周期


CMacProtocol::CMacProtocol()
{
}

bool CMacProtocol::transmitFrame(CGeneralNode& src, CFrame* frame, int currentTime)
{
	static map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > > frameArrivals;

	bool rcv = false;
	vector<CGeneralNode*> neighbors;

	src.consumeEnergy(frame->getSize() * CGeneralNode::CONSUMPTION_BYTE_SEND);
	CMacProtocol::transmitTry();

	/************************************************ Sensor Node *******************************************************/

	vector<CNode*> nodes = CNode::getNodes();
	for( vector<CNode*>::iterator idstNode = nodes.begin(); idstNode != nodes.end(); ++idstNode )
	{
		CNode* dstNode = *idstNode;
		//skip itself
		if( ( dstNode )->getID() == src.getID() )
			continue;

		if( CBasicEntity::withinRange(src, *dstNode, CGeneralNode::RANGE_TRANS) )
		{
			//统计sink节点的相遇计数
			if( typeid( src ) == typeid( CSink ) )
				CSink::encount();

			if( dstNode->isAwake() )
			{
				//统计sink节点的相遇计数
				if( typeid( src ) == typeid( CSink ) )
					CSink::encountActive();

				if( Bet(CGeneralNode::PROB_TRANS) )
					neighbors.push_back(dstNode);
			}
		}
	}

	/*************************************************** Sink **********************************************************/

	CSink* sink = CSink::getSink();
	if( CBasicEntity::withinRange(src, *sink, CGeneralNode::RANGE_TRANS)
	   && Bet(CGeneralNode::PROB_TRANS)
	   && sink->getID() != src.getID() )
	{
		neighbors.push_back(sink);
		CSink::encount();
		CSink::encountActive();
	}


	/**************************************************** MA ***********************************************************/

	vector<CMANode*> MAs = CMANode::getMANodes();
	for( vector<CMANode*>::iterator iMA = MAs.begin(); iMA != MAs.end(); ++iMA )
	{
		//skip itself
		if( ( *iMA )->getID() == src.getID() )
			continue;

		if( CBasicEntity::withinRange(src, **iMA, CGeneralNode::RANGE_TRANS)
		   && Bet(CGeneralNode::PROB_TRANS)
		   && ( *iMA )->isAwake() )
		{
			neighbors.push_back(*iMA);
		}
	}

	int timeTrans = 0;
	timeTrans = CNode::calTimeForTrans(frame);
	int timeArrival = currentTime + timeTrans;
	vector< pair<CFrame*, vector< CGeneralNode* > > > currentArrivals;

	//压入将来的到达队列
	if( timeArrival > currentTime )
	{
		vector< pair<CFrame*, vector< CGeneralNode* > > > futureArrivals;
		map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > >::iterator iArrivals = frameArrivals.find(timeArrival);
		if( iArrivals != frameArrivals.end() )
			futureArrivals = ( *iArrivals ).second;

		futureArrivals.push_back(pair<CFrame*, vector< CGeneralNode* > >(frame, neighbors));
		frameArrivals[timeArrival] = futureArrivals;
	}
	//压入当前的到达队列
	else
		currentArrivals.push_back(pair<CFrame*, vector< CGeneralNode* > >(frame, neighbors));

	//取出之前存入的当前时间的到达队列
	map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > >::iterator iMoreArrivals = frameArrivals.find(currentTime);
	if( iMoreArrivals != frameArrivals.end() )
	{
		vector< pair<CFrame*, vector< CGeneralNode* > > > moreCurrentArrivals = ( *iMoreArrivals ).second;
		currentArrivals.insert(currentArrivals.end(), moreCurrentArrivals.begin(), moreCurrentArrivals.end());

		//从队列中移除
		frameArrivals.erase(iMoreArrivals);
	}

	//投递队列中的数据
	for( vector< pair<CFrame*, vector< CGeneralNode* > > >::iterator iArrival = currentArrivals.begin();
		 iArrival != currentArrivals.end(); ++iArrival )
	{
		CFrame* currentFrame = iArrival->first;
		vector<CGeneralNode*> currentNeighbors = iArrival->second;
		for( vector<CGeneralNode*>::iterator ineighbor = currentNeighbors.begin(); ineighbor != currentNeighbors.end(); ++ineighbor )
		{
			rcv = rcv || receiveFrame(**ineighbor, currentFrame, currentTime);
		}

		free(currentFrame);
	}

	return rcv;

	// TODO: sort by distance with src node ?
}

bool CMacProtocol::receiveFrame(CGeneralNode& gnode, CFrame* frame, int currentTime)
{
	// Make local copy
	frame = new CFrame(*frame);
	CGeneralNode* gFromNode = frame->getSrcNode();
	CGeneralNode* gToNode = frame->getDstNode();
	
	//if( gnode.isOccupied() )
	//	return false;

	int timeTrans = 0;
	timeTrans = CNode::calTimeForTrans(frame);;
	//gnode.Occupy(timeTrans);    

	//非广播且目标节点非本节点，即过听
	if( gToNode != nullptr
	    && gToNode->getID() != gnode.getID() )
	{
		gnode.Overhear();
		return false;
	}
	
	gnode.consumeEnergy(frame->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECEIVE);
	
	vector<CPacket*> packets = frame->getPackets();
	vector<CPacket*> packetsToSend;
	CFrame* frameToSend = nullptr;

	switch( ROUTING_PROTOCOL )
	{
		case _prophet:

			packetsToSend = CProphet::receivePackets(gnode, *gFromNode, packets, currentTime);

			break;

		case _xhar:

			packetsToSend = HAR::receivePackets(gnode, *gFromNode, packets, currentTime);

			break;
		
		default:
			break;
	}

	/*********************************************** send reply *******************************************************/

	if( ! packetsToSend.empty() )
	{
		frameToSend = new CFrame(gnode, *gFromNode, packetsToSend);
		transmitFrame( gnode, frameToSend, currentTime );			
	}
	else
	{
		CMacProtocol::transmitSucceed();
	}
	return true;
}

void CMacProtocol::ChangeNodeNumber(int currentTime)
{
	if( ! ( currentTime % SLOT_CHANGE_NUM_NODE == 0 ) )
		return;

	flash_cout << endl << "####  < " << currentTime << " >  NODE NUMBER CHANGE" ;
	
	int delta = CNode::ChangeNodeNumber();

	if(delta >= 0)
	{
		cout << "######  ( " << delta << " nodes added )" << endl;
	}
	else
	{
		cout << "######  ( " << -delta << " nodes removed )" << endl;
	}
}

bool CMacProtocol::UpdateNodeStatus(int currentTime)
{
	bool death = false;
	vector<CNode *> nodes = CNode::getNodes();
	for( vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
	{
		(*inode)->updateStatus(currentTime);
		death = death || ( ! ( *inode )->isAlive() );
	}
	if( death )
	{
		CNode::ClearDeadNodes(currentTime);
	}

	//按照轨迹文件的时槽统计节点相遇计数
	if( nodes.empty() )
		return false;

	nodes = CSortHelper::mergeSort(nodes);

	if( currentTime % CCTrace::SLOT_TRACE == 0 )
	{
		for( vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
		{
			for( vector<CNode *>::iterator jnode = inode; jnode != nodes.end(); ++jnode )
			{
				if( ( *inode )->getX() + CGeneralNode::RANGE_TRANS < ( *jnode )->getX() )
					break;
				if( CBasicEntity::withinRange(**inode, **jnode, CGeneralNode::RANGE_TRANS) )
					CNode::encount();
			}
		}
	}

	return CNode::hasNodes(currentTime);
}

void CMacProtocol::UpdateMANodeStatus(int currentTime)
{
	vector<CMANode *> MAs = CMANode::getMANodes();

	for( vector<CMANode *>::iterator iMA = MAs.begin(); iMA != MAs.end(); ++iMA )
		( *iMA )->updateStatus(currentTime);
}

bool CMacProtocol::Prepare(int currentTime)
{
	//Node Number Test:
	if( TEST_DYNAMIC_NUM_NODE )
		CMacProtocol::ChangeNodeNumber(currentTime);

	CSink::getSink()->updateStatus(currentTime);

	if( ! CMacProtocol::UpdateNodeStatus(currentTime) )
		return false;

	//热点选取
	if( HOTSPOT_SELECT != _none )
	{
		CHotspotSelect::CollectNewPositions(currentTime);
		CHotspotSelect::HotspotSelect(currentTime);

		//检测节点所在热点区域
		CHotspot::UpdateAtHotspotForNodes(currentTime);

	}

	return true;
}

void CMacProtocol::CommunicateWithNeighbor(int currentTime)
{
	static bool print = false;
	if( currentTime == 0 
		|| print )
	{
		flash_cout << "####  < " << currentTime << " >  DATA DELIVERY            " << endl ;
		print = false;
	}

	// TODO: sink receive RTS / send by slot ?
	// Prophet: sink => nodes
	// xHAR: sink => MAs
	CSink* sink = CSink::getSink();
	transmitFrame( *sink, sink->sendRTS(currentTime) , currentTime );

	vector<CNode*> nodes = CNode::getNodes();
	vector<CMANode*> MAs = CMANode::getMANodes();

	switch( ROUTING_PROTOCOL )
	{
	case _prophet:

		for(vector<CNode*>::iterator srcNode = nodes.begin(); srcNode != nodes.end(); ++srcNode )
		{
			if( (*srcNode)->isDiscovering() )
			{
				transmitFrame( **srcNode, (*srcNode)->sendRTSWithCapacityAndPred(currentTime), currentTime );
				(*srcNode)->finishDiscovering();
			}
		}
		
		break;

	case _xhar:
		
		// xHAR: MAs => nodes
		for(vector<CMANode*>::iterator srcMA = MAs.begin(); srcMA != MAs.end(); ++srcMA )
		{
			// skip discover if buffer is full && _selfish is used
			if( (*srcMA)->getCapacityForward() > 0 )
				transmitFrame( **srcMA, (*srcMA)->sendRTSWithCapacity(currentTime) , currentTime );
		}
		// xHAR: no forward between nodes

		break;

	default:
		break;
				
	}

	if( ( currentTime + SLOT ) % SLOT_LOG == 0 )
	{
		double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
		flash_cout << "######  [ Delivery Ratio ]  " << deliveryRatio << " %                             " << endl << endl;
		print = true;
	}
}

void CMacProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_LOG == 0
			|| currentTime == RUNTIME ) )
		return;

	//节点相遇、数据传输、能耗
	if( currentTime % SLOT_LOG == 0
	   || currentTime == RUNTIME )
	{
		//节点个数
		ofstream node(PATH_ROOT + PATH_LOG + FILE_NODE, ios::app);
		if( currentTime == 0 )
		{
			node << endl << INFO_LOG << endl;
			node << INFO_NODE;
		}
		node << currentTime << TAB << CNode::getNNodes() << endl;
		node.close();

		//MA和节点 / 节点间的相遇次数
		ofstream encounter( PATH_ROOT + PATH_LOG + FILE_ENCOUNTER, ios::app);
		if(currentTime == 0)
		{
			encounter <<  endl << INFO_LOG << endl ;
			encounter << INFO_ENCOUNTER ;
		}
		encounter << currentTime << TAB;
		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _xhar )
		{
			//encounter << CNode::getPercentEncounterActiveAtHotspot() << TAB << CNode::getEncounterActiveAtHotspot() << TAB;
			encounter << CNode::getPercentEncounterAtHotspot() << TAB << CNode::getEncounterAtHotspot() << TAB;
		}
		//encounter << CNode::getPercentEncounterActive() << TAB << CNode::getEncounterActive() << TAB;
		encounter << CNode::getEncounter() << TAB;
		encounter << endl;
		encounter.close();

		//sink和节点的相遇次数
		ofstream sink( PATH_ROOT + PATH_LOG + FILE_SINK, ios::app);
		if(currentTime == 0)
		{
			sink <<  endl << INFO_LOG << endl ;
			sink << INFO_SINK ;
		}
		sink << currentTime << TAB;
		sink << CSink::getPercentEncounterActive() << TAB << CSink::getEncounterActive() << TAB << CSink::getEncounter() << TAB;
		sink << endl;
		sink.close();

	}

	if( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE == 0
	   || currentTime == RUNTIME )
	{
		//数据传输
		ofstream transmit( PATH_ROOT + PATH_LOG + FILE_TRANSMIT, ios::app);
		if(currentTime == 0)
		{
			transmit << endl << INFO_LOG << endl ;
			transmit << INFO_TRANSMIT ;
		}
		transmit << currentTime << TAB << CMacProtocol::getPercentTransmitSuccessful() << TAB << CMacProtocol::getTransmitSuccessful() << TAB << CMacProtocol::getTransmit() << TAB;
		transmit << endl;
		transmit.close();

		//节点唤醒时间
		ofstream activation(PATH_ROOT + PATH_LOG + FILE_ACTIVATION, ios::app);
		if( currentTime == 0 )
		{
			activation << endl << INFO_LOG << endl;
			activation << INFO_ACTIVATION;
		}
		activation << currentTime << TAB;
		vector<CNode *> allNodes = CNode::getAllNodes(true);
		for( auto inode = allNodes.begin(); inode != allNodes.end(); ++inode )
		{
			if( !( *inode )->isAlive() )
				activation << "-" << TAB;
			else
				activation << ( *inode )->getPercentTimeAwake() << "  ";
		}
		activation << endl;
		activation.close();

		//平均能耗
		ofstream energy_consumption( PATH_ROOT + PATH_LOG + FILE_ENERGY_CONSUMPTION, ios::app);
		if(currentTime == 0)
		{
			energy_consumption <<  endl << INFO_LOG << endl ; 
			energy_consumption << INFO_ENERGY_CONSUMPTION ;
		}
		energy_consumption << currentTime << TAB << CData::getAverageEnergyConsumption() ;
		if( CNode::finiteEnergy() )
		{			
			//节点剩余能量
			energy_consumption << TAB << CNode::getSumEnergyConsumption() << TAB << CNode::getNodes().size() << TAB;
			vector<CNode *> allNodes = CNode::getAllNodes(true);
			for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
				energy_consumption << (*inode)->getEnergy() << TAB;
		}
		energy_consumption << endl;
		energy_consumption.close();

	}

}

void CMacProtocol::PrintFinal(int currentTime)
{
	ofstream final( PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app);
	final << CData::getAverageEnergyConsumption() << TAB << CMacProtocol::getPercentTransmitSuccessful() << TAB;
	//final << CNode::getPercentEncounterActive() << TAB ;
	if( CNode::finiteEnergy() )
		final << currentTime << TAB << CNode::getNodes().size() << TAB ;

	final.close();

}