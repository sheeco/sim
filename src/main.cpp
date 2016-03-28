#pragma once

#include "Global.h"
#include "Sink.h"
#include "MANode.h"
#include "HDC.h"
#include "Prophet.h"
#include "HAR.h"
#include "Epidemic.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "PostSelect.h"


// TODO: move all func definition into cpp file except for inline func
// TODO: ����������ھ�̬���������� private / protected
// TODO: CConfiguration / CConfigureHelper ?
// TODO: Ĭ�����ò�����Ϊ�� XML ��ȡ
// TODO: MulitCast ?

void InitConfiguration()
{
	/************************************** Default Config **************************************/

	CGeneralNode::RANGE_TRANS = 100;  //transmission range
	CGeneralNode::PROB_TRANS = 1.0;

	CSink::SINK_ID = 0; //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ
	CSink::SINK_X = 0;
	CSink::SINK_Y = 0;
	CSink::CAPACITY_BUFFER = 999999999;  //������

	CNode::INIT_NUM_NODE = 0;

	CRoutingProtocol::SLOT_DATA_SEND = SLOT_MOBILITYMODEL;  //���ݷ���slot
	CNode::DEFAULT_DATA_RATE = 0;
	CNode::SIZE_DATA = 0;
	CGeneralNode::SIZE_CTRL = 0;
	CMacProtocol::SIZE_HEADER_MAC = 8;  //Mac Header Size

	CNode::CAPACITY_BUFFER = 0;
	CNode::CAPACITY_ENERGY = 0;
	CNode::LIFETIME_SPOKEN_CACHE = 0;
	CNode::MODE_RECEIVE = CGeneralNode::_loose;
	CNode::MODE_SEND = CGeneralNode::_dump;
	CNode::MODE_QUEUE = CGeneralNode::_fifo;

	CNode::SLOT_TOTAL = 0;
	CNode::DEFAULT_DUTY_CYCLE = 0;
	CNode::HOTSPOT_DUTY_CYCLE = 0; 
	CNode::DEFAULT_DISCOVER_CYCLE = 0;
	CMacProtocol::RANDOM_STATE_INIT = false;

	/********** Dynamic Node Number **********/
	CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
	CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //��̬�ڵ��������ʱ���ڵ���������仯������
	CNode::INIT_NUM_NODE = 0;
	CNode::MIN_NUM_NODE = 0;
	CNode::MAX_NUM_NODE = 0;
	/********** ------------------- **********/

	/*********************************  HAR  *******************************/

	CMANode::SPEED = 30;
	CMANode::CAPACITY_BUFFER = 100;
	CMANode::MODE_RECEIVE = CGeneralNode::_selfish;

	/******************************  Epidemic  *****************************/

	CData::MAX_HOP = 0;
	CData::MAX_TTL = 0;

	CEpidemic::MAX_DATA_RELAY = -1;

	/******************************  Prophet  ******************************/

	CNode::INIT_DELIVERY_PRED = 0.75;  //�ο�ֵ 0.75
	CNode::RATIO_PRED_DECAY = 0.98;  //�ο�ֵ 0.98(/s)
	CNode::RATIO_PRED_TRANS = 0.25;  //�ο�ֵ 0.25
	CProphet::TRANS_STRICT_BY_PRED = false;
	CProphet::MAX_DATA_TRANS = 0;

#ifdef USE_PRED_TOLERANCE

	CProphet::TOLERANCE_PRED = 0;
//	DECAY_TOLERANCE_PRED = 1;

#endif

	/**************************  Hotspot Select  ***************************/

	CHotspot::SLOT_POSITION_UPDATE = 100;  //������Ϣ�ռ���slot
	CHotspot::SLOT_HOTSPOT_UPDATE = 900;  //�����ȵ�ͷ����slot
	CHotspot::TIME_HOSPOT_SELECT_START = CHotspot::SLOT_HOTSPOT_UPDATE;  //no MA node at first

	CPostSelect::ALPHA = 0.03;  //ratio for post selection
	HAR::BETA = 0.0025;  //ratio for true hotspot
	//HAR::GAMMA = 0.5;  //ratio for HotspotsAboveAverage
	HAR::CO_HOTSPOT_HEAT_A1 = 1;
	HAR::CO_HOTSPOT_HEAT_A2 = 30;

	/******************************* IHAR **********************************/

	HAR::LAMBDA = 0;
	HAR::LIFETIME_POSITION = 3600;

	/***************************** merge-HAR *******************************/

	CHotspotSelect::TEST_HOTSPOT_SIMILARITY = true;
	CHotspot::RATIO_MERGE_HOTSPOT = 1.0;
	CHotspot::RATIO_NEW_HOTSPOT = 1.0;
	CHotspot::RATIO_OLD_HOTSPOT = 1.0;
	HAR::MIN_WAITING_TIME = 0;  //add minimum waiting time to each hotspot
	HAR::TEST_BALANCED_RATIO = false;

	//HAR::TEST_LEARN = false;
	//CPosition::CO_POSITION_DECAY = 1.0;
	//HAR::MIN_POSITION_WEIGHT = 0;


	/************************************** Necessary Config **************************************/

	SLOT = 1;
	// TODO: should be read from .trace file
	SLOT_MOBILITYMODEL = 30;
	SLOT_LOG = 100;

	DATATIME = 15000;
	RUNTIME = 15000;

	/*********** Depend on DATASET ***********/
	DATASET = "KAIST";
	CGeneralNode::RANGE_TRANS = 250;
	CSink::SINK_X = -200;  //for KAIST
	CSink::SINK_Y = 200;
	CNode::INIT_NUM_NODE = 29;
	CNode::MIN_NUM_NODE = CNode::INIT_NUM_NODE * 1.4;
	CNode::MAX_NUM_NODE = CNode::INIT_NUM_NODE * 0.6;
	/*********** ----------------- ***********/

	/***************************  Node & Data  ***************************/

	CGeneralNode::PROB_TRANS = 1.0;

	CNode::DEFAULT_DATA_RATE = 1.0 / 30.0;
	CNode::CAPACITY_BUFFER = 200;
	CNode::CAPACITY_ENERGY = 0;
	CNode::LIFETIME_SPOKEN_CACHE = 0;

	CNode::SIZE_DATA = 250;  //Up to 250 Bytes
	CGeneralNode::SIZE_CTRL = 10;

	/********************************  DC  ********************************/

	CNode::SLOT_TOTAL = 10 * SLOT_MOBILITYMODEL;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
	CNode::DEFAULT_DISCOVER_CYCLE = 10; 

	/******************************  Prophet  ******************************/

	CProphet::TRANS_STRICT_BY_PRED = false;
	CProphet::MAX_DATA_TRANS = 0;

	/** Opt **/
#ifdef USE_PRED_TOLERANCE

	CProphet::TOLERANCE_PRED = 0;
//	DECAY_TOLERANCE_PRED = 1;

#endif

}

