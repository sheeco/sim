#include "MacProtocol.h"
#include "Configuration.h"
#include "Node.h"
#include "Sink.h"
#include "MANode.h"
#include "Prophet.h"
#include "HAR.h"
#include "HotspotSelect.h"
#include "PrintHelper.h"

int CMacProtocol::transmitSuccessful = 0;
int CMacProtocol::transmit = 0;


CMacProtocol::CMacProtocol()
{
}

bool CMacProtocol::transmitFrame(CGeneralNode& src, CFrame* frame, int currentTime)
{
	static map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > > frameArrivals;

	bool rcv = false;
	vector<CGeneralNode*> neighbors;

	src.consumeEnergy(frame->getSize() * getConfig<double>("trans", "consumption_byte_send"), currentTime);
	CMacProtocol::transmitTry();

	/************************************************ Sensor Node *******************************************************/

	vector<CNode*> nodes = CNode::getNodes();
	for( vector<CNode*>::iterator idstNode = nodes.begin(); idstNode != nodes.end(); ++idstNode )
	{
		CNode* dstNode = *idstNode;
		//skip itself
		if( ( dstNode )->getID() == src.getID() )
			continue;

		if( CBasicEntity::withinRange(src, *dstNode, getConfig<int>("trans", "range_trans")) )
		{
			//统计sink节点的相遇计数
			if( typeid( src ) == typeid( CSink ) )
				CSink::encount();

			if( dstNode->isAwake() )
			{
				//统计sink节点的相遇计数
				if( typeid( src ) == typeid( CSink ) )
					CSink::encountActive();

				if( Bet( getConfig<double>("trans", "prob_trans")) )
					neighbors.push_back(dstNode);
			}
		}
	}

	/*************************************************** Sink **********************************************************/

	CSink* sink = CSink::getSink();
	if( CBasicEntity::withinRange(src, *sink, getConfig<int>("trans", "range_trans"))
	   && Bet( getConfig<double>("trans", "prob_trans"))
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

		if( CBasicEntity::withinRange(src, **iMA, getConfig<int>("trans", "range_trans"))
		   && Bet( getConfig<double>("trans", "prob_trans"))
		   && ( *iMA )->isAwake() )
		{
			neighbors.push_back(*iMA);
		}
	}

	int timeTrans = 0;
	timeTrans = int( getTransmissionDelay(frame) );
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
			bool new_rcv = receiveFrame(**ineighbor, currentFrame, currentTime);
			rcv = rcv || new_rcv;
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
	
	//if( gnode.getTime() > currentTime )
	//	return false;

	//if( gnode.isOccupied() )
	//	return false;

	int timeTrans = 0;
	timeTrans = int( getTransmissionDelay(frame) );
	//gnode.Occupy(timeTrans);    
	//if( timeTrans > 0 )
	//	gnode.updateStatus(currentTime + timeTrans);

	//非广播且目标节点非本节点，即过听
	if( gToNode != nullptr
	    && gToNode->getID() != gnode.getID() )
	{
		gnode.Overhear(currentTime);
		return false;
	}
	
	gnode.consumeEnergy(frame->getSize() * getConfig<double>("trans", "consumption_byte_receive"), currentTime);
	
	vector<CPacket*> packets = frame->getPackets();
	vector<CPacket*> packetsToSend;
	CFrame* frameToSend = nullptr;

	switch( getConfig<CConfiguration::EnumRoutingProtocolScheme>("simulation", "routing_protocol") )
	{
		case config::_prophet:

			packetsToSend = CProphet::receivePackets(gnode, *gFromNode, packets, currentTime);

			break;

		case config::_xhar:

			packetsToSend = HAR::receivePackets(gnode, *gFromNode, packets, currentTime);

			break;
		
		default:
			break;
	}

	/*********************************************** send reply *******************************************************/

	if( ! packetsToSend.empty() )
	{
		frameToSend = new CFrame(gnode, *gFromNode, packetsToSend);
		transmitFrame( gnode, frameToSend, currentTime + timeTrans);
	}
	else
	{
		CMacProtocol::transmitSucceed();
	}
	return true;
}

