#pragma once

#include "../.project/version.h"
#include "Global.h"
#include "Sink.h"
#include "MANode.h"
#include "HDC.h"
#include "Prophet.h"
#include "HAR.h"
//#include "Epidemic.h"
#include "SortHelper.h"
#include "HotspotSelect.h"
#include "PostSelect.h"
#include "Trace.h"
#include "FileHelper.h"


// TODO: move all func definition into cpp file except for inline func
// TODO: ����������ھ�̬���������� private / protected
// TODO: CConfiguration / CConfigureHelper ?
// TODO: Ĭ�����ò�����Ϊ�� XML ��ȡ
// TODO: MulitCast ?


void Help()
{
	//cout << INFO_HELP << endl;
	//ofstream help(FILE_HELP, ios::out);
	//help << INFO_HELP;
	//help.close();

	ifstream help(PATH_RUN + FILE_HELP, ios::in);
	cout << help.rdbuf();
	help.close();
}

bool ParseConfiguration(int argc, char* argv[], string description)
{
	if( argc <= 0 )
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Parameters are requested ! ";
		cout << endl << error.str() << endl;
		Help();
		_PAUSE_;
		Exit(EINVAL, error.str());
		return false;
	}

	// ��ʹ�õ������в���������ļ�
	ofstream config_log(PATH_ROOT + PATH_LOG + FILE_CONFIG, ios::app);
	config_log << "######  " << description << endl;
	for( int i = 0; i < argc; ++i )
	{
		if( argv[i][0] == '-' )
			config_log << endl;
		config_log << argv[i] << " ";
	}

	config_log.close();

	try
	{
		for( int iField = 0; iField < argc; )
		{
			string field = argv[iField];
			char *endptr = nullptr;

			//<mode> ����ֵ��Ĳ���
			if( field == "--hdc" )
			{
				MAC_PROTOCOL = _hdc;
				HOTSPOT_SELECT = _original;
				++iField;
			}
			else if( field == "--prophet" )
			{
				ROUTING_PROTOCOL = _prophet;
				++iField;
			}
			//			else if( field == "-epidemic" )
			//			{
			//				ROUTING_PROTOCOL = _epidemic;
			//				++iField;
			//			}
			else if( field == "--har" )
			{
				ROUTING_PROTOCOL = _xhar;
				HOTSPOT_SELECT = _original;
				++iField;
			}
			else if( field == "--hs" )
			{
				HOTSPOT_SELECT = _original;
				++iField;
			}
			else if( field == "--ihs" )
			{
				HOTSPOT_SELECT = _improved;
				++iField;
			}
			else if( field == "--mhs" )
			{
				HOTSPOT_SELECT = _merge;
				++iField;
			}

			//bool ����
			else if( field == "-continuous-trace" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				string val(argv[iField + 1]);
				if( val == "on" )
					CCTrace::CONTINUOUS_TRACE = true;
				else if( val == "off" )
					CCTrace::CONTINUOUS_TRACE = false;
				else
					throw field;
				iField += 2;
			}
			else if( field == "-hotspot-similarity" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				string val(argv[iField + 1]);
				if( val == "on" )
					CHotspotSelect::TEST_HOTSPOT_SIMILARITY = true;
				else if( val == "off" )
					CHotspotSelect::TEST_HOTSPOT_SIMILARITY = false;
				else
					throw field;
				iField += 2;
			}
			else if( field == "-dynamic-node-number" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				string val(argv[iField + 1]);
				if( val == "on" )
					CMacProtocol::TEST_DYNAMIC_NUM_NODE = true;
				else if( val == "off" )
					CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
				else
					throw field;
				iField += 2;
			}
			else if( field == "-balanced-ratio" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				string val(argv[iField + 1]);
				if( val == "on" )
					HAR::TEST_BALANCED_RATIO = true;
				else if( val == "off" )
					HAR::TEST_BALANCED_RATIO = false;
				else
					throw field;
				iField += 2;
			}
			else if( field == "-random-state" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				string val(argv[iField + 1]);
				if( val == "on" )
					CMacProtocol::RANDOM_STATE_INIT = true;
				else if( val == "off" )
					CMacProtocol::RANDOM_STATE_INIT = false;
				else
					throw field;
				iField += 2;
			}

			//int ����
			else if( field == "-time-data" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				DATATIME = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			else if( field == "-time-run" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				RUNTIME = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			//else if( field == "-slot" )
			//{
			//	if( iField < argc - 1 )
			//		SLOT = atoi( argv[ iField + 1 ] );

			//	if( SLOT > CCTrace::SLOT_TRACE )
			//		SLOT = CCTrace::SLOT_TRACE;
			//	iField += 2;
			//}
			else if( field == "-node" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::INIT_NUM_NODE = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-log-slot" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				SLOT_LOG = strtol( argv[iField + 1], &endptr, 10);
				//				//�۲����ڲ�ӦС�ڹ�������
				//				if( SLOT_LOG < CNode::SLOT_TOTAL )
				//					SLOT_LOG = CNode::SLOT_TOTAL;

				iField += 2;
			}
			else if( field == "-trans-range" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CGeneralNode::RANGE_TRANS = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-trans-speed" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::SPEED_TRANS = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-lifetime" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CHotspotSelect::LIFETIME_POSITION = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-cycle" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::SLOT_TOTAL = strtol( argv[iField + 1], &endptr, 10);

				//�۲����ڲ�ӦС�ڹ�������
				if( SLOT_LOG < CNode::SLOT_TOTAL )
					SLOT_LOG = CNode::SLOT_TOTAL;

				iField += 2;
			}
			else if( field == "-slot-carrier-sense" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::DEFAULT_SLOT_CARRIER_SENSE = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-dc-default" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::DEFAULT_DUTY_CYCLE = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-dc-hotspot" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::HOTSPOT_DUTY_CYCLE = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-hop" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CData::MAX_HOP = strtol( argv[iField + 1], &endptr, 10);
				//				if( ( CData::MAX_HOP > 0 )
				//					&& ( CData::MAX_TTL > 0 ) )
				//				{
				//					string error = "Error @ ParseConfiguration() : Argument -hop & -ttl cannot be used both";
				//					cout << error << endl;
				//					_PAUSE_;
				//					Exit(EINVAL, error);
				//				}

				iField += 2;
			}
			//			else if( field == "-ttl" )
			//			{
			//				if( iField < argc - 1 )
			//					CData::MAX_TTL = atoi( argv[ iField + 1 ] );
			//				if( ( CData::MAX_HOP > 0 )
			//					&& ( CData::MAX_TTL > 0 ) )
			//				{
			//					string error = "Error @ ParseConfiguration() : Argument -hop & -ttl cannot be used both";
			//					cout << error << endl;
			//					_PAUSE_;
			//					Exit(EINVAL, error);
			//				}
			//
			//				iField += 2;
			//			}
			else if( field == "-buffer" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::CAPACITY_BUFFER = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-buffer-ma" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CMANode::CAPACITY_BUFFER = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-data-rate" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::DEFAULT_DATA_RATE = double(1) / strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-data-size" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::SIZE_DATA = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-energy" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::CAPACITY_ENERGY = 1000 * strtol( argv[iField + 1], &endptr, 10);
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			//			else if( field == "-queue" )
			//			{
			//				if( iField < argc - 1 )
			//					CEpidemic::MAX_DATA_RELAY = atoi( argv[ iField + 1 ] );
			//				iField += 2;
			//			}			
			else if( field == "-spoken" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CNode::LIFETIME_SPOKEN_CACHE = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}
			else if( field == "-trans-window" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CRoutingProtocol::WINDOW_TRANS = strtol( argv[iField + 1], &endptr, 10);
				iField += 2;
			}


			//double ����
			else if( field == "-alpha" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CPostSelect::ALPHA = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-beta" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				HAR::BETA = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-lambda" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CHotspotSelect::LAMBDA = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-merge" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CHotspotSelect::RATIO_MERGE_HOTSPOT = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-old" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CHotspotSelect::RATIO_OLD_HOTSPOT = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-trans-prob" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CGeneralNode::PROB_TRANS = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-pred-init" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CProphet::INIT_PRED = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			else if( field == "-pred-decay" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;
				CProphet::RATIO_PRED_DECAY = strtod( argv[iField + 1], &endptr);
				iField += 2;
			}
			//ʵ���϶�WSN���Բ���ʹ�õ�
			//			else if( field == "-pred-trans" )
			//			{
			//				if( iField < argc - 1 )
			//					CProphet::RATIO_PRED_TRANS = atof( argv[ iField + 1 ] );
			//				iField += 2;
			//			}

			//�ַ�������
			else if( field == "-dataset" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;

				char arg[20] = { '\0' };
				strcpy(arg, argv[iField + 1]);
				DATASET = string(arg);

				iField += 2;
			}
			else if( field == "-log-path" )
			{
				if( ( iField + 1 ) >= argc )
					throw field;

				char arg[20] = { '\0' };
				strcpy(arg, argv[iField + 1]);
				PATH_ROOT = "../" + string(arg) + "/";

				iField += 2;
			}


			//��������������ֵ�Ĳ���
			else if( field == "-sink" )
			{
				if( ( iField + 2 ) >= argc )
					throw field;
				CSink::SINK_X = strtod( argv[iField + 1], &endptr);
				CSink::SINK_Y = atof(argv[iField + 2]);
				iField += 3;
			}
			else if( field == "-heat" )
			{
				if( ( iField + 2 ) >= argc )
					throw field;
				HAR::CO_HOTSPOT_HEAT_A1 = strtod( argv[iField + 1], &endptr);
				HAR::CO_HOTSPOT_HEAT_A2 = atof(argv[iField + 2]);
				iField += 3;
			}


			//���help��Ϣ
			else if( field == "-help" )
			{
				Help();
				_PAUSE_;
				Exit(ESKIP);
			}
			else
				throw argv[iField];

			if( endptr != nullptr 
			    && *endptr != '\0' )
			{
				throw field;
			}

		}
	}

	// Unkown command name
	catch( char* arg )
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Cannot find command '" << arg << "' ! ";
		cout << endl << error.str() << endl;
		Help();
		_PAUSE_;
		Exit(EINVAL, error.str());
		return false;
	}

	// Wrong value format for command
	catch( string field )
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Wrong value for command '" << field << "' ! ";
		cout << endl << error.str() << endl;
		_PAUSE_;
		Exit(EINVAL, error.str());
		return false;
	}

	catch( exception e )
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Unknown parse error ! ";
		cout << endl << error.str() << endl;
		Help();
		_PAUSE_;
		Exit(EINVAL, error.str());
		return false;
	}
	return true;

}

