#include "Global.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "Prophet.h"
#include "SortHelper.h"

bool CProphet::TRANS_STRICT_BY_PRED = true;
int CProphet::CAPACITY_FORWARD = 0;

#ifdef USE_PRED_TOLERANCE

double CProphet::TOLERANCE_PRED = 0;
//double CProphet::DECAY_TOLERANCE_PRED = 1;

#endif


CProphet::CProphet()
{
	
}

CProphet::~CProphet()
{
	
}

bool CProphet::shouldForward(CNode* node, map<int, double> dstPred)
{
	double predNode = node->getDeliveryPreds().find(CSink::getSink()->getID())->second;
	double predDst = dstPred.find(CSink::getSink()->getID())->second;
#ifdef USE_PRED_TOLERANCE
	predDst += TOLERANCE_PRED;
#endif

	if( predNode == predDst )
	{
		if( TRANS_STRICT_BY_PRED )
			return Bet(0.5);
		else
			return true;
	}
	else
		return ( predDst > predNode );
}

vector<CData> CProphet::getDataForTrans(CNode* node)
{
	vector<CData> datas = node->getAllData();

	if( CAPACITY_FORWARD > 0
		&& datas.size() > CAPACITY_FORWARD )
	{
		datas = CSortHelper::mergeSort(datas, CSortHelper::ascendByTimeBirth);
		if( CNode::MODE_QUEUE == CGeneralNode::_fifo )
			datas = vector<CData>(datas.begin(), datas.begin() + CAPACITY_FORWARD);
		else
			datas = vector<CData>(datas.rbegin(), datas.rbegin() + CAPACITY_FORWARD);
	}

	return datas;
}

vector<CData> CProphet::bufferData(CNode* node, vector<CData> datas, int time)
{
	if( datas.empty() )
		return vector<CData>();

	for(vector<CData>::iterator idata = datas.begin(); idata != datas.end(); ++idata)
		idata->arriveAnotherNode(time);

	node->pushIntoBuffer(datas);
	vector<CData> ack = datas;

	vector<CData> overflow = node->updateBufferStatus(time);

	RemoveFromList(ack, overflow);

	return ack;
}

vector<CGeneralData*> CProphet::receiveContents(CNode* node, CSink* sink, vector<CGeneralData*> contents, int time)
{
	vector<CGeneralData*> contentsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
	vector<CData> dataToSend;  //空vector代表拒绝传输数据

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

				//收到RTS，就认为开始一次数据传输尝试
				CNode::transmitTry();

				node->updateDeliveryPredsWithSink();

				if( node->getAllData().empty() )
				{
					//没有数据需要向Sink传输，也认为数据传输成功
					CNode::transmitSucceed();

					return contentsToSend;
				}
				//CTS
				ctrlToSend = new CCtrl(node->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_cts);
				// + DATA
				dataToSend = node->getAllData();

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

				//收到ACK，认为数据传输成功
				CNode::transmitSucceed();

				//收到空的ACK时，结束本次数据传输
				if( ctrl->getACK().empty() )
					return contentsToSend;
				//clear data with ack
				else
					node->checkDataByAck( ctrl->getACK() );

				flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Sink )       " ;

				return contentsToSend;

				break;

			default:
				break;
			}
			++icontent;
		}
		else
		{
			++icontent;
		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
	{
		contentsToSend.push_back(ctrlToSend);
	}
	if( indexToSend != nullptr )
	{
		contentsToSend.push_back(indexToSend);
	}
	if( nodataToSend != nullptr )
	{
		contentsToSend.push_back(nodataToSend);
	}
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			contentsToSend.push_back(new CData(*idata));
	}

	return contentsToSend;
	
}

