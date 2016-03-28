#include "Global.h"
#include "Node.h"
#include "Sink.h"
#include "SMac.h"
#include "HDC.h"
#include "Prophet.h"
#include "SortHelper.h"

bool CProphet::TRANS_STRICT_BY_PRED = true;
int CProphet::MAX_DATA_TRANS = 0;

#ifdef USE_PRED_TOLERANCE

double CProphet::TOLERANCE_PRED = 0;
//double CProphet::DECAY_TOLERANCE_PRED = 1;

#endif


//void CProphet::SendData(int currentTime)
//{
//	if( ! ( currentTime % SLOT_DATA_SEND == 0 ) )
//		return;
//	cout << "########  < " << currentTime << " >  DATA DELIVERY" << endl ;
//
//	ofstream sink( PATH_ROOT + PATH_LOG + FILE_SINK, ios::app);
//	if(currentTime == 0)
//	{
//		sink << INFO_LOG << endl ;
//		sink << INFO_SINK ;
//	}
//
//	int nEncounterAtSink = 0;
//	//���ؽ�����node����x��������
//	vector<CNode *> nodes = CNode::getNodes();
//	nodes = CSortHelper::mergeSort(nodes);
//
//	//�жϹ���״̬����sinkͶ�����ݣ��ڵ��ͨ��
//	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
//	{
//		////�����������״̬��ֱ������
//		//if( ! (*inode)->isListening() )
//		//	continue;
//
//		bool atSink = false;
//		
//		//�����������״̬������
//		if( (*inode)->isListening() )
//		{
//			if( CBasicEntity::getDistance( *CSink::getSink(), **inode ) <= CGeneralNode::RANGE_TRANS )
//			{
//				atSink = true;
//				//deliver data to sink
//				flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getSizeBuffer() << " data to Sink )                     " ;
//				CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );
//				(*inode)->updateDeliveryPredsWithSink();
//				++nEncounterAtSink;
//			}
//		}
//
//		//scan other nodes and forward data
//		//inode < jnode�����κνڵ��ֻ��һ��ͨ�Ż���
//		for(vector<CNode *>::iterator jnode = inode + 1; jnode != nodes.end(); ++jnode)
//		{
//			if( (*jnode)->getX() + CGeneralNode::RANGE_TRANS < (*inode)->getX() )
//				continue;
//			if( (*inode)->getX() + CGeneralNode::RANGE_TRANS < (*jnode)->getX() )
//				break;
//			if( CBasicEntity::getDistance( **inode, **jnode ) > CGeneralNode::RANGE_TRANS )
//				continue;
//
//			if( (*inode)->isAtHotspot() || (*jnode)->isAtHotspot() )
//				CNode::encountAtHotspot();
//			else
//				CNode::encountOnRoute();
//
//			//�����������״̬������
//			if( ! ( (*inode)->isListening() && (*jnode)->isListening() ) )
//				continue;
//
//			CNode::encountActive();
//
//			//init by node with smaller id
//			CNode *smaller, *greater = nullptr;
//			smaller = (*inode)->getID() < (*jnode)->getID() ? *inode : *jnode ;
//			greater = (*inode)->getID() > (*jnode)->getID() ? *inode : *jnode ;
//
//			bool fail = false;
//			bool skip = false;
//			map<int, double> preds;
//			vector<int> sv;
//			vector<CData> data;
//
//			//forward data
//			
//			preds = smaller->sendDeliveryPreds();
//			sv = smaller->sendSummaryVector();
//
//			if( preds.empty() )
//				skip = true;
//			else
//			{
//				preds = greater->receiveDeliveryPredsAndSV(preds, sv );
//				if( preds.empty() )
//					fail = true;
//				else
//				{
//					greater->updateDeliveryPredsWith(smaller->getID(), preds);
//					data = greater->sendDataByPredsAndSV(preds, sv );
//					if( data.empty() )
//						skip = true;
//					else
//						fail = ! smaller->receiveData(currentTime, data) ;
//				}
//			}
//			if( ! fail )
//			{
//				preds = greater->sendDeliveryPreds();
//				sv = greater->sendSummaryVector();
//				if( preds.empty() && skip )
//					skip = true;
//				else
//				{
//					skip = false;
//					preds = smaller->receiveDeliveryPredsAndSV(preds, sv );
//					if( preds.empty() )
//						fail = true;
//					else
//					{
//						smaller->updateDeliveryPredsWith(greater->getID(), preds);
//						data = smaller->sendDataByPredsAndSV(preds, sv );
//						if( data.empty() )
//							skip = true;
//						else
//							fail = ! greater->receiveData( currentTime, data ) ;
//					}
//				}
//				if( ! fail )
//				{	
//					if( skip )
//					{
//						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " skip communication )          " ;
//					}
//					else
//					{
//						flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " finish communication )          " ;
//						
//						//��Sink�����Ľڵ�����ڽ��յ������ڵ������֮����Sink���ж���Ͷ��
//						if( atSink == true )
//						{
//							flash_cout << "####  ( Node " << (*inode)->getID() << " delivers " << (*inode)->getSizeBuffer() << " data to Sink )                     " ;
//							CSink::getSink()->receiveData( currentTime, (*inode)->sendAllData(CGeneralNode::_dump) );						
//						}
//
//					}
//				}
//				else
//				{
//					flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
//				}
//			}
//			else
//			{
//				flash_cout << "####  ( Node " << (*inode)->getID() << " & " << (*jnode)->getID() << " fail communication )          " ;
//			}
//		}
//
//	}
//
//	//�������нڵ��buffer״̬��¼
//	for(vector<CNode *>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
//		(*inode)->recordBufferStatus();
//
//	//����̨���ʱ����һλС��
//	double deliveryRatio = NDigitFloat( CData::getDeliveryRatio() * 100, 1);
//	flash_cout << "####  [ Delivery Ratio ]  " << deliveryRatio << " %                                       " << endl<< endl;
//	sink << currentTime << TAB << nEncounterAtSink << endl;
//	sink.close();
//
//}