bool ParseConfiguration(string filename)
{
	if( !CFileHelper::IfExists(filename) )
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Cannot find file \"" << filename << "\" ! ";
		cout << endl << error.str() << endl;
		_PAUSE_;
		Exit(ENOENT, error.str());
	}

	//read string from file
	ifstream file(filename, ios::in);
	string config(( std::istreambuf_iterator<char>(file) ), std::istreambuf_iterator<char>());

	//parse string into tokens
	char* delim = " \t\n";
	vector<char*> args; 
	char* pToken = strtok( const_cast<char*>( config.c_str() ), delim);
	
	while( pToken != nullptr )
	{
		args.push_back(pToken);
		pToken = strtok(nullptr, delim);
	}

	if( ! args.empty() )
	{
		int argc = args.size();
		char** argv = (char**)(new char*[argc]);
		for(int i = 0; i < argc; ++i)
			argv[i] = args[i];
		
		bool rtn = ParseConfiguration(argc, argv, filename);
		delete argv;
		return rtn;
	}
	else
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Cannot find configuration in " << filename << "!";
		cout << endl << error.str() << endl;
		_PAUSE_;
		Exit(ENOEXEC, error.str());
		return false;
	}
}

void InitConfiguration()
{
	SLOT = 1;
	SLOT_LOG = 100;

	/************************************** Default Config **************************************/

	CGeneralNode::RANGE_TRANS = 100;
	CNode::SPEED_TRANS = 2500;
	CGeneralNode::PROB_TRANS = 1.0;

	CSink::SINK_ID = 0; //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ
	CSink::SINK_X = 0;
	CSink::SINK_Y = 0;
	CSink::CAPACITY_BUFFER = 999999999;  //������

	CNode::INIT_NUM_NODE = 0;

	//CRoutingProtocol::SLOT_DATA_SEND = CCTrace::SLOT_TRACE;  //���ݷ���slot
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
	CNode::DEFAULT_SLOT_CARRIER_SENSE = 0;
	CMacProtocol::RANDOM_STATE_INIT = true;

	/********** Dynamic Node Number **********/
	CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
	CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;  //��̬�ڵ��������ʱ���ڵ���������仯������
	CNode::INIT_NUM_NODE = 0;
	CNode::MIN_NUM_NODE = 0;
	CNode::MAX_NUM_NODE = 0;
	/********** ------------------- **********/

	/*********************************  HAR  *******************************/

	CMANode::START_COUNT_ID = 100;  //ID����ʼֵ�����ںʹ������ڵ�������
	CMANode::SPEED = 30;
	CMANode::CAPACITY_BUFFER = 100;
	//CMANode::MODE_RECEIVE = CGeneralNode::_selfish;
	CMANode::MODE_RECEIVE = CGeneralNode::_loose;

	HAR::CO_HOTSPOT_HEAT_A1 = 1;
	HAR::CO_HOTSPOT_HEAT_A2 = 30;

	/******************************  Epidemic  *****************************/

	CData::MAX_HOP = 0;
//	CData::MAX_TTL = 0;

//	CEpidemic::MAX_DATA_RELAY = -1;

	/******************************  Prophet  ******************************/

	CProphet::INIT_PRED = 0.70;  //�ο�ֵ 0.75
	CProphet::RATIO_PRED_DECAY = 0.90;  //�ο�ֵ 0.98(/s)
	CProphet::RATIO_PRED_TRANS = 0.20;  //�ο�ֵ 0.25
	CProphet::TRANS_STRICT_BY_PRED = false;
	CProphet::WINDOW_TRANS = 10;


	/**************************  Hotspot Select  ***************************/

	CHotspotSelect::SLOT_POSITION_UPDATE = 100;  //������Ϣ�ռ���slot
	CHotspotSelect::SLOT_HOTSPOT_UPDATE = 900;  //�����ȵ�ͷ����slot
	CHotspotSelect::STARTTIME_HOSPOT_SELECT = CHotspotSelect::SLOT_HOTSPOT_UPDATE;  //no MA node at first
	CHotspotSelect::TEST_HOTSPOT_SIMILARITY = true;

	CPostSelect::ALPHA = 0.03;  //ratio for post selection
	HAR::BETA = 0.0025;  //ratio for true hotspot
	//HAR::GAMMA = 0.5;  //ratio for HotspotsAboveAverage

	/******************************** IHS **********************************/

	CHotspotSelect::LAMBDA = 0;
	CHotspotSelect::LIFETIME_POSITION = 3600;

	/****************************** merge-HS *******************************/

	CHotspotSelect::RATIO_MERGE_HOTSPOT = 1.0;
	CHotspotSelect::RATIO_NEW_HOTSPOT = 1.0;
	CHotspotSelect::RATIO_OLD_HOTSPOT = 1.0;
	HAR::MIN_WAITING_TIME = 0;  //add minimum waiting time to each hotspot
	HAR::TEST_BALANCED_RATIO = false;

	//HAR::TEST_LEARN = false;
	//CPosition::CO_POSITION_DECAY = 1.0;
	//HAR::MIN_POSITION_WEIGHT = 0;


	/************************************** Necessary Config **************************************/

	// TODO: should be read from .trace file
	CCTrace::SLOT_TRACE = 30;
	DATATIME = 15000;
	RUNTIME = 15000;

	/*********** Depend on DATASET ***********/
	CCTrace::CONTINUOUS_TRACE = false;
	DATASET = "KAIST";
	//CGeneralNode::RANGE_TRANS = 250;
	CSink::SINK_X = -200;  //for KAIST
	CSink::SINK_Y = 200;
	CNode::INIT_NUM_NODE = 29;
	CNode::MIN_NUM_NODE = int( CNode::INIT_NUM_NODE * 1.4 );
	CNode::MAX_NUM_NODE = int( CNode::INIT_NUM_NODE * 0.6 );
	/*********** ----------------- ***********/

	/***************************  Node & Data  ***************************/

	CGeneralNode::PROB_TRANS = 1.0;

	CNode::DEFAULT_DATA_RATE = 1.0 / 30.0;
	CNode::CAPACITY_BUFFER = 0;
	CNode::CAPACITY_ENERGY = 0;
	CNode::LIFETIME_SPOKEN_CACHE = 0;

	CNode::SIZE_DATA = 200;  //Up to 250 Bytes
	CGeneralNode::SIZE_CTRL = 10;

	/********************************  DC  ********************************/

	CNode::SLOT_TOTAL = CCTrace::SLOT_TRACE;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
	CNode::DEFAULT_SLOT_CARRIER_SENSE = 0; 


	ParseConfiguration(PATH_RUN + FILE_DEFAULT_CONFIG);

}

