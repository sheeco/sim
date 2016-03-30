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
	CCtrl* nodataToSend = nullptr;  //NODATA��������Ϊ�գ�û���ʺϴ��������
	vector<CData> dataToSend;  //��vector����ܾ���������

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

				//�յ�RTS������Ϊ��ʼһ�����ݴ��䳢��
				CNode::transmitTry();

				node->updateDeliveryPredsWithSink();

				if( node->getAllData().empty() )
				{
					//û��������Ҫ��Sink���䣬Ҳ��Ϊ���ݴ���ɹ�
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

				//�յ�ACK����Ϊ���ݴ���ɹ�
				CNode::transmitSucceed();

				//�յ��յ�ACKʱ�������������ݴ���
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

			//ACK������յ�������ȫ�������������Ϳյ�ACK��
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
	CCtrl* nodataToSend = nullptr;  //NODATA��������Ϊ�գ�û���ʺϴ��������
	vector<CData> dataToSend;  //��vector����ܾ���������
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

				//�յ�RTS������Ϊ��ʼһ�����ݴ��䳢��
				CNode::transmitTry();

				//skip if has spoken recently
				if( node->hasSpokenRecently(dynamic_cast<CNode*>(fromNode), time) )
				{
					//�������䣬Ҳ��Ϊ���ݴ���ɹ�
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
					//node->skipRTS();  //����δʹ�ã�
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
				//·��Э��������ýڵ�ת������
//				debugNodeID = node->getID();
//				debugFromID = fromNode->getID();
				if( shouldForward(node, ctrl->getPred() ) )
				{
					if( capacity == 0 )
					{
						//�����������ݣ�ֱ�ӽ�������
						CNode::transmitSucceed();

						return contentsToSend;
					}

					dataToSend = getDataForTrans(node);

					//������Ϊ��
					if( dataToSend.empty() )
						nodataToSend = new CCtrl(node->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_no_data);
					else if( capacity > 0 )
					{
						dataToSend = CNode::removeDataByCapacity(dataToSend, capacity);
					}
					
					//���·��Э��������ýڵ㷢�����ݣ��Է��ڵ�Ͳ����������ݣ�������跢��capacity
					if( capacityToSend != nullptr )
					{
						free(capacityToSend);
						capacityToSend = nullptr;
					}
				}
				//���򣬲�����ת������ʱ��dataToSend����

				//update preds
				node->updateDeliveryPredsWith( fromNode->getID(), ctrl->getPred() );

				//ע�⣺�����ȡ����Prophet��Ҫ�������ڵ㶼�ܾ��������ݣ��˴������¿յ���Ӧ��ֱ�ӽ����������ݴ��䣬�����Ҫ�Կ���Ӧ����transmitSucceed()��
				//     ���򲢲��������䣬����Ϊ�Է����������ݷ���ACK��

				break;

				/*************************************** rcv ACK **************************************/

			case CCtrl::_ack:

				//�յ�ACK����Ϊ���ݴ���ɹ�
				CNode::transmitSucceed();
				//��������ھ��б�
				node->addToSpokenCache( fromNode, time );

				//�յ��յ�ACKʱ�������������ݴ���
				if( ctrl->getACK().empty() )
					return contentsToSend;
				//clear data with ack
				else
					node->checkDataByAck( ctrl->getACK() );

				flash_cout << "####  ( Node " << NDigitString(node->getID(), 2) << "  >---- " << NDigitString( ctrl->getACK().size(), 3, ' ') << "  ---->  Node " << NDigitString(fromNode->getID(), 2) << " )                       " ;

				return contentsToSend;

				break;

			case CCtrl::_no_data:

				//�յ�NODATA��Ҳ���ظ�һ���յ�ACK������Ҳ������Ϊ���ݴ���ɹ�
				//�յ�ACK
				ctrlToSend = new CCtrl(node->getID(), vector<CData>(), time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
				//��������ھ��б�
				node->addToSpokenCache( fromNode, time );

				//ά�����ݴ���ĵ����ԣ�����յ����ݻ�NODATA���Ͳ��������ݻ�NODATA��ע�⣺ǰ���ǿ��ư����������ݰ�֮ǰ��
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

			//ά�����ݴ���ĵ����ԣ�����յ����ݻ�NODATA���Ͳ��������ݻ�NODATA��ע�⣺ǰ���ǿ��ư����������ݰ�֮ǰ��
			dataToSend.clear();
			if( nodataToSend != nullptr )
			{
				free(nodataToSend);
				nodataToSend = nullptr;
			}

			//accept data into buffer
			vector<CData> ack;
			ack = CProphet::bufferData(node, datas, time);
			//ACK������յ�������ȫ�������������Ϳյ�ACK��
			ctrlToSend = new CCtrl(node->getID(), ack, time, CGeneralNode::SIZE_CTRL, CCtrl::_ack);
			//��������ھ��б�
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
