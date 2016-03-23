#include "Node.h"
#include "MacProtocol.h"
#include "Sink.h"
#include "Prophet.h"

int CMacProtocol::SIZE_HEADER_MAC = 0;  //Mac Header Size
bool CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
int CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //动态节点个数测试时，节点个数发生变化的周期


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
	gnode.consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECIEVE );

	vector<CGeneralData*> contents = package->getContent();
	CGeneralNode* dst = package->getSrcNode();


	/*********************************************** Sensor Node *******************************************************/

	if( typeid(gnode) == typeid(CNode) )
	{
		CNode* node = dynamic_cast<CNode*>(&gnode);
		CCtrl* ctrlToSend = nullptr;
		CCtrl* ctrlPiggback = nullptr;
		vector<CData> dataToSend;

		for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); )
		{
			
			/***************************************** rcv Ctrl Message *****************************************/

			if( typeid(**icontent) == typeid(CCtrl) )
			{
				CCtrl* ctrl = dynamic_cast<CCtrl*>(*icontent);
				switch( ctrl->getType() )
				{

					/*************************************** rcv RTS **************************************/

					case CCtrl::_rts:
						//CTS
						ctrlToSend = new CCtrl(node->getID(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_cts);

						//if RTS is from sink, send CTS & datas
						if( ctrl->getNode() == CSink::SINK_ID )
						{
							if( gnode.getAllData().empty() )
								return;

							dataToSend = node->getAllData();
						}
						//skip if has spoken recently
						else if( node->hasSpokenRecently(dynamic_cast<CNode*>(dst), currentTime) )
						{
							flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  ----- skip -----  Node " << NDigitString(dst->getID(), 2) << " )                " ;
							return;
						}
						else
						{
							//piggyback with data index otherwise
							ctrlPiggback = new CCtrl(node->getID(), node->getDeliveryPreds(), node->updateSummaryVector(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_index);
							node->skipRTS();  //（暂未使用）
						}

						// TODO: mark skipRTS ?
						// TODO: connection established ?
						break;

					/*************************************** rcv CTS **************************************/

					case CCtrl::_cts:
						//data index
						ctrlToSend = new CCtrl(node->getID(), node->getDeliveryPreds(), node->updateSummaryVector(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_index);

						// TODO: connection established ?
						break;
			
					/****************************** rcv Data Index ( dp + sv ) ****************************/

					case CCtrl::_index:
						//update preds
						node->updateDeliveryPredsWith( dst->getID(), ctrl->getPred() );

						dataToSend = CProphet::selectDataByIndex(node, ctrl);
						break;
			
					/*************************************** rcv ACK **************************************/

					case CCtrl::_ack:
						node->addToSpokenCache( (CNode*)(&dst), currentTime );
						//clear data with ack
						node->checkDataByAck( ctrl->getACK() );

						if( dst->getID() == CSink::SINK_ID )
							flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  >----" << NDigitString( ctrl->getACK().size(), 3, ' ') << " ---->  Sink )       " ;
						else
							flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  >----" << NDigitString( ctrl->getACK().size(), 3, ' ') << " ---->  Node " << NDigitString(dst->getID(), 2) << "   )                       " ;

						break;

					default:
						break;
				}
				++icontent;
			}

			/******************************************* rcv Data *******************************************/

			else if( typeid(**icontent) == typeid(CData) )
			{
				//extract data content
				vector<CData> datas;
				do
				{
					datas.push_back( *dynamic_cast<CData*>(*icontent) );
					++icontent;
				} while( icontent != contents.end() );
			
				//accept data into buffer
				vector<CData> ack;
				ack = CProphet::bufferData(node, datas, currentTime);
				//ACK
				if( ! ack.empty() )
					ctrlToSend = new CCtrl(node->getID(), ack, currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
			}
		}

		/********************************** snd ***********************************/

		CPackage* packageToSend = nullptr;
		if( ctrlToSend != nullptr )
		{
			if( ctrlPiggback != nullptr )
			{
				packageToSend = new CPackage(*node, *dst, *ctrlToSend, *ctrlPiggback);		
				free(ctrlPiggback);
			}
			else if( ! dataToSend.empty() )
			{
				packageToSend = new CPackage(*node, *dst, *ctrlToSend, dataToSend);		
			}
			else
			{
				packageToSend = new CPackage(*node, *dst, *ctrlToSend);		
			}
		
			free(ctrlToSend);
		}
		else
		{
			// TODO: connection closed ?
			return;
		}
		node->consumeEnergy( packageToSend->getSize() * CGeneralNode::CONSUMPTION_BYTE_SEND );
		transmitPackage( packageToSend, dst, currentTime );
		
	}

	/*************************************************** Sink **********************************************************/

	else if( typeid(gnode) == typeid(CSink) )
	{
		CCtrl* ctrlToSend = nullptr;

		for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); )
		{
			if( typeid(**icontent) == typeid(CData) )
			{
				//extract data content
				vector<CData> datas;
				do
				{
					datas.push_back( *dynamic_cast<CData*>(*icontent) );
					++icontent;
				} while( icontent != contents.end() );
			
				//accept data into buffer
				vector<CData> ack = CSink::bufferData(currentTime, datas);

				//ACK
				if( ! ack.empty() )
					ctrlToSend = new CCtrl(CSink::SINK_ID, ack, currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
			}
			else
				++icontent;
		}

		/********************************** snd ***********************************/

		CPackage* packageToSend = nullptr;
		if( ctrlToSend != nullptr )
		{
			packageToSend = new CPackage(*CSink::getSink(), *dst, *ctrlToSend);		
			transmitPackage( packageToSend, dst, currentTime );
		}
	}
	
	free(package);

}