void InitLogPath()
{
	// Generate timestamp & output path

	// Create root path (../test/) if doesn't exist
	if( access(PATH_ROOT.c_str(), 00) != 0 )
		_mkdir(PATH_ROOT.c_str());

	time_t seconds;  //��ʱ��  
	char temp[65] = { '\0' };
	seconds = time(nullptr); //��ȡĿǰ��ʱ��  
	strftime(temp, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));
	TIMESTAMP = string(temp);
	strftime(temp, 64, "%Y-%m-%d-%H-%M-%S", localtime(&seconds));
	string timestring;
	timestring = string(temp);
	INFO_LOG = "@" + TIMESTAMP + TAB;
	PATH_LOG = "." + timestring + "/";

	// Create log path
	if( access(( PATH_ROOT + PATH_LOG ).c_str(), 00) != 0 )
		_mkdir(( PATH_ROOT + PATH_LOG ).c_str());

	// Hide folder
	LPWSTR wstr = CString(( PATH_ROOT + PATH_LOG ).c_str()).AllocSysString();
	int attr = GetFileAttributes(wstr);
	if( ( attr & FILE_ATTRIBUTE_HIDDEN ) == 0 )
	{
		SetFileAttributes(wstr, attr | FILE_ATTRIBUTE_HIDDEN);
	}

}

