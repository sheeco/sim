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

vector<CGeneralNode*> CMacProtocol::findNeighbors(CGeneralNode & src)
{
	vector<CGeneralNode*> neighbors;

	/************************************************ Sensor Node *******************************************************/

	vector<CNode*> nodes = CNode::getAllNodes();
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

				neighbors.push_back(dstNode);
			}
		}
	}

	/*************************************************** Sink **********************************************************/

	CSink* sink = CSink::getSink();
	if( CBasicEntity::withinRange(src, *sink, getConfig<int>("trans", "range_trans"))
	   && sink->getID() != src.getID() )
	{
		neighbors.push_back(sink);
		CSink::encount();
		CSink::encountActive();
	}

	return neighbors;
	// TODO: sort by distance with src node ?
}

bool CMacProtocol::receiveFrame(CGeneralNode& gnode, CFrame* frame, int now, vector<CGeneralNode*>(*findNeighbors)( CGeneralNode& ), vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ))
{
	if(! Bet(getConfig<double>("trans", "prob_trans")) )
	   return false;

	// Make local copy
	frame = new CFrame(*frame);
	CGeneralNode* gFromNode = frame->getSrcNode();
	CGeneralNode* gToNode = frame->getDstNode();

	//if( gnode.getTime() > now )
	//	return false;

	//if( gnode.isOccupied() )
	//	return false;

	int timeTrans = 0;
	timeTrans = int(getTransmissionDelay(frame));
	//gnode.Occupy(timeTrans);    
	//if( timeTrans > 0 )
	//	gnode.updateStatus(now + timeTrans);

	//非广播且目标节点非本节点，即过听
	if( gToNode != nullptr
	   && gToNode->getID() != gnode.getID() )
	{
		gnode.Overhear(now);
		return false;
	}

	gnode.consumeEnergy(frame->getSize() * getConfig<double>("trans", "consumption_byte_receive"), now);

	vector<CPacket*> packets = frame->getPackets();
	vector<CPacket*> packetsToSend;
	CFrame* frameToSend = nullptr;

	packetsToSend = receivePackets(gnode, *gFromNode, packets, now);


	/*********************************************** send reply *******************************************************/

	if( !packetsToSend.empty() )
	{
		frameToSend = new CFrame(gnode, *gFromNode, packetsToSend);
		transmitFrame(gnode, frameToSend, now + timeTrans, findNeighbors, receivePackets);
	}
	else
	{
		CMacProtocol::transmitSucceed();
	}
	return true;
}

bool CMacProtocol::transmitFrame(CGeneralNode& src, CFrame* frame, int now, vector<CGeneralNode*>(*findNeighbors)( CGeneralNode& ), vector<CPacket*>(*receivePackets)( CGeneralNode &gToNode, CGeneralNode &gFromNode, vector<CPacket*> packets, int time ))
{
	static map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > > frameArrivals;

	CMacProtocol::transmitTry();
	src.consumeEnergy(frame->getSize() * getConfig<double>("trans", "consumption_byte_send"), now);

	int timeTrans = 0;
	timeTrans = int(CMacProtocol::getTransmissionDelay(frame));
	int timeArrival = now + timeTrans;
	vector< pair<CFrame*, vector< CGeneralNode* > > > currentArrivals;

	vector<CGeneralNode*> neighbors = findNeighbors(src);
	//压入将来的到达队列
	if( timeArrival > now )
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
	map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > >::iterator iMoreArrivals = frameArrivals.find(now);
	if( iMoreArrivals != frameArrivals.end() )
	{
		vector< pair<CFrame*, vector< CGeneralNode* > > > moreCurrentArrivals = ( *iMoreArrivals ).second;
		currentArrivals.insert(currentArrivals.end(), moreCurrentArrivals.begin(), moreCurrentArrivals.end());

		//从队列中移除
		frameArrivals.erase(iMoreArrivals);
	}

	bool rcv = false;
	//投递队列中的数据
	for( vector< pair<CFrame*, vector< CGeneralNode* > > >::iterator iArrival = currentArrivals.begin();
		iArrival != currentArrivals.end(); ++iArrival )
	{
		CFrame* currentFrame = iArrival->first;
		vector<CGeneralNode*> currentNeighbors = iArrival->second;
		for( vector<CGeneralNode*>::iterator ineighbor = currentNeighbors.begin(); ineighbor != currentNeighbors.end(); ++ineighbor )
		{
			bool new_rcv = receiveFrame(**ineighbor, currentFrame, now, findNeighbors, receivePackets);
			rcv = rcv || new_rcv;
		}

		free(currentFrame);
	}

	return rcv;
}