bool CMacProtocol::UpdateNodeStatus(int currentTime)
{
	vector<CNode *> nodes = CNode::getNodes();
	for( CNode * inode : nodes )
		inode->updateStatus(currentTime);

	//按照轨迹文件的时槽统计节点相遇计数
	if( ! CNode::hasNodes(currentTime) )
		return false;

	nodes = CSortHelper::mergeSort(nodes);

	if( currentTime % getConfig<int>("trace", "interval") == 0 )
	{
		for( vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
		{
			for( vector<CNode *>::iterator jnode = inode; jnode != nodes.end(); ++jnode )
			{
				if( ( *inode )->getX() + getConfig<int>("trans", "range_trans") < ( *jnode )->getX() )
					break;
				if( CBasicEntity::withinRange(**inode, **jnode, getConfig<int>("trans", "range_trans")) )
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
	CSink::getSink()->updateStatus(currentTime);

	if( ! CMacProtocol::UpdateNodeStatus(currentTime) )
		return false;

	//热点选取
	if( getConfig<CConfiguration::EnumHotspotSelectScheme>("simulation", "hotspot_select") != config::_skip )
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
		CPrintHelper::PrintHeading(currentTime, "DATA DELIVERY");
		print = false;
	}

	// TODO: sink receive RTS / send by slot ?
	// Prophet: sink => nodes
	// xHAR: sink => MAs
	CSink* sink = CSink::getSink();
	transmitFrame( *sink, sink->sendRTS(currentTime) , currentTime );

	vector<CNode*> nodes = CNode::getNodes();
	vector<CMANode*> MAs = CMANode::getMANodes();

	switch( getConfig<CConfiguration::EnumRoutingProtocolScheme>("simulation", "routing_protocol") )
	{
	case config::_prophet:

		for(vector<CNode*>::iterator srcNode = nodes.begin(); srcNode != nodes.end(); ++srcNode )
		{
			if( (*srcNode)->isDiscovering() )
			{
				transmitFrame( **srcNode, (*srcNode)->sendRTSWithCapacityAndPred(currentTime), currentTime );
				(*srcNode)->finishDiscovering();
			}
		}
		
		break;

	case config::_xhar:
		
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

	if( ( currentTime + getConfig<int>("simulation", "slot") ) % getConfig<int>("log", "slot_log") == 0 )
	{
		CPrintHelper::PrintPercentage("Delivery Ratio", CData::getDeliveryRatio());
		CPrintHelper::PrintNewLine();
		print = true;
	}
}

void CMacProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % getConfig<int>("log", "slot_log") == 0
			|| currentTime == getConfig<int>("simulation", "runtime") ) )
		return;

	//节点相遇、数据传输、能耗
	if( currentTime % getConfig<int>("log", "slot_log") == 0
	   || currentTime == getConfig<int>("simulation", "runtime") )
	{
		//节点个数
		ofstream node(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_node"), ios::app);
		if( currentTime == 0 )
		{
			node << endl << getConfig<string>("log", "info_log") << endl;
			node << getConfig<string>("log", "info_node") << endl;
		}
		node << currentTime << TAB << CNode::getNodeCount() << endl;
		node.close();

		//MA和节点 / 节点间的相遇次数
		ofstream encounter( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_encounter"), ios::app);
		if(currentTime == 0)
		{
			encounter <<  endl << getConfig<string>("log", "info_log") << endl ;
			encounter << getConfig<string>("log", "info_encounter") << endl;
		}
		encounter << currentTime << TAB;
		if( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc || getConfig<CConfiguration::EnumRoutingProtocolScheme>("simulation", "routing_protocol") == config::_xhar )
		{
			//encounter << CNode::getPercentEncounterActiveAtHotspot() << TAB << CNode::getEncounterActiveAtHotspot() << TAB;
			encounter << CNode::getPercentEncounterAtHotspot() << TAB << CNode::getEncounterAtHotspot() << TAB;
		}
		//encounter << CNode::getPercentEncounterActive() << TAB << CNode::getEncounterActive() << TAB;
		encounter << CNode::getEncounter() << TAB;
		encounter << endl;
		encounter.close();

		//sink和节点的相遇次数
		ofstream sink( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_sink"), ios::app);
		if(currentTime == 0)
		{
			sink <<  endl << getConfig<string>("log", "info_log") << endl ;
			sink << getConfig<string>("log", "info_sink") << endl;
		}
		sink << currentTime << TAB;
		sink << CSink::getPercentEncounterActive() << TAB << CSink::getEncounterActive() << TAB << CSink::getEncounter() << TAB;
		sink << endl;
		sink.close();

	}

	if( currentTime % getConfig<int>("hs", "slot_hotspot_update") == 0
	   || currentTime == getConfig<int>("simulation", "runtime") )
	{
		//数据传输
		ofstream transmit( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_transmit"), ios::app);
		if(currentTime == 0)
		{
			transmit << endl << getConfig<string>("log", "info_log") << endl ;
			transmit << getConfig<string>("log", "info_transmit") << endl;
		}
		transmit << currentTime << TAB << CMacProtocol::getPercentTransmitSuccessful() << TAB << CMacProtocol::getTransmitSuccessful() << TAB << CMacProtocol::getTransmit() << TAB;
		transmit << endl;
		transmit.close();

		//节点唤醒时间
		ofstream activation(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_activation"), ios::app);
		if( currentTime == 0 )
		{
			activation << endl << getConfig<string>("log", "info_log") << endl;
			activation << getConfig<string>("log", "info_activation") << endl;
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
		ofstream energy_consumption( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_energy_consumption"), ios::app);
		if(currentTime == 0)
		{
			energy_consumption <<  endl << getConfig<string>("log", "info_log") << endl ; 
			energy_consumption << getConfig<string>("log", "info_energy_consumption") << endl;
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
	ofstream final( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_final"), ios::app);
	final << CData::getAverageEnergyConsumption() << TAB << CMacProtocol::getPercentTransmitSuccessful() << TAB;
	//final << CNode::getPercentEncounterActive() << TAB ;
	if( CNode::finiteEnergy() )
		final << currentTime << TAB << CNode::getNodes().size() << TAB ;

	final.close();

}