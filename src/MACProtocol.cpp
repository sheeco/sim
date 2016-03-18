#include "Node.h"
#include "MacProtocol.h"
#include "Sink.h"
#include "Prophet.h"

int CMacProtocol::MAC_SIZE = 0;  //Mac Header Size
bool CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
int CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //动态节点个数测试时，节点个数发生变化的周期


//CMacProtocol::CMacProtocol()
//{
//}
//
//CMacProtocol::~CMacProtocol()
//{
//}

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
	if( ! ( currentTime % SLOT_MOBILITYMODEL == 0 ) )
		return;

	//cout << endl << "########  < " << currentTime << " >  NODE LOCATION UPDATE" ;
	bool allAlive = true;
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
		allAlive &= (*inode)->updateStatus(currentTime);
	if( ! allAlive )
		CNode::ClearDeadNodes();
}

void CMacProtocol::receivePackage(CGeneralNode& gnode, CPackage* package, int currentTime) 
{
	gnode.consumeEnergy( package->getSize() * CGeneralNode::CONSUMPTION_BYTE_RECIEVE );

	vector<CGeneralData*> contents = package->getContent();
	CGeneralNode* dst = package->getSourceNode();

	if( typeid(gnode) == typeid(CNode) )
	{
		CNode* node = dynamic_cast<CNode*>(&gnode);
		CCtrl* ctrlToSend = nullptr;
		CCtrl* ctrlPiggback = nullptr;
		vector<CData> dataToSend;

		for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); )
		{
			if( typeid(**icontent) == typeid(CCtrl) )
			{
				CCtrl* ctrl = dynamic_cast<CCtrl*>(*icontent);
				switch( ctrl->getType() )
				{
					case CCtrl::_rts:
						//CTS
						ctrlToSend = new CCtrl(node->getID(), currentTime, CGeneralNode::CTRL_SIZE, CCtrl::_cts);

						//if RTS is from sink, send CTS & datas
						if( ctrl->getNode() == CSink::SINK_ID )
						{
							dataToSend = node->getAllData();
						}
						//skip if has spoken recently
						else if( node->hasSpokenRecently(dynamic_cast<CNode*>(dst), currentTime) )
						{
							return;
						}
						else
						{
							//piggyback with data index otherwise
							ctrlPiggback = new CCtrl(node->getID(), node->getDeliveryPreds(), node->updateSummaryVector(), currentTime, CGeneralNode::CTRL_SIZE, CCtrl::_index);
						}

						// TODO: mark skipRTS ?
						// TODO: connection established ?
						break;

					case CCtrl::_cts:
						//data index
						ctrlToSend = new CCtrl(node->getID(), node->getDeliveryPreds(), node->updateSummaryVector(), currentTime, CGeneralNode::CTRL_SIZE, CCtrl::_index);

						// TODO: connection established ?
						break;
			
					case CCtrl::_index:
						//update preds
						node->updateDeliveryPredsWith( dst->getID(), ctrl->getPred() );

						dataToSend = CProphet::selectDataByIndex(node, ctrl);
						break;
			
					case CCtrl::_ack:
						node->addToSpokenCache( (CNode*)(&dst), currentTime );
						//clear data with ack
						node->checkDataByAck( ctrl->getACK() );
						break;

					default:
						break;
				}

				++icontent;
			}
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
					ctrlToSend = new CCtrl(node->getID(), ack, currentTime, CGeneralNode::CTRL_SIZE, CCtrl::_ack);
			}
		}

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
					ctrlToSend = new CCtrl(CSink::SINK_ID, ack, currentTime, CGeneralNode::CTRL_SIZE, CCtrl::_ack);
			}
		}

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
//	vector<CNode*> rcvNodes;
	bool rcv = false;
	vector<CNode*> nodes = CNode::getNodes();
	for(vector<CNode*>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
	{
		if( (*inode)->getID() == package->getSourceNode()->getID() )
			continue;
		if( (*inode)->isListening() 
			&& CBasicEntity::getDistance( *package->getSourceNode(), **inode ) <= CGeneralNode::TRANS_RANGE
			&& Bet(CGeneralNode::PROB_DATA_FORWARD) )
		{
//			rcvNodes.push_back(*inode);
			rcv = true;
			receivePackage(**inode, package, currentTime);
		}
		else
			free(package);
	}

	// TODO: sort by distance with src node ?
//	return rcvNodes;
	return rcv;
}

// TODO: 
bool CMacProtocol::transmitPackage(CPackage* package, CGeneralNode* dst, int currentTime)
{
	if( dst->isListening() 
		&& Bet(CGeneralNode::PROB_DATA_FORWARD) )
	{
		receivePackage(*dst, package, currentTime);
		return true;
	}
	else
		free(package);
	return false;
}

void CMacProtocol::PrintInfo(int currentTime)
{
	if( ! ( currentTime % SLOT_RECORD_INFO == 0
			|| currentTime == RUNTIME ) )
		return;

	//投递率、延迟、Energy Consumption、节点buffer状态统计 ...
	if( currentTime % CHotspot::SLOT_HOTSPOT_UPDATE  == 0
		|| currentTime == RUNTIME )
	{
		//MA和节点 / 节点间的相遇次数
		ofstream encounter("encounter.txt", ios::app);
		if(currentTime == 0)
		{
			encounter << INFO_LOG ;
			encounter << INFO_ENCOUNTER ;
		}
		encounter << currentTime << TAB;
		if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
			encounter << CNode::getEncounterAtHotspotPercent() << TAB << CNode::getEncounterAtHotspot() << TAB 
					  << CNode::getEncounterActivePercent() << TAB << CNode::getEncounterActive << TAB;
		encounter << CNode::getEncounter() << TAB;
		encounter << endl;
		encounter.close();

		//平均能耗
		ofstream energy_consumption("energy-consumption.txt", ios::app);
		if(currentTime == 0)
		{
			energy_consumption << INFO_LOG ; 
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
	ofstream debug(FILE_DEBUG, ios::app);
	debug << CData::getAverageEnergyConsumption() << TAB << CNode::getEncounterActivePercent() << TAB ;
	if( MAC_PROTOCOL == _hdc )
	{
		debug << CNode::getEncounterAtHotspotPercent() << TAB ;
	}
	if( CNode::finiteEnergy() )
		debug << currentTime << TAB << CNode::getNodes().size() << TAB ;

	debug.close();

}