vector<CGeneralData*> CProphet::receiveContents(CSink* sink, CNode* fromNode, vector<CGeneralData*> contents, int time)
{
	vector<CGeneralData*> contentsToSend;
	CCtrl* ctrlToSend = nullptr;

	for(vector<CGeneralData*>::iterator icontent = contents.begin(); icontent != contents.end(); )
	{
		if( typeid(**icontent) == typeid(CCtrl) )
		{
			CCtrl* ctrl = dynamic_cast<CCtrl*>(*icontent);
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
			vector<CData> ack = CSink::bufferData(time, datas);

			//ACK（如果收到的数据全部被丢弃，发送空的ACK）
			ctrlToSend = new CCtrl(CSink::SINK_ID, ack, time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
		}
	}

	/********************************** wrap ***********************************/

	CPackage* packageToSend = nullptr;
	if( ctrlToSend != nullptr )
		contentsToSend.push_back(ctrlToSend);

	return contentsToSend;

}

vector<CGeneralData*> CProphet::receiveContents(CNode* node, CNode* fromNode, vector<CGeneralData*> contents, int time)
{
	vector<CGeneralData*> contentsToSend;
	CCtrl* ctrlToSend = nullptr;
	CCtrl* capacityToSend = nullptr;
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA包代表缓存为空，没有适合传输的数据
	vector<CData> dataToSend;  //空vector代表拒绝传输数据
	int capacity = -1;

//	int debugNodeID = 0;
//	int debugFromID = 0;

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

				//收到RTS，就认为开始一次数据传输尝试
				CNode::transmitTry();

				//skip if has spoken recently
				if( node->hasSpokenRecently(dynamic_cast<CNode*>(fromNode), time) )
				{
					//跳过传输，也认为数据传输成功
					CNode::transmitSucceed();
					flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  ----- skip -----  Node " << NDigitString(fromNode->getID(), 2) << " )                " ;
					return contentsToSend;
				}
				//rcv RTS from node
				else
				{
					//CTS
					ctrlToSend = new CCtrl(node->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_cts);

					// + Capacity
					if( CNode::MODE_RECEIVE == CNode::_RECEIVE::_selfish 
						&& node->hasData() )
						capacityToSend = new CCtrl(node->getID(), node->getDeliveryPreds(), time, CGeneralNode::SIZE_CTRL, CCtrl::_index);

					// + Index
					indexToSend = new CCtrl(node->getID(), node->getDeliveryPreds(), time, CGeneralNode::SIZE_CTRL, CCtrl::_index);
					//node->skipRTS();  //（暂未使用）
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

			case CCtrl::_index:

				// + DATA / NODATA
				//路由协议允许向该节点转发数据
//				debugNodeID = node->getID();
//				debugFromID = fromNode->getID();
				if( shouldForward(node, ctrl->getPred() ) )
				{
					if( capacity == 0 )
					{
						//不允许发送数据，直接结束传输
						CNode::transmitSucceed();

						return contentsToSend;
					}

					dataToSend = getDataForTrans(node);

					//但缓存为空
					if( dataToSend.empty() )
						nodataToSend = new CCtrl(node->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_no_data);
					else if( capacity > 0 )
					{
						dataToSend = CNode::removeDataByCapacity(dataToSend, capacity);
					}
					
					//如果路由协议允许向该节点发送数据，对方节点就不允许发送数据，因此无需发送capacity
					if( capacityToSend != nullptr )
					{
						free(capacityToSend);
						capacityToSend = nullptr;
					}
				}
				//否则，不允许转发数据时，dataToSend留空

				//update preds
				node->updateDeliveryPredsWith( fromNode->getID(), ctrl->getPred() );

				//注意：如果（取决于Prophet的要求）两个节点都拒绝发送数据，此处将导致空的响应，直接结束本次数据传输，因此需要对空相应调用transmitSucceed()；
				//     否则并不结束传输，还将为对方发来的数据发送ACK；

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//收到ACK，认为数据传输成功
				CNode::transmitSucceed();
				//加入最近邻居列表
				node->addToSpokenCache( fromNode, time );

				//收到空的ACK时，结束本次数据传输
				if( ctrl->getACK().empty() )
					return contentsToSend;
				//clear data with ack
				else
					node->checkDataByAck( ctrl->getACK() );

				flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Node " << NDigitString(fromNode->getID(), 2) << " )                       " ;

				return contentsToSend;

				break;

			case CCtrl::_no_data:

				//收到NODATA，也将回复一个空的ACK，即，也将被认为数据传输成功
				//空的ACK
				ctrlToSend = new CCtrl(node->getID(), vector<CData>(), time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
				//加入最近邻居列表
				node->addToSpokenCache( fromNode, time );

				//维持数据传输的单向性：如果收到数据或NODATA，就不发送数据或NODATA（注意：前提是控制包必须在数据包之前）
				dataToSend.clear();
				if( nodataToSend != nullptr )
				{
					free(nodataToSend);
					nodataToSend = nullptr;
				}

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

			//维持数据传输的单向性：如果收到数据或NODATA，就不发送数据或NODATA（注意：前提是控制包必须在数据包之前）
			dataToSend.clear();
			if( nodataToSend != nullptr )
			{
				free(nodataToSend);
				nodataToSend = nullptr;
			}

			//accept data into buffer
			vector<CData> ack;
			ack = CProphet::bufferData(node, datas, time);
			//ACK（如果收到的数据全部被丢弃，发送空的ACK）
			ctrlToSend = new CCtrl(node->getID(), ack, time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
			//加入最近邻居列表
			node->addToSpokenCache( fromNode, time );

		}
	}

	/********************************** wrap ***********************************/

	if( ctrlToSend != nullptr )
	{
		contentsToSend.push_back(ctrlToSend);
	}
	if( capacityToSend != nullptr )
	{
		contentsToSend.push_back(capacityToSend);
	}
	if( indexToSend != nullptr )
	{
		contentsToSend.push_back(indexToSend);
	}
	if( nodataToSend != nullptr )
	{
		contentsToSend.push_back(nodataToSend);
	}
	if( ! dataToSend.empty() )
	{
		for(auto idata = dataToSend.begin(); idata != dataToSend.end(); ++idata)
			contentsToSend.push_back(new CData(*idata));
	}

	return contentsToSend;

}

bool CProphet::Operate(int currentTime)
{
	if( ! CNode::hasNodes(currentTime) )
		return false;

	bool hasNodes = true;
	if( MAC_PROTOCOL == _hdc )
		hasNodes = CHDC::Operate(currentTime);	
	else
		hasNodes = CSMac::Operate(currentTime);

	if( ! hasNodes )
		return false;

	PrintInfo(currentTime);

	return true;
}
