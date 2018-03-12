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

	vector<CNode*> nodes = CNode::getAliveNodes();
	for( vector<CNode*>::iterator idstNode = nodes.begin(); idstNode != nodes.end(); ++idstNode )
	{
		CNode* dstNode = *idstNode;
		//skip itself
		if( ( dstNode )->getID() == src.getID() )
			continue;

		if( CBasicEntity::withinRange(src, *dstNode, getConfig<int>("trans", "range_trans")) )
		{
			//ͳ��sink�ڵ����������
			if( typeid( src ) == typeid( CSink ) )
				CSink::encount();

			if( dstNode->isAwake() )
			{
				//ͳ��sink�ڵ����������
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

	//�ǹ㲥��Ŀ��ڵ�Ǳ��ڵ㣬������
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
	//ѹ�뽫���ĵ������
	if( timeArrival > now )
	{
		vector< pair<CFrame*, vector< CGeneralNode* > > > futureArrivals;
		map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > >::iterator iArrivals = frameArrivals.find(timeArrival);
		if( iArrivals != frameArrivals.end() )
			futureArrivals = ( *iArrivals ).second;

		futureArrivals.push_back(pair<CFrame*, vector< CGeneralNode* > >(frame, neighbors));
		frameArrivals[timeArrival] = futureArrivals;
	}
	//ѹ�뵱ǰ�ĵ������
	else
		currentArrivals.push_back(pair<CFrame*, vector< CGeneralNode* > >(frame, neighbors));

	//ȡ��֮ǰ����ĵ�ǰʱ��ĵ������
	map<int, vector< pair<CFrame*, vector< CGeneralNode* > > > >::iterator iMoreArrivals = frameArrivals.find(now);
	if( iMoreArrivals != frameArrivals.end() )
	{
		vector< pair<CFrame*, vector< CGeneralNode* > > > moreCurrentArrivals = ( *iMoreArrivals ).second;
		currentArrivals.insert(currentArrivals.end(), moreCurrentArrivals.begin(), moreCurrentArrivals.end());

		//�Ӷ������Ƴ�
		frameArrivals.erase(iMoreArrivals);
	}

	bool rcv = false;
	//Ͷ�ݶ����е�����
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

	//�ڵ����������ݴ��䡢�ܺ�
	if( now % getConfig<int>("log", "slot_log") == 0
	   || now == getConfig<int>("simulation", "runtime") )
	{
		//�ڵ����
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

		//MA�ͽڵ� / �ڵ�����������
		ofstream encounter( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_encounter"), ios::app);
		if(now == 0)
		{
			encounter <<  endl << getConfig<string>("log", "info_log") << endl ;
			encounter << getConfig<string>("log", "info_encounter") << endl;
		}
		encounter << now << TAB;
		if( getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc || getConfig<config::EnumRoutingProtocolScheme>("simulation", "routing_protocol") == config::_xhar )
		{
			//encounter << CNode::getPercentEncounterActiveAtHotspot() << TAB << CNode::getEncounterActiveAtHotspot() << TAB;
			encounter << CHotspot::getPercentEncounterAtHotspot() << TAB << CHotspot::getEncounterAtHotspot() << TAB;
		}
		//encounter << CNode::getPercentEncounterActive() << TAB << CNode::getEncounterActive() << TAB;
		encounter << CNode::getEncounter() << TAB;
		encounter << endl;
		encounter.close();

		//sink�ͽڵ����������
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
		//���ݴ���
		ofstream transmit( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_transmit"), ios::app);
		if(now == 0)
		{
			transmit << endl << getConfig<string>("log", "info_log") << endl ;
			transmit << getConfig<string>("log", "info_transmit") << endl;
		}
		transmit << now << TAB << CMacProtocol::getPercentTransmitSuccessful() << TAB << CMacProtocol::getTransmitSuccessful() << TAB << CMacProtocol::getTransmit() << TAB;
		transmit << endl;
		transmit.close();

		//�ڵ㻽��ʱ��
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

		//ƽ���ܺ�
		ofstream energy_consumption( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_energy_consumption"), ios::app);
		if(now == 0)
		{
			energy_consumption <<  endl << getConfig<string>("log", "info_log") << endl ; 
			energy_consumption << getConfig<string>("log", "info_energy_consumption") << endl;
		}
		energy_consumption << now << TAB << CData::getAverageEnergyConsumption() ;
		if( CNode::finiteEnergy() )
		{			
			//�ڵ�ʣ������
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