#include "Node.h"
#include "MacProtocol.h"
#include "Sink.h"
#include "Prophet.h"

bool CMacProtocol::RANDOM_STATE_INIT = false;
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
		CCtrl* indexToSend = nullptr;
		CCtrl* nodataToSend = nullptr;
		vector<CData> dataToSend;  //空vector代表没有适合传输的数据

		for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); )
		{
			
			/***************************************** rcv Ctrl Message *****************************************/

			//如果被告知没有需要传输的数据，则不发送ACK，但也认为传输成功
//			if( *icontent == nullptr )
//			{
//				CNode::transmitSucceed();
//				++icontent;
//			}
//			else 
			if( typeid(**icontent) == typeid(CCtrl) )
			{
				CCtrl* ctrl = dynamic_cast<CCtrl*>(*icontent);
				switch( ctrl->getType() )
				{

					/*************************************** rcv RTS **************************************/

					case CCtrl::_rts:

						//收到RTS，就认为开始一次数据传输尝试
						CNode::transmitTry();

						//if RTS is from sink, send CTS & datas
						if( ctrl->getNode() == CSink::SINK_ID )
						{
							node->updateDeliveryPredsWithSink();

							if( gnode.getAllData().empty() )
							{
								//没有数据需要向Sink传输，也认为数据传输成功
								CNode::transmitSucceed();

								return;
							}
							//CTS
							ctrlToSend = new CCtrl(node->getID(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_cts);
							// + DATA
							dataToSend = node->getAllData();
						}
						//skip if has spoken recently
						else if( node->hasSpokenRecently(dynamic_cast<CNode*>(dst), currentTime) )
						{
							//跳过传输，也认为数据传输成功
							CNode::transmitSucceed();
							flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  ----- skip -----  Node " << NDigitString(dst->getID(), 2) << " )                " ;
							return;
						}
						//rcv RTS from node
						else
						{
							//进行数据传输，就加入最近邻居列表
							node->addToSpokenCache( (CNode*)(&dst), currentTime );

							//CTS
							ctrlToSend = new CCtrl(node->getID(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_cts);

							// + Index
							indexToSend = new CCtrl(node->getID(), node->getDeliveryPreds(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_index);
//							node->skipRTS();  //（暂未使用）
						}

						// TODO: mark skipRTS ?
						// TODO: connection established ?
						break;

					/*************************************** rcv CTS **************************************/

					case CCtrl::_cts:

						// TODO: connection established ?
						break;
			
					/****************************** rcv Data Index ( dp / sv ) ****************************/

					case CCtrl::_index:

						//update preds
						node->updateDeliveryPredsWith( dst->getID(), ctrl->getPred() );
						// + DATA / NODATA
						if( CProphet::shouldForward(node, ctrl->getPred() ) )
							dataToSend = CProphet::getDataForTrans(node);
						else
							nodataToSend = new CCtrl(node->getID(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_no_data);

						break;
			
					/*************************************** rcv ACK **************************************/

					case CCtrl::_ack:
						
						//收到ACK，认为数据传输成功
						CNode::transmitSucceed();

						//收到空的ACK时，结束本次数据传输
						if( ctrl->getACK().empty() )
							return;
						//clear data with ack
						else
							node->checkDataByAck( ctrl->getACK() );

						if( dst->getID() == CSink::SINK_ID )
							flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Sink )       " ;
						else
							flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Node " << NDigitString(dst->getID(), 2) << " )                       " ;

						return;

						break;

					case CCtrl::_no_data:
						
						//收到NODATA，也认为数据传输成功
						CNode::transmitSucceed();

						//空的ACK
						ctrlToSend = new CCtrl(node->getID(), vector<CData>(), currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
						//收到NODATA时，不回复数据
						dataToSend.clear();

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
				//ACK（如果收到的数据全部被丢弃，发送空的ACK）
				ctrlToSend = new CCtrl(node->getID(), ack, currentTime, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
			}
		}

		/********************************** snd ***********************************/

		CPackage* packageToSend = nullptr;
		vector<CCtrl> ctrlsToSend;

		if( ctrlToSend != nullptr )
		{
			ctrlsToSend.push_back(*ctrlToSend);
		}
		if( indexToSend != nullptr )
		{
			ctrlsToSend.push_back(*indexToSend);
		}
		if( nodataToSend != nullptr )
		{
			ctrlsToSend.push_back(*nodataToSend);
		}

		if( dataToSend.empty() )
		{
			switch( ctrlsToSend.size() )
			{
			case 1:
				packageToSend = new CPackage(*node, *dst, ctrlsToSend[0]);
				break;

			case 2:
				packageToSend = new CPackage(*node, *dst, ctrlsToSend[0], ctrlsToSend[1]);
				break;

			case 3:
				packageToSend = new CPackage(*node, *dst, ctrlsToSend[0], ctrlsToSend[1], ctrlsToSend[2]);
				break;

			default:
				break;
			}
		}
		else
		{
			switch( ctrlsToSend.size() )
			{
			case 0:
				packageToSend = new CPackage(*node, *dst, dataToSend);
				break;

			case 1:
				packageToSend = new CPackage(*node, *dst, ctrlsToSend[0], dataToSend);
				break;

			case 2:
				packageToSend = new CPackage(*node, *dst, ctrlsToSend[0], ctrlsToSend[1], dataToSend);
				break;

			default:
				break;
			}
		}

		node->consumeEnergy( packageToSend->getSize() * CGeneralNode::CONSUMPTION_BYTE_SEND );
		transmitPackage( packageToSend, dst, currentTime );

		if( ctrlToSend != nullptr )
		{
			free(ctrlToSend);
			ctrlToSend = nullptr;
		}
		if( indexToSend != nullptr )
		{
			free(indexToSend);
			indexToSend = nullptr;
		}
		if( nodataToSend != nullptr )
		{
			free(nodataToSend);
			nodataToSend = nullptr;
		}

		
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

				//ACK（如果收到的数据全部被丢弃，发送空的ACK）
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
			free(ctrlToSend);
			transmitPackage( packageToSend, dst, currentTime );
		}
	}
	
	free(package);
	package = nullptr;

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
		for(vector<CNode*>::iterator dstNode = nodes.begin(); dstNode != nodes.end(); ++dstNode)
		{
			//skip itself
			if( (*dstNode)->getID() == srcNode->getID() )
				continue;

			if( CBasicEntity::getDistance( *srcNode, **dstNode ) <= CGeneralNode::RANGE_TRANS )
			{
				if( (*dstNode)->isListening() )
				{
					if( ! (*dstNode)->isDiscovering() )  //避免重复计算
					{
						CNode::encountActive();
						if( (*dstNode)->isAtHotspot() || srcNode->isAtHotspot() )
							CNode::encountActiveAtHotspot();
					}

					if( Bet(CGeneralNode::PROB_TRANS) )
					{
						rcv = true;
						receivePackage(**dstNode, package, currentTime);
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

					if( Bet(CGeneralNode::PROB_TRANS) )
					{
						rcv = true;
						receivePackage(**inode, package, currentTime);
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
		if( Bet(CGeneralNode::PROB_TRANS) )
		{
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
		flash_cout << "########  < " << currentTime << " >  DATA DELIVERY            " << endl ;
		print = false;
	}

	UpdateNodeStatus(currentTime);

	// TODO: sink receive RTS / send by slot ?
	broadcastPackage( CSink::getSink()->sendRTS(currentTime) , currentTime );

	vector<CNode*> nodes = CNode::getNodes();
	for(vector<CNode*>::iterator srcNode = nodes.begin(); srcNode != nodes.end(); ++srcNode )
	{
		for(vector<CNode*>::iterator dstNode = srcNode; dstNode != nodes.end(); ++dstNode)
		{
			if( CBasicEntity::getDistance( **srcNode, **dstNode ) <= CGeneralNode::RANGE_TRANS )
			{
				CNode::encount();
				if( (*dstNode)->isAtHotspot() || (*srcNode)->isAtHotspot() )  //避免重复计算
					CNode::encountAtHotspot();
			}
		}

		if( (*srcNode)->isDiscovering() )
		{
			broadcastPackage( (*srcNode)->sendRTS(currentTime), currentTime );
			(*srcNode)->finishDiscovering();
		}
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
		{
			encounter << CNode::getPercentEncounterActiveAtHotspot() << TAB << CNode::getEncounterActiveAtHotspot() << TAB;
			encounter << CNode::getPercentEncounterAtHotspot() << TAB << CNode::getEncounterAtHotspot() << TAB;
		}
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
		final << CNode::getPercentEncounterActiveAtHotspot() << TAB ;
	}
	if( CNode::finiteEnergy() )
		final << currentTime << TAB << CNode::getNodes().size() << TAB ;

	final.close();

}