bool CMacProtocol::Prepare(int now)
{
	CSink::UpdateStatus(now);
	return true;
}

void CMacProtocol::PrintInfo(int now)
{
	PrintInfo(CNode::getAllNodes(), now);
}

void CMacProtocol::PrintInfo(vector<CNode*> allNodes, int now)
{
	if( ! ( now % getConfig<int>("log", "slot_log") == 0
			|| now == getConfig<int>("simulation", "runtime") ) )
		return;

	//节点相遇、数据传输、能耗
	if( now % getConfig<int>("log", "slot_log") == 0
	   || now == getConfig<int>("simulation", "runtime") )
	{
		//节点个数
		ofstream node(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_node"), ios::app);
		if( now == 0 )
		{
			node << endl << getConfig<string>("log", "info_log") << endl;
			node << getConfig<string>("log", "info_node") << endl;
		}
		int nAlive = 0;
		for( auto node : allNodes )
		{
			if( node->isAlive() )
				++nAlive;
		}
		node << now << TAB << nAlive << endl;
		node.close();

		//MA和节点 / 节点间的相遇次数
		ofstream encounter( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_encounter"), ios::app);
		if(now == 0)
		{
			encounter <<  endl << getConfig<string>("log", "info_log") << endl ;
			encounter << getConfig<string>("log", "info_encounter") << endl;
		}
		encounter << now << TAB;
		if( getConfig<CConfiguration::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc || getConfig<CConfiguration::EnumRoutingProtocolScheme>("simulation", "routing_protocol") == config::_xhar )
		{
			//encounter << CNode::getPercentEncounterActiveAtHotspot() << TAB << CNode::getEncounterActiveAtHotspot() << TAB;
			encounter << CHotspot::getPercentEncounterAtHotspot() << TAB << CHotspot::getEncounterAtHotspot() << TAB;
		}
		//encounter << CNode::getPercentEncounterActive() << TAB << CNode::getEncounterActive() << TAB;
		encounter << CNode::getEncounter() << TAB;
		encounter << endl;
		encounter.close();

		//sink和节点的相遇次数
		ofstream sink( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_sink"), ios::app);
		if(now == 0)
		{
			sink <<  endl << getConfig<string>("log", "info_log") << endl ;
			sink << getConfig<string>("log", "info_sink") << endl;
		}
		sink << now << TAB;
		sink << CSink::getPercentEncounterActive() << TAB << CSink::getEncounterActive() << TAB << CSink::getEncounter() << TAB;
		sink << endl;
		sink.close();

	}

	if( now % getConfig<int>("hs", "slot_hotspot_update") == 0
	   || now == getConfig<int>("simulation", "runtime") )
	{
		//数据传输
		ofstream transmit( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_transmit"), ios::app);
		if(now == 0)
		{
			transmit << endl << getConfig<string>("log", "info_log") << endl ;
			transmit << getConfig<string>("log", "info_transmit") << endl;
		}
		transmit << now << TAB << CMacProtocol::getPercentTransmitSuccessful() << TAB << CMacProtocol::getTransmitSuccessful() << TAB << CMacProtocol::getTransmit() << TAB;
		transmit << endl;
		transmit.close();

		//节点唤醒时间
		ofstream activation(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_activation"), ios::app);
		if( now == 0 )
		{
			activation << endl << getConfig<string>("log", "info_log") << endl;
			activation << getConfig<string>("log", "info_activation") << endl;
		}
		activation << now << TAB;
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
		if(now == 0)
		{
			energy_consumption <<  endl << getConfig<string>("log", "info_log") << endl ; 
			energy_consumption << getConfig<string>("log", "info_energy_consumption") << endl;
		}
		energy_consumption << now << TAB << CData::getAverageEnergyConsumption() ;
		if( CNode::finiteEnergy() )
		{			
			//节点剩余能量
			energy_consumption << TAB << CNode::getSumEnergyConsumption() << TAB << CNode::getAllNodes().size() << TAB;
			for(auto inode = allNodes.begin(); inode != allNodes.end(); ++inode)
				energy_consumption << (*inode)->getEnergyLeft() << TAB;
		}
		energy_consumption << endl;
		energy_consumption.close();

	}

}

void CMacProtocol::PrintFinal(int now)
{
}