CProphet::CProphet() {}

CProphet::~CProphet() {}

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

	if( MAX_DATA_TRANS > 0
		&& datas.size() > MAX_DATA_TRANS )
	{
		datas = CSortHelper::mergeSort(datas, CSortHelper::ascendByTimeBirth);
		if( CNode::MODE_QUEUE == CGeneralNode::_fifo )
			datas = vector<CData>(datas.begin(), datas.begin() + MAX_DATA_TRANS);
		else
			datas = vector<CData>(datas.rbegin(), datas.rbegin() + MAX_DATA_TRANS);
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

				/*************************************** rcv CTS **************************************/

			case CCtrl::_cts:

				break;

				/****************************** rcv Data Index ( dp / sv ) ****************************/

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
	CCtrl* indexToSend = nullptr;
	CCtrl* nodataToSend = nullptr;  //NODATA��������Ϊ�գ�û���ʺϴ��������
	vector<CData> dataToSend;  //��vector����ܾ���������

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

				/****************************** rcv Data Index ( dp / sv ) ****************************/

			case CCtrl::_index:

				// + DATA / NODATA
				//·��Э��������ýڵ�ת������
//				debugNodeID = node->getID();
//				debugFromID = fromNode->getID();
				if( CProphet::shouldForward(node, ctrl->getPred() ) )
				{
					dataToSend = CProphet::getDataForTrans(node);
					//������Ϊ��
					if( dataToSend.empty() )
						nodataToSend = new CCtrl(node->getID(), time, CGeneralNode::SIZE_CTRL, CCtrl::_no_data);
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
				node->addToSpokenCache( (CNode*)(&fromNode), time );

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
				node->addToSpokenCache( (CNode*)(&fromNode), time );

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
			node->addToSpokenCache( (CNode*)(&fromNode), time );

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

bool CProphet::Operate(int currentTime)
{
	if( ! CNode::hasNodes(currentTime) )
		return false;

	if( MAC_PROTOCOL == _hdc )
		CHDC::Operate(currentTime);	
	else
		CSMac::Operate(currentTime);

	PrintInfo(currentTime);

	return true;
}