void Help()
{
	cout << INFO_HELP << endl;
	ofstream help(FILE_HELP, ios::out);
	help << INFO_HELP;
	help.close();
}

bool ParseArguments(int argc, char* argv[])
{
	try
	{
		int iField = 0;
		for(iField = 1; iField < argc; )
		{
			string field = argv[iField];

			//<mode> ������ֵ�Ĳ����Ͳ���
			if( field == "-hdc" )
			{
				MAC_PROTOCOL = _hdc;
				++iField;
			}
			else if( field == "-prophet" )
			{
				ROUTING_PROTOCOL = _prophet;
				++iField;
			}
			else if( field == "-epidemic" )
			{
				ROUTING_PROTOCOL = _epidemic;
				++iField;
			}
			else if( field == "-har" )
			{
				ROUTING_PROTOCOL = _har;
				++iField;
			}
			else if( field == "-hs" )
			{
				HOTSPOT_SELECT = _original;
				++iField;
			}
			else if( field == "-ihs" )
			{
				HOTSPOT_SELECT = _improved;
				++iField;
			}
			else if( field == "-mhs" )
			{
				HOTSPOT_SELECT = _merge;
				++iField;
			}
			else if( field == "-hotspot-similarity" )
			{
				CHotspotSelect::TEST_HOTSPOT_SIMILARITY = true;
				++iField;
			}
			else if( field == "-dynamic-node-number" )
			{
				CMacProtocol::TEST_DYNAMIC_NUM_NODE = true;
				++iField;
			}
			else if( field == "-balanced-ratio" )
			{
				HAR::TEST_BALANCED_RATIO = true;
				++iField;
			}
			else if( field == "-random-state" )
			{
				CMacProtocol::RANDOM_STATE_INIT = true;
				++iField;
			}

			//���Ͳ���
			else if( field == "-time-data" )
			{
				if( iField < argc - 1 )
					DATATIME = atoi( argv[ iField + 1 ] );
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			else if( field == "-time-run" )
			{
				if( iField < argc - 1 )
					RUNTIME = atoi( argv[ iField + 1 ] );
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			else if( field == "-slot" )
			{
				if( iField < argc - 1 )
					SLOT = atoi( argv[ iField + 1 ] );

				if( SLOT > SLOT_MOBILITYMODEL )
					SLOT = SLOT_MOBILITYMODEL;
				iField += 2;
			}
			else if( field == "-node" )
			{
				if( iField < argc - 1 )
					CNode::INIT_NUM_NODE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-log-slot" )
			{
				if( iField < argc - 1 )
					SLOT_LOG = atoi( argv[ iField + 1 ] );
				//�۲����ڲ�ӦС�ڹ�������
				if( SLOT_LOG < CNode::SLOT_TOTAL )
					SLOT_LOG = CNode::SLOT_TOTAL;

				iField += 2;
			}
			else if( field == "-trans-range" )
			{
				if( iField < argc - 1 )
					CGeneralNode::RANGE_TRANS = atoi( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-lifetime" )
			{
				if( iField < argc - 1 )
					HAR::LIFETIME_POSITION = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-cycle" )
			{
				if( iField < argc - 1 )
					CNode::SLOT_TOTAL = atoi( argv[ iField + 1 ] );
				//�۲����ڲ�ӦС�ڹ�������
				if( SLOT_LOG < CNode::SLOT_TOTAL )
					SLOT_LOG = CNode::SLOT_TOTAL;
				iField += 2;
			}
			else if( field == "-slot-discover" )
			{
				if( iField < argc - 1 )
					CNode::DEFAULT_DISCOVER_CYCLE = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-dc-default" )
			{
				if( iField < argc - 1 )
					CNode::DEFAULT_DUTY_CYCLE = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-dc-hotspot" )
			{
				if( iField < argc - 1 )
					CNode::HOTSPOT_DUTY_CYCLE = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-hop" )
			{
				if( iField < argc - 1 )
					CData::MAX_HOP = atoi( argv[ iField + 1 ] );
				if( ( CData::MAX_HOP > 0 )
					&& ( CData::MAX_TTL > 0 ) )
				{
					string error = "Error @ ParseArguments() : Argument -hop & -ttl cannot be used both";
					cout << error << endl;
					_PAUSE_;
					Exit(EINVAL, error);
				}

				iField += 2;
			}
			else if( field == "-ttl" )
			{
				if( iField < argc - 1 )
					CData::MAX_TTL = atoi( argv[ iField + 1 ] );
				if( ( CData::MAX_HOP > 0 )
					&& ( CData::MAX_TTL > 0 ) )
				{
					string error = "Error @ ParseArguments() : Argument -hop & -ttl cannot be used both";
					cout << error << endl;
					_PAUSE_;
					Exit(EINVAL, error);
				}

				iField += 2;
			}
			else if( field == "-buffer" )
			{
				if( iField < argc - 1 )
					CNode::CAPACITY_BUFFER = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-data-rate" )
			{
				if( iField < argc - 1 )
					CNode::DEFAULT_DATA_RATE = double(1) / atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-data-size" )
			{
				if( iField < argc - 1 )
					CNode::SIZE_DATA = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-energy" )
			{
				if( iField < argc - 1 )
					CNode::CAPACITY_ENERGY = 1000 * atoi( argv[ iField + 1 ] );
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			else if( field == "-queue" )
			{
				if( iField < argc - 1 )
					CEpidemic::MAX_DATA_RELAY = atoi( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-spoken" )
			{
				if( iField < argc - 1 )
					CNode::LIFETIME_SPOKEN_CACHE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}


			//double����
			else if( field == "-alpha" )
			{
				if( iField < argc - 1 )
					CPostSelect::ALPHA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-beta" )
			{
				if( iField < argc - 1 )
					HAR::BETA = atof( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-lambda" )
			{
				if( iField < argc - 1 )
					HAR::LAMBDA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-merge" )
			{
				if( iField < argc - 1 )
					CHotspot::RATIO_MERGE_HOTSPOT = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-old" )
			{
				if( iField < argc - 1 )
					CHotspot::RATIO_OLD_HOTSPOT = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-trans-prob" )
			{
				if( iField < argc - 1 )
					CGeneralNode::PROB_TRANS = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-pred-init" )
			{
				if( iField < argc - 1 )
					CNode::INIT_DELIVERY_PRED = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-pred-decay" )
			{
				if( iField < argc - 1 )
					CNode::RATIO_PRED_DECAY = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			//ʵ���϶�WSN���Բ���ʹ�õ�
//			else if( field == "-pred-trans" )
//			{
//				if( iField < argc - 1 )
//					CNode::RATIO_PRED_TRANS = atof( argv[ iField + 1 ] );
//				iField += 2;
//			}
			else if( field == "-pred-tolerance" )
			{
#ifdef USE_PRED_TOLERANCE
				if( iField < argc - 1 )
					CProphet::TOLERANCE_PRED = atof( argv[ iField + 1 ] );
#endif
				iField += 2;
			}

			//��������������ֵ�Ĳ���
			else if( field == "-sink" )
			{
				if(iField < argc - 2)
				{
					CSink::SINK_X = atof( argv[ iField + 1 ] );
					CSink::SINK_Y = atof( argv[ iField + 2 ] );
				}
				iField += 3;
			}			
			else if( field == "-heat" )
			{
				if(iField < argc - 2)
				{
					HAR::CO_HOTSPOT_HEAT_A1 = atof( argv[ iField + 1 ] );
					HAR::CO_HOTSPOT_HEAT_A2 = atof( argv[ iField + 2 ] );
				}
				iField += 3;
			}

			//�ַ�������
			else if( field == "-dataset" )
			{
				if(iField < argc - 2)
				{
					char arg[20] = {'\0'};
					strcpy(arg, argv[ iField + 1 ]);
					DATASET = string( arg );
				}
				iField += 2;
			}			
			else if( field == "-log-path" )
			{
				if(iField < argc - 2)
				{
					char arg[20] = {'\0'};
					strcpy(arg, argv[ iField + 1 ]);
					PATH_ROOT = "../" + string( arg ) + "/";
				}
				iField += 2;
			}			


			//���help��Ϣ
			else if( field == "-help" )
			{
				Help();
				_PAUSE_;
				Exit(ESKIP);
			}
			else
				++iField;

		}
	}
	catch(exception e)
	{
		stringstream error;
		error << "Error @ ParseArguments() : Wrong Parameter Format!";
		cout << endl << error.str() << endl;
		Help();
		_PAUSE_;
		Exit(EINVAL, error.str());
		return false;
	}

	// Generate timestamp & output path

	// Create root path (../test/) if doesn't exist
	if( access(PATH_ROOT.c_str(), 00) != 0 )
		_mkdir(PATH_ROOT.c_str());

	time_t seconds;  //��ʱ��  
	char temp[65] = {'\0'};
	seconds = time(nullptr); //��ȡĿǰ��ʱ��  
	strftime(temp, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));  
	TIMESTAMP = string(temp);
	strftime(temp, 64, "%Y-%m-%d-%H-%M-%S", localtime(&seconds));  
	string timestring;
	timestring = string(temp);
	INFO_LOG = "@" + TIMESTAMP + TAB;
	PATH_LOG = "." + timestring + "/";

	// Create log path
	if( access( (PATH_ROOT + PATH_LOG).c_str(), 00 ) != 0 )
		_mkdir( (PATH_ROOT + PATH_LOG).c_str() );

	// Hide folder
	LPWSTR wstr = CString( (PATH_ROOT + PATH_LOG).c_str()).AllocSysString();
	int attr = GetFileAttributes( wstr );
	if ( (attr & FILE_ATTRIBUTE_HIDDEN) == 0 )
	{
		SetFileAttributes(wstr, attr | FILE_ATTRIBUTE_HIDDEN);
	}

	return true;

}

void PrintConfiguration()
{
	ofstream parameters( PATH_ROOT + PATH_LOG + FILE_PARAMETES, ios::app);
	parameters << endl << endl << INFO_LOG << endl << endl;

	parameters << "CYCLE" << TAB << CNode::SLOT_TOTAL << endl;
	parameters << "DEFAULT_DC" << TAB << CNode::DEFAULT_DUTY_CYCLE<< endl;
	if( MAC_PROTOCOL == _hdc )
		parameters << "HOTSPOT_DC" << TAB << CNode::HOTSPOT_DUTY_CYCLE << endl;

	if( CData::useHOP() )
		parameters << "HOP" << TAB << CData::MAX_HOP << endl;
	else
		parameters << "TTL" << TAB << CData::MAX_TTL << endl;
	parameters << "DATA_RATE" << TAB << "1 / " << int( 1 / CNode::DEFAULT_DATA_RATE ) << endl;
	parameters << "DATA_SIZE" << TAB << CNode::SIZE_DATA << endl;
	parameters << "BUFFER" << TAB << CNode::CAPACITY_BUFFER << endl;
	parameters << "ENERGY" << TAB << CNode::CAPACITY_ENERGY << endl;

	parameters << "DATA_TIME" << TAB << DATATIME << endl;
	parameters << "RUN_TIME" << TAB << RUNTIME << endl;
	parameters << "PROB_TRANS" << TAB << CGeneralNode::PROB_TRANS << endl;
	parameters << "DATA_TRANS" << TAB << CProphet::MAX_DATA_TRANS << endl;

	CNode::INIT_DELIVERY_PRED = 0.70;  //0.75
	CNode::RATIO_PRED_DECAY = 0.90;  //0.98(/s)
	CNode::RATIO_PRED_TRANS = 0.20;  //0.25
	CProphet::MAX_DATA_TRANS = 0;


	//����ļ�Ϊ��ʱ������ļ�ͷ
	ofstream final( PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app);
	final.seekp(0, ios::end);
	if( ! final.tellp() )
		final << INFO_FINAL ;

	final << DATATIME << TAB << RUNTIME << TAB << CGeneralNode::PROB_TRANS << TAB << CNode::CAPACITY_BUFFER << TAB << CNode::CAPACITY_ENERGY << TAB ;
	if( CData::useHOP() )
		final << CData::MAX_HOP << TAB ;
	else 
		final << CData::MAX_TTL << TAB ;

	final << CNode::SLOT_TOTAL << TAB << CNode::DEFAULT_DUTY_CYCLE << TAB ;

	parameters << endl;
	if( ROUTING_PROTOCOL == _epidemic )
	{
		INFO_LOG += "$Epidemic ";
		parameters << "$Epidemic " << endl << endl;
	}
	else if( ROUTING_PROTOCOL == _prophet )
	{
		INFO_LOG += "$Prophet ";
		parameters << "$Prophet " << endl << endl;
		parameters << "PRED_INIT" << TAB << CNode::INIT_DELIVERY_PRED << endl;
		parameters << "PRED_DECAY" << TAB << CNode::RATIO_PRED_DECAY << endl;
		parameters << "PRED_TRANS" << TAB << CNode::RATIO_PRED_TRANS << endl;
#ifdef USE_PRED_TOLERANCE
		parameters << "PRED_TOLERANCE" << TAB << CProphet::TOLERANCE_PRED << endl;
//		parameters << "PRED_TOLERANCE" << TAB << CProphet::DECAY_TOLERANCE_PRED << endl;
#endif
	}

	if( MAC_PROTOCOL == _hdc )
	{
		INFO_LOG += "$HDC ";
		parameters << "$HDC " << endl << endl;
	}

	if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
	{
		if( HOTSPOT_SELECT == _improved )
		{
			INFO_LOG += "$IHAR ";
			parameters << "$IHAR " << endl << endl;
			parameters << "POSITION_LIFETIME" << TAB << HAR::LIFETIME_POSITION << endl << endl;

			final << HAR::LIFETIME_POSITION << TAB ;
		}

		else if( HOTSPOT_SELECT == _merge )
		{
			INFO_LOG += "$mHAR ";
			parameters << "$mHAR " << endl << endl;
			parameters << "RATIO_MERGE" << TAB << CHotspot::RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << CHotspot::RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << CHotspot::RATIO_OLD_HOTSPOT << endl;

			final << CHotspot::RATIO_MERGE_HOTSPOT << TAB << CHotspot::RATIO_OLD_HOTSPOT << TAB ;

		}

		else
		{
			final << CNode::HOTSPOT_DUTY_CYCLE << TAB << CPostSelect::ALPHA << TAB << HAR::BETA << TAB ;
		}

		parameters << "ALPHA" << TAB << CPostSelect::ALPHA << endl;
		parameters << "BETA" << TAB << HAR::BETA << endl;
		parameters << "HEAT_CO_1" << TAB << HAR::CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << HAR::CO_HOTSPOT_HEAT_A2 << endl;
	}

	if( CMacProtocol::TEST_DYNAMIC_NUM_NODE)
	{
		INFO_LOG += "#DYNAMIC_NODE_NUMBER";
		parameters << endl;
		parameters << "#DYNAMIC_NODE_NUMBER" << endl;
	}

	parameters << endl;

	parameters.close();
	final.close();

	//����汾��Ϣ
	ifstream versionInput( PATH_RUN + FILE_VERION, ios::in);
	ofstream version( PATH_ROOT + PATH_LOG + FILE_VERION, ios::out);

	version << versionInput.rdbuf();

	versionInput.close();
	version.close();
}

bool Run()
{
	int currentTime = 0;
	while( currentTime <= RUNTIME )
	{
		bool dead = false;

		switch( ROUTING_PROTOCOL )
		{
			case _prophet:
				dead = ! CProphet::Operate(currentTime);
				break;

			case _epidemic:
				dead = ! CEpidemic::Operate(currentTime);
				break;

			case _har:
				dead = ! HAR::Operate(currentTime);
				break;

			default:
				break;
		}

		if( dead )
		{
			RUNTIME = currentTime;
			break;
		}
		currentTime += SLOT;
	}

	switch( ROUTING_PROTOCOL )
	{
		case _prophet:
			CProphet::PrintFinal(currentTime);
			break;

		case _epidemic:
			CEpidemic::PrintFinal(currentTime);
			break;

		case _har:
			HAR::PrintFinal(currentTime);
			break;

		default:
			break;
	}

	return true;
}

int main(int argc, char* argv[])
{
	// TODO: release �汾��Ӧ��Ϊ while(1) ѭ��

	/************************************ ����Ĭ��ֵ *************************************/

	InitConfiguration();

	/************************************ �����в������� *************************************/

	ParseArguments(argc, argv);

	/********************************* ��log��Ϣ�Ͳ�����Ϣд���ļ� ***********************************/
	
	PrintConfiguration();

	srand( static_cast<unsigned>(time(nullptr)) ); 

	Run();

	Exit(EFINISH);

	_ALERT_;
}