// TODO: print all significant parameters
void PrintConfiguration()
{
	ofstream parameters( PATH_ROOT + PATH_LOG + FILE_PARAMETES, ios::app);
	parameters << endl << endl << INFO_LOG << endl << endl;

	parameters << "CYCLE" << TAB << CNode::SLOT_TOTAL << endl;
	parameters << "DEFAULT_DC" << TAB << CNode::DEFAULT_DUTY_CYCLE<< endl;

	if( CData::useHOP() )
		parameters << "HOP" << TAB << CData::MAX_HOP << endl;
//	else
//		parameters << "TTL" << TAB << CData::MAX_TTL << endl;
	parameters << "DATA_RATE" << TAB << "1 / " << int( 1 / CNode::DEFAULT_DATA_RATE ) << endl;
	parameters << "DATA_SIZE" << TAB << CNode::SIZE_DATA << endl;
	parameters << "BUFFER" << TAB << CNode::CAPACITY_BUFFER << endl;
	parameters << "ENERGY" << TAB << CNode::CAPACITY_ENERGY << endl;

	parameters << "DATA_TIME" << TAB << DATATIME << endl;
	parameters << "RUN_TIME" << TAB << RUNTIME << endl;
	parameters << "PROB_TRANS" << TAB << CGeneralNode::PROB_TRANS << endl;
	parameters << "WINDOW_TRANS" << TAB << CProphet::WINDOW_TRANS << endl;


	//����ļ�Ϊ��ʱ������ļ�ͷ
	ofstream final( PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app);
	final.seekp(0, ios::end);
	if( ! final.tellp() )
		final << INFO_FINAL ;

	final << DATATIME << TAB << RUNTIME << TAB << CGeneralNode::PROB_TRANS << TAB << CNode::CAPACITY_BUFFER << TAB << CNode::CAPACITY_ENERGY << TAB ;
	if( CData::useHOP() )
		final << CData::MAX_HOP << TAB ;

	final << CNode::SLOT_TOTAL << TAB << CNode::DEFAULT_DUTY_CYCLE << TAB ;

//	if( ROUTING_PROTOCOL == _epidemic )
//	{
//		INFO_LOG += "$Epidemic ";
//		parameters << "$Epidemic " << endl << endl;
//	}
//	else 

	if( ROUTING_PROTOCOL == _prophet )
	{
		INFO_LOG += "$Prophet ";
		parameters << endl << "$Prophet " << endl << endl;
		parameters << "PRED_INIT" << TAB << CProphet::INIT_PRED << endl;
		parameters << "PRED_DECAY" << TAB << CProphet::RATIO_PRED_DECAY << endl;
		parameters << "PRED_TRANS" << TAB << CProphet::RATIO_PRED_TRANS << endl;
	}

	if( MAC_PROTOCOL == _hdc )
	{
		INFO_LOG += "$HDC ";
		parameters << endl << "$HDC " << endl << endl;
		parameters << "HOTSPOT_DC" << TAB << CNode::HOTSPOT_DUTY_CYCLE << endl;
		final << CNode::HOTSPOT_DUTY_CYCLE << TAB ;
	}

	if( HOTSPOT_SELECT != _none )
	{
		if( HOTSPOT_SELECT == _improved )
		{
			INFO_LOG += "$iHS ";
			parameters << endl << "$iHS " << endl << endl;
			parameters << "POSITION_LIFETIME" << TAB << CHotspotSelect::LIFETIME_POSITION << endl << endl;

			final << CHotspotSelect::LIFETIME_POSITION << TAB ;
		}

		else if( HOTSPOT_SELECT == _merge )
		{
			INFO_LOG += "$mHS ";
			parameters << endl << "$mHS " << endl << endl;
			parameters << "RATIO_MERGE" << TAB << CHotspotSelect::RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << CHotspotSelect::RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << CHotspotSelect::RATIO_OLD_HOTSPOT << endl;

			final << CHotspotSelect::RATIO_MERGE_HOTSPOT << TAB << CHotspotSelect::RATIO_OLD_HOTSPOT << TAB ;

		}

		else
		{
			INFO_LOG += "$HS ";
			parameters << endl << "$HS " << endl << endl;
		}

		parameters << "ALPHA" << TAB << CPostSelect::ALPHA << endl;
		//final << CPostSelect::ALPHA << TAB ;
	}

	if( ROUTING_PROTOCOL == _xhar )
	{
		INFO_LOG += "$xHAR ";
		parameters << endl << "$xHAR " << endl << endl;
		parameters << "BETA" << TAB << HAR::BETA << endl;
		parameters << "HEAT_CO_1" << TAB << HAR::CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << HAR::CO_HOTSPOT_HEAT_A2 << endl;
		//final << HAR::BETA << TAB;
	}

	if( CMacProtocol::TEST_DYNAMIC_NUM_NODE)
	{
		INFO_LOG += "#DYNAMIC_NODE_NUMBER";
		parameters << endl << "#DYNAMIC_NODE_NUMBER" << endl;
	}

	parameters << endl;

	parameters.close();
	final.close();

	// ����汾��Ϣ

	//ifstream versionInput( PATH_RUN + FILE_VERION, ios::in);
	//ofstream version( PATH_ROOT + PATH_LOG + FILE_VERION, ios::out);
	//version << versionInput.rdbuf();
	//versionInput.close();
	//version.close();

	ofstream version(PATH_ROOT + PATH_LOG + FILE_VERION, ios::out);

	version << _VERSION_MAJOR_ << "." << _VERSION_MINOR_ << "." << _VERSION_BUILD_;

	version.close();

}

bool RunSimulation()
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

//			case _epidemic:
//				dead = ! CEpidemic::Operate(currentTime);
//				break;

			case _xhar:
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

//		case _epidemic:
//			CEpidemic::PrintFinal(currentTime);
//			break;

		case _xhar:
			HAR::PrintFinal(currentTime);
			break;

		default:
			break;
	}

	return true;
}

bool Run(int argc, char* argv[])
{
	// TODO: release �汾��Ӧ��Ϊ while(1) ѭ��

	/***************************** ��ʼ��ʱ����� log �ļ��е�******************************/

	InitLogPath();

	/************************************ ����Ĭ��ֵ *************************************/

	InitConfiguration();

	/********************************** �����в������� ************************************/

	ParseConfiguration( argc - 1 , ++argv, "Command Line Configuration" );

	/*************************** �� log ��Ϣ�Ͳ�����Ϣд���ļ� ******************************/

	PrintConfiguration();

	srand(static_cast<unsigned>( time(nullptr) ));

	RunSimulation();

	Exit(EFINISH);

	_ALERT_;

	return true;
}


int main(int argc, char* argv[])
{
	return Run(argc, argv);
}