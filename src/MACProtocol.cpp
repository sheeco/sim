#include "Node.h"
#include "Sink.h"
#include "MANode.h"
#include "MacProtocol.h"
#include "Prophet.h"
#include "HAR.h"
#include "HotspotSelect.h"

int CMacProtocol::transmitSuccessful = 0;
int CMacProtocol::transmit = 0;
bool CMacProtocol::RANDOM_STATE_INIT = false;
int CMacProtocol::SIZE_HEADER_MAC = 0;  //Mac Header Size
bool CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
int CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;  //动态节点个数测试时，节点个数发生变化的周期


CMacProtocol::CMacProtocol()
{
}

CMacProtocol::~CMacProtocol()
{
}

void CMacProtocol::receivePackage(CGeneralNode& gnode, CPackage* package, int currentTime) 
{
	// Make local copy
	package = new CPackage(*package);
	CGeneralNode* gFromNode = package->getSrcNode();
	CSink* sink = CSink::getSink();
	vector<CGeneralData*> contents = package->getContents();
	vector<CGeneralData*> contentsToSend;
	CPackage* packageToSend = nullptr;

	if( typeid(gnode) == typeid(CSink) )
	{

		/*********************************************** Sink <- MA *******************************************************/

		if( typeid(*gFromNode) == typeid(CMANode) )
		{
			CMANode* fromMA = dynamic_cast<CMANode*>( gFromNode );
			contentsToSend = HAR::receiveContents(sink, fromMA, contents, currentTime);
		}

		/*********************************************** Sink <- Node *******************************************************/

		else if( typeid(*gFromNode) == typeid(CNode) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( gFromNode );
			contentsToSend = CProphet::receiveContents(sink, fromNode, contents, currentTime);
		}

	}
	
	else if( typeid(gnode) == typeid(CMANode) )
	{
		CMANode* toMA = dynamic_cast<CMANode*>( &gnode );
		
		/************************************************ MA <- sink *******************************************************/

		if( typeid(*gFromNode) == typeid(CSink) )
		{
			contentsToSend = HAR::receiveContents(toMA, sink, contents, currentTime);
		}
		
		/************************************************ MA <- node *******************************************************/

		if( typeid(*gFromNode) == typeid(CNode) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( gFromNode );
			contentsToSend = HAR::receiveContents(toMA, fromNode, contents, currentTime);
		}
	}

	else if( typeid(gnode) == typeid(CNode) )
	{
		CNode* node = dynamic_cast<CNode*>( &gnode );

		/*********************************************** Node <- Sink *******************************************************/

		if( typeid(*gFromNode) == typeid(CSink) )
		{
			contentsToSend = CProphet::receiveContents(node, sink, contents, currentTime);
		}

		/************************************************ Node <- MA *******************************************************/

		else if( typeid(*gFromNode) == typeid(CMANode) )
		{
			CMANode* fromMA = dynamic_cast<CMANode*>( gFromNode );
			contentsToSend = HAR::receiveContents(node, fromMA, contents, currentTime);
		}

		/*********************************************** Node <- Node *******************************************************/

		else if( typeid(*gFromNode) == typeid(CNode) )
		{
			CNode* fromNode = dynamic_cast<CNode*>( gFromNode );
			contentsToSend = CProphet::receiveContents(node, fromNode, contents, currentTime);
		}

	}

	/*********************************************** send reply *******************************************************/

	if( ! contentsToSend.empty() )
	{
		packageToSend = new CPackage(gnode, *gFromNode, contentsToSend);
		transmitPackage( gnode, gFromNode, packageToSend, currentTime );			
	}
	else
	{
		CMacProtocol::transmitSucceed();
	}

}