bool CMacProtocol::broadcastPackage(CPackage* package, int currentTime)
{
	bool rcv = false;
	CGeneralNode* gnode = package->getSrcNode();
	vector<CNode*> nodes = CNode::getNodes();
	if( package->getDstNode() != nullptr )
	{
		cout << "Error @ CMacProtocol::broadcastPackage() : package is not for broadcast" << endl;
		_PAUSE_;
	}

	/************************************************ Sensor Node *******************************************************/

	if( typeid(*gnode) == typeid(CNode) )
	{

		CNode* srcNode = dynamic_cast<CNode*>( gnode );	
		for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
		{
			//skip itself
			if( (*inode)->getID() == srcNode->getID() )
				continue;

			if( CBasicEntity::getDistance( *srcNode, **inode ) <= CGeneralNode::RANGE_TRANS )
			{
				CNode::encount();

				if( (*inode)->isAtHotspot() || srcNode->isAtHotspot() )
					CNode::encountAtHotspot();

				if( (*inode)->isListening() )
				{
					CNode::encountActive();

					CNode::transmitTry();
					if( Bet(CGeneralNode::PROB_TRANS) )
					{
						rcv = true;
						receivePackage(**inode, package, currentTime);
						CNode::transmitSucceed();
					}
				}
			}
		}
	}

	/*************************************************** Sink **********************************************************/

	else if( typeid(*gnode) == typeid(CSink) )
	{

		CSink* sink = CSink::getSink();	
		for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
		{
			if( CBasicEntity::getDistance( *package->getSrcNode(), **inode ) <= CGeneralNode::RANGE_TRANS )
			{
				CSink::encount();

				if( (*inode)->isListening() )
				{
					CSink::encountActive();

					CNode::transmitTry();
					if( Bet(CGeneralNode::PROB_TRANS) )
					{
						rcv = true;
						receivePackage(**inode, package, currentTime);
						CNode::transmitSucceed();
					}
				}
			}
		}

		
	}
	
	free(package);

	// TODO: sort by distance with src node ?
	return rcv;
}

// TODO: add flash_cout & delivery% print
bool CMacProtocol::transmitPackage(CPackage* package, CGeneralNode* dst, int currentTime)
{
	if( dst->isListening() )
	{
		CNode::transmitTry();

		if( Bet(CGeneralNode::PROB_TRANS) )
		{
			receivePackage(*dst, package, currentTime);
			CNode::transmitSucceed();
			return true;
		}
		else
			flash_cout << "####  ( Node " << NDigitString( package->getSrcNode()->getID(), 2) << "  xxxxx fail xxxxx  Node " << NDigitString( dst->getID(), 2) << " )     " ;
	}
	free(package);
	
	return false;
}

void CMacProtocol::ChangeNodeNumber(int currentTime)
{
	if( ! ( currentTime % SLOT_CHANGE_NUM_NODE == 0 ) )
		return;

	cout << endl << "########  < " << currentTime << " >  NODE NUMBER CHANGE" ;
	
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

void CMacProtocol::UpdateNodeStatus(int currentTime)
{
	//cout << endl << "########  < " << currentTime << " >  NODE LOCATION UPDATE" ;

	bool death = false;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		death |= (*inode)->updateStatus(currentTime);
	if( death )
		CNode::ClearDeadNodes();
}

void CMacProtocol::TransmitData(int currentTime)
{
	static bool print = false;
	if( currentTime == 0 
		|| print )
	{
		cout << endl << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;
		print = false;
	}

	UpdateNodeStatus(currentTime);

	// TODO: sink receive RTS / send by slot ?
	broadcastPackage( CSink::getSink()->sendRTS(currentTime) , currentTime );

	vector<CNode*> nodes = CNode::getNodes();
	for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
	{
		if( (*inode)->isDiscovering() )
			broadcastPackage( (*inode)->sendRTS(currentTime), currentTime );
	}

	if( ( currentTime + SLOT ) % SLOT_LOG == 0 )
	{
		double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
		flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                             " << endl;
		print = true;
	}
}

void CMacProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_LOG == 0
			|| currentTime == RUNTIME ) )
		return;

	//节点相遇、数据传输、能耗
	if( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE  == 0
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
		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
			encounter << CNode::getPercentEncounterAtHotspot() << TAB << CNode::getEncounterAtHotspot() << TAB;
		encounter << CNode::getPercentEncounterActive() << TAB << CNode::getEncounterActive() << TAB << CNode::getEncounter() << TAB;
		encounter << endl;
		encounter.close();

		//数据传输
		ofstream transmit( PATH_ROOT + PATH_LOG + FILE_TRANSMIT, ios::app);
		if(currentTime == 0)
		{
			transmit << endl << INFO_LOG << endl ;
			transmit << INFO_TRANSMIT ;
		}
		transmit << currentTime << TAB << CNode::getPercentTransmitSuccessful() << TAB << CNode::getTransmitSuccessful() << TAB << CNode::getTransmit() << TAB;
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
	final << CData::getAverageEnergyConsumption() << TAB << CNode::getPercentTransmitSuccessful() << TAB << CNode::getPercentEncounterActive() << TAB ;
	if( MAC_PROTOCOL == _hdc )
	{
		final << CNode::getPercentEncounterAtHotspot() << TAB ;
	}
	if( CNode::finiteEnergy() )
		final << currentTime << TAB << CNode::getNodes().size() << TAB ;

	final.close();

}