void CMacProtocol::broadcastPackage(CGeneralNode& src, CPackage* package, int currentTime)
{
	bool rcv = false;
	vector<CNode*> nodes = CNode::getNodes();
	if( package->getDstNode() != nullptr )
	{
		cout << "Error @ CMacProtocol::broadcastPackage() : package is not for broadcast" << endl;
		_PAUSE_;
	}

	src.consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_SEND);

	/************************************************ Sensor Node *******************************************************/

	if( typeid(src) == typeid(CNode) )
	{

		CNode* srcNode = dynamic_cast<CNode*>( &src );	
		for(vector<CNode*>::iterator dstNode = nodes.begin(); dstNode != nodes.end(); ++dstNode)
		{
			//skip itself
			if( (*dstNode)->getID() == srcNode->getID() )
				continue;

			if( CBasicEntity::withinRange( *srcNode, **dstNode, CGeneralNode::RANGE_TRANS ) )
			{
				CMacProtocol::transmitTry();

				if( Bet(CGeneralNode::PROB_TRANS) )
				{
					(*dstNode)->consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECEIVE);
					receivePackage(**dstNode, package, currentTime);
				}
			}
		}
	}

	/*************************************************** Sink **********************************************************/

	else if( typeid(src) == typeid(CSink) )
	{
		// Prophet: sink => nodes
		if( ROUTING_PROTOCOL == _prophet )
		{
			CSink* sink = CSink::getSink();	
			for(vector<CNode*>::iterator dstNode = nodes.begin(); dstNode != nodes.end(); ++dstNode)
			{
				if( CBasicEntity::withinRange( *sink, **dstNode, CGeneralNode::RANGE_TRANS ) )
				{
					CSink::encount();
					CMacProtocol::transmitTry();

					if( (*dstNode)->isListening() )
					{
						CSink::encountActive();

						if( Bet(CGeneralNode::PROB_TRANS) )
						{
							(*dstNode)->consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECEIVE);
							receivePackage(**dstNode, package, currentTime);
						}
					}
				}
			}
		}

		// xHAR: sink => MAs
		else if( ROUTING_PROTOCOL == _xhar )
		{
			CSink* sink = CSink::getSink();	
			vector<CMANode*> MAs = CMANode::getMANodes();
			for(vector<CMANode*>::iterator iMA = MAs.begin(); iMA != MAs.end(); ++iMA)
			{
				if( CBasicEntity::withinRange( *sink, **iMA, CGeneralNode::RANGE_TRANS ) )
				{
					CMacProtocol::transmitTry();

					if( (*iMA)->isListening() )
					{
						if( Bet(CGeneralNode::PROB_TRANS) )
						{
							(*iMA)->consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECEIVE);
							receivePackage(**iMA, package, currentTime);
						}
					}
				}
			}
		}
		
	}

	/**************************************************** MA **********************************************************/

	// xHAR: ma => nodes
	else if( typeid(src) == typeid(CMANode) )
	{

		CMANode* ma = dynamic_cast<CMANode*>( &src );
		for(vector<CNode*>::iterator dstNode = nodes.begin(); dstNode != nodes.end(); ++dstNode)
		{
			if( CBasicEntity::withinRange( *ma, **dstNode, CGeneralNode::RANGE_TRANS ) )
			{
				CMANode::encount();
				CMacProtocol::transmitTry();

				if( (*dstNode)->isListening() )
				{
					if( Bet(CGeneralNode::PROB_TRANS) )
					{
						(*dstNode)->consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECEIVE);
						receivePackage(**dstNode, package, currentTime);
					}
				}
			}
		}

	}
	
	free(package);

	// TODO: sort by distance with src node ?
}

bool CMacProtocol::transmitPackage(CGeneralNode& src, CGeneralNode* dst, CPackage* package, int currentTime)
{
	src.consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_SEND);
	CMacProtocol::transmitTry();

	if( dst->isListening() )
	{
		if( Bet(CGeneralNode::PROB_TRANS) )
		{
			dst->consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECEIVE);
			receivePackage(*dst, package, currentTime);
			return true;
		}
		else
			flash_cout << "####  ( Node " << NDigitString( package->getSrcNode()->getID(), 2) << "  >----  fail  xxxx>  Node " << NDigitString( dst->getID(), 2) << " )     " ;
	}
	free(package);
	
	return false;
}

void CMacProtocol::ChangeNodeNumber(int currentTime)
{
	if( ! ( currentTime % SLOT_CHANGE_NUM_NODE == 0 ) )
		return;

	flash_cout << endl << "########  < " << currentTime << " >  NODE NUMBER CHANGE" ;
	
	int delta = CNode::ChangeNodeNumber();

	if(delta >= 0)
	{
		cout << "####  ( " << delta << " nodes added )" << endl;
	}
	else
	{
		cout << "####  ( " << -delta << " nodes removed )" << endl;
	}
}

bool CMacProtocol::UpdateNodeStatus(int currentTime)
{
	bool death = false;
	vector<CNode *> nodes = CNode::getNodes();
	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
		death |= (*inode)->updateStatus(currentTime);
	if( death )
		CNode::ClearDeadNodes();

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
		flash_cout << "########  < " << currentTime << " >  DATA DELIVERY            " << endl ;
		print = false;
	}

	// TODO: sink receive RTS / send by slot ?
	// Prophet: sink => nodes
	// xHAR: sink => MAs
	broadcastPackage( *CSink::getSink(), CSink::getSink()->sendRTS(currentTime) , currentTime );

	vector<CNode*> nodes = CNode::getNodes();
	vector<CMANode*> MAs = CMANode::getMANodes();

	switch( ROUTING_PROTOCOL )
	{
	case _prophet:

		for(vector<CNode*>::iterator srcNode = nodes.begin(); srcNode != nodes.end(); ++srcNode )
		{
			if( (*srcNode)->isDiscovering() )
			{
				broadcastPackage( **srcNode, (*srcNode)->sendRTSWithCapacityAndPred(currentTime), currentTime );
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
				broadcastPackage( **srcMA, (*srcMA)->sendRTSWithCapacity(currentTime) , currentTime );
		}
		// xHAR: no forward between nodes

		break;

	default:
		break;
				
	}

	if( ( currentTime + SLOT ) % SLOT_LOG == 0 )
	{
		double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
		flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                             " << endl << endl;
		print = true;
	}
}

void CMacProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_LOG == 0
			|| currentTime == RUNTIME ) )
		return;

	//节点相遇、数据传输、能耗
	if( currentTime % CHotspotSelect::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
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