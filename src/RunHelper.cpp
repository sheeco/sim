#include "RunHelper.h"
#include "Configuration.h"
#include "FileHelper.h"
#include "Prophet.h"
#include "HAR.h"
#include "HDC.h"
#include "../.project/version.h"


void CRunHelper::InitLogPath()
{
	// Generate timestamp & output path

	// Create root path (../test/) if doesn't exist
	if( access(PATH_ROOT.c_str(), 00) != 0 )
		_mkdir(PATH_ROOT.c_str());

	time_t seconds;  //秒时间  
	char temp[65] = { '\0' };
	seconds = time(nullptr); //获取目前秒时间  
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

void CRunHelper::Help()
{
	ifstream help(PATH_RUN + FILE_HELP, ios::in);
	cout << help.rdbuf();
	help.close();
	_PAUSE_;
}

void CRunHelper::InitConfiguration()
{
	SLOT = 1;
	SLOT_LOG = 100;

	/************************************** Default Config **************************************/

	CGeneralNode::RANGE_TRANS = 100;
	CNode::SPEED_TRANS = 2500;
	CGeneralNode::PROB_TRANS = 1.0;

	CSink::SINK_ID = 0; //0为sink节点预留，传感器节点ID从1开始
	CSink::SINK_X = 0;
	CSink::SINK_Y = 0;
	CSink::CAPACITY_BUFFER = 999999999;  //无限制

	CNode::INIT_NUM_NODE = 0;

	//CRoutingProtocol::SLOT_DATA_SEND = CCTrace::SLOT_TRACE;  //数据发送slot
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
	CMacProtocol::SYNC_DC = true;

	/********** Dynamic Node Number **********/
	CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
	CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspotSelect::SLOT_HOTSPOT_UPDATE;  //动态节点个数测试时，节点个数发生变化的周期
	CNode::INIT_NUM_NODE = 0;
	CNode::MIN_NUM_NODE = 0;
	CNode::MAX_NUM_NODE = 0;
	/********** ------------------- **********/

	/*********************************  HAR  *******************************/

	CMANode::START_COUNT_ID = 100;  //ID的起始值，用于和传感器节点相区分
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

	CProphet::INIT_PRED = 0.70;  //参考值 0.75
	CProphet::RATIO_PRED_DECAY = 0.90;  //参考值 0.98(/s)
	CProphet::RATIO_PRED_TRANS = 0.20;  //参考值 0.25
	CProphet::TRANS_STRICT_BY_PRED = true;
	CProphet::WINDOW_TRANS = 10;


	/**************************  Hotspot Select  ***************************/

	CHotspotSelect::SLOT_POSITION_UPDATE = 100;  //地理信息收集的slot
	CHotspotSelect::SLOT_HOTSPOT_UPDATE = 900;  //更新热点和分类的slot
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
	CCTrace::CONTINUOUS_TRACE = true;
	DATASET = "KAIST";
	//CGeneralNode::RANGE_TRANS = 250;
	CSink::SINK_X = -200;  //for KAIST
	CSink::SINK_Y = 200;
	CNode::INIT_NUM_NODE = 29;
	CNode::MIN_NUM_NODE = int(CNode::INIT_NUM_NODE * 1.4);
	CNode::MAX_NUM_NODE = int(CNode::INIT_NUM_NODE * 0.6);
	/*********** ----------------- ***********/

	/***************************  Node & Data  ***************************/

	CGeneralNode::PROB_TRANS = 1.0;

	CNode::DEFAULT_DATA_RATE = 6.8;  // 200 Byte / 30 s
	CNode::CAPACITY_BUFFER = 0;
	CNode::CAPACITY_ENERGY = 0;
	CNode::LIFETIME_SPOKEN_CACHE = 0;

	CNode::SIZE_DATA = 200;  //Up to 250 Bytes
	CGeneralNode::SIZE_CTRL = 10;

	/********************************  DC  ********************************/

	CNode::SLOT_TOTAL = CCTrace::SLOT_TRACE;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
	CNode::DEFAULT_SLOT_CARRIER_SENSE = 0;


	/*********************************************  添加命令格式定义  *********************************************/

	//不带参数的命令
	CConfiguration::AddConfiguration("--hdc", &MAC_PROTOCOL, _hdc, "");
	CConfiguration::AddConfiguration("--prophet", &ROUTING_PROTOCOL, _prophet, "");
	CConfiguration::AddConfiguration("--har", &ROUTING_PROTOCOL, _xhar, "");
	CConfiguration::AddConfiguration("--hs", &HOTSPOT_SELECT, _original, "");
	CConfiguration::AddConfiguration("--ihs", &HOTSPOT_SELECT, _improved, "");
	CConfiguration::AddConfiguration("--mhs", &HOTSPOT_SELECT, _merge, "");

	//bool 参数
	CConfiguration::AddConfiguration("-continuous-trace", CConfiguration::_bool, &CCTrace::CONTINUOUS_TRACE, "");
	CConfiguration::AddConfiguration("-dc-sync", CConfiguration::_bool, &CMacProtocol::SYNC_DC, "");
	CConfiguration::AddConfiguration("-dynamic-node-number", CConfiguration::_bool, &CMacProtocol::TEST_DYNAMIC_NUM_NODE, "");
	CConfiguration::AddConfiguration("-pred-strict", CConfiguration::_bool, &CProphet::TRANS_STRICT_BY_PRED, "");
	CConfiguration::AddConfiguration("-hotspot-similarity", CConfiguration::_bool, &CHotspotSelect::TEST_HOTSPOT_SIMILARITY, "");
	CConfiguration::AddConfiguration("-balanced-ratio", CConfiguration::_bool, &HAR::TEST_BALANCED_RATIO, "");
	CConfiguration::AddConfiguration("-time-data", CConfiguration::_int, &DATATIME, "");
	CConfiguration::AddConfiguration("-time-run", CConfiguration::_int, &RUNTIME, "");
	CConfiguration::AddConfiguration("-slot", CConfiguration::_int, &SLOT, "");
	CConfiguration::AddConfiguration("-node", CConfiguration::_int, &CNode::INIT_NUM_NODE, "");
	CConfiguration::AddConfiguration("-log-slot", CConfiguration::_int, &SLOT_LOG, "");
	CConfiguration::AddConfiguration("-trans-range", CConfiguration::_int, &CGeneralNode::RANGE_TRANS, "");
	CConfiguration::AddConfiguration("-trans-speed", CConfiguration::_int, &CNode::SPEED_TRANS, "");
	CConfiguration::AddConfiguration("-lifetime", CConfiguration::_int, &CHotspotSelect::LIFETIME_POSITION, "");
	CConfiguration::AddConfiguration("-cycle", CConfiguration::_int, &CNode::SLOT_TOTAL, "");
	CConfiguration::AddConfiguration("-slot-carrier-sense", CConfiguration::_int, &CNode::DEFAULT_SLOT_CARRIER_SENSE, "");
	//CConfiguration::AddConfiguration("-ttl", CConfiguration::_int, &CData::MAX_TTL, "");
	CConfiguration::AddConfiguration("-hop", CConfiguration::_int, &CData::MAX_HOP, "");
	CConfiguration::AddConfiguration("-buffer", CConfiguration::_int, &CNode::CAPACITY_BUFFER, "");
	CConfiguration::AddConfiguration("-buffer-ma", CConfiguration::_int, &CMANode::CAPACITY_BUFFER, "");
	CConfiguration::AddConfiguration("-data-rate", CConfiguration::_int, &CNode::DEFAULT_DATA_RATE, "");  // TODO: 
	CConfiguration::AddConfiguration("-data-size", CConfiguration::_int, &CNode::SIZE_DATA, "");
	//CConfiguration::AddConfiguration("-queue", CConfiguration::_int, &CEpidemic::MAX_DATA_RELAY, "");
	CConfiguration::AddConfiguration("-energy", CConfiguration::_int, &CNode::CAPACITY_ENERGY, "");
	CConfiguration::AddConfiguration("-spoken", CConfiguration::_int, &CNode::LIFETIME_SPOKEN_CACHE, "");
	CConfiguration::AddConfiguration("-trans-window", CConfiguration::_int, &CRoutingProtocol::WINDOW_TRANS, "");

	//double 参数
	CConfiguration::AddConfiguration("-dc-default", CConfiguration::_double, &CNode::DEFAULT_DUTY_CYCLE, "");
	CConfiguration::AddConfiguration("-dc-hotspot", CConfiguration::_double, &CNode::HOTSPOT_DUTY_CYCLE, "");
	CConfiguration::AddConfiguration("-alpha", CConfiguration::_double, &CPostSelect::ALPHA, "");
	CConfiguration::AddConfiguration("-beta", CConfiguration::_double, &HAR::BETA, "");
	CConfiguration::AddConfiguration("-lambda", CConfiguration::_double, &CHotspotSelect::LAMBDA, "");
	CConfiguration::AddConfiguration("-merge", CConfiguration::_double, &CHotspotSelect::RATIO_MERGE_HOTSPOT, "");
	CConfiguration::AddConfiguration("-old", CConfiguration::_double, &CHotspotSelect::RATIO_OLD_HOTSPOT, "");
	CConfiguration::AddConfiguration("-trans-prob", CConfiguration::_double, &CGeneralNode::PROB_TRANS, "");
	CConfiguration::AddConfiguration("-pred-init", CConfiguration::_double, &CProphet::INIT_PRED, "");
	CConfiguration::AddConfiguration("-pred-decay", CConfiguration::_double, &CProphet::RATIO_PRED_DECAY, "");

	//string 参数
	CConfiguration::AddConfiguration("-dataset", CConfiguration::_string, &DATASET, "");
	// PATH_ROOT = "../" + string(arg) + "/";
	CConfiguration::AddConfiguration("-log-path", CConfiguration::_string, &PATH_ROOT, "");

	//带双参数的命令
	CConfiguration::AddConfiguration("-sink", CConfiguration::_double, &CSink::SINK_X, CConfiguration::_double, &CSink::SINK_Y, "");
	CConfiguration::AddConfiguration("-heat", CConfiguration::_double, &HAR::CO_HOTSPOT_HEAT_A1, CConfiguration::_double, &HAR::CO_HOTSPOT_HEAT_A2, "");


	/*********************************************  按照命令格式解析参数配置  *********************************************/

	CConfiguration::ParseConfiguration(PATH_RUN + FILE_DEFAULT_CONFIG);


	/*********************************************  检验和修整相互耦合的参数  *********************************************/

	if( CNode::finiteEnergy() )
		RUNTIME = DATATIME = 999999;

	//if( ( CData::MAX_HOP > 0 )
	//   && ( CData::MAX_TTL > 0 ) )
	//{
	//	throw pair<int, string>(EINVAL, string("CConfiguration::ParseConfiguration() : Argument -hop & -ttl cannot be used both") );
	//}

}

// TODO: print all significant parameters
void CRunHelper::PrintConfiguration()
{
	ofstream parameters(PATH_ROOT + PATH_LOG + FILE_PARAMETES, ios::app);
	parameters << endl << endl << INFO_LOG << endl << endl;

	parameters << "CYCLE" << TAB << CNode::SLOT_TOTAL << endl;
	parameters << "DEFAULT_DC" << TAB << CNode::DEFAULT_DUTY_CYCLE << endl;

	if( CData::useHOP() )
		parameters << "HOP" << TAB << CData::MAX_HOP << endl;
	//	else
	//		parameters << "TTL" << TAB << CData::MAX_TTL << endl;
	parameters << "DATA_RATE" << TAB << "1 / " << int(1 / CNode::DEFAULT_DATA_RATE) << endl;
	parameters << "DATA_SIZE" << TAB << CNode::SIZE_DATA << endl;
	parameters << "BUFFER" << TAB << CNode::CAPACITY_BUFFER << endl;
	parameters << "ENERGY" << TAB << CNode::CAPACITY_ENERGY << endl;

	parameters << "DATA_TIME" << TAB << DATATIME << endl;
	parameters << "RUN_TIME" << TAB << RUNTIME << endl;
	parameters << "PROB_TRANS" << TAB << CGeneralNode::PROB_TRANS << endl;
	parameters << "WINDOW_TRANS" << TAB << CProphet::WINDOW_TRANS << endl;


	//输出文件为空时，输出文件头
	ofstream final(PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app);
	final.seekp(0, ios::end);
	if( !final.tellp() )
		final << INFO_FINAL;

	final << DATATIME << TAB << RUNTIME << TAB << CGeneralNode::PROB_TRANS << TAB << CNode::CAPACITY_BUFFER << TAB << CNode::CAPACITY_ENERGY << TAB;
	if( CData::useHOP() )
		final << CData::MAX_HOP << TAB;

	final << CNode::SLOT_TOTAL << TAB << CNode::DEFAULT_DUTY_CYCLE << TAB;

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
		final << CNode::HOTSPOT_DUTY_CYCLE << TAB;
	}

	if( HOTSPOT_SELECT != _none )
	{
		if( HOTSPOT_SELECT == _improved )
		{
			INFO_LOG += "$iHS ";
			parameters << endl << "$iHS " << endl << endl;
			parameters << "POSITION_LIFETIME" << TAB << CHotspotSelect::LIFETIME_POSITION << endl << endl;

			final << CHotspotSelect::LIFETIME_POSITION << TAB;
		}

		else if( HOTSPOT_SELECT == _merge )
		{
			INFO_LOG += "$mHS ";
			parameters << endl << "$mHS " << endl << endl;
			parameters << "RATIO_MERGE" << TAB << CHotspotSelect::RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << CHotspotSelect::RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << CHotspotSelect::RATIO_OLD_HOTSPOT << endl;

			final << CHotspotSelect::RATIO_MERGE_HOTSPOT << TAB << CHotspotSelect::RATIO_OLD_HOTSPOT << TAB;

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

	if( CMacProtocol::TEST_DYNAMIC_NUM_NODE )
	{
		INFO_LOG += "#DYNAMIC_NODE_NUMBER";
		parameters << endl << "#DYNAMIC_NODE_NUMBER" << endl;
	}

	parameters << endl;

	parameters.close();
	final.close();

	// 输出版本信息

	//ifstream versionInput( PATH_RUN + FILE_VERION, ios::in);
	//ofstream version( PATH_ROOT + PATH_LOG + FILE_VERION, ios::out);
	//version << versionInput.rdbuf();
	//versionInput.close();
	//version.close();

	ofstream version(PATH_ROOT + PATH_LOG + FILE_VERION, ios::out);

	version << _VERSION_MAJOR_ << "." << _VERSION_MINOR_ << "." << _VERSION_BUILD_;

	version.close();

}

bool CRunHelper::Simulation(vector<string> args)
{
	PrepareSimulation(args);
	RunSimulation();

	return true;
}

bool CRunHelper::PrepareSimulation(vector<string> args)
{
	/***************************** 初始化时间戳和 log 文件夹等******************************/

	InitLogPath();

	/************************************ 参数默认值 *************************************/

	InitConfiguration();

	/********************************** 命令行参数解析 ************************************/

	CConfiguration::ParseConfiguration(args, "Command Line Configuration");

	/*************************** 将 log 信息和参数信息写入文件 ******************************/

	PrintConfiguration();

	return true;
}

bool CRunHelper::RunSimulation()
{
	int currentTime = 0;
	while( currentTime <= RUNTIME )
	{
		bool dead = false;

		switch( ROUTING_PROTOCOL )
		{
			case _prophet:
				dead = !CProphet::Operate(currentTime);
				break;

				//			case _epidemic:
				//				dead = ! CEpidemic::Operate(currentTime);
				//				break;

			case _xhar:
				dead = !HAR::Operate(currentTime);
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

bool CRunHelper::Run(int argc, char* argv[])
{
	// TODO: release 版本中应改为 while(1) 循环

	srand(static_cast<unsigned>( time(nullptr) ));

	try
	{
		vector<string> args = CConfiguration::getConfiguration(argc - 1, ( argv + 1 ));

		Simulation(args);

	}
	catch(string error)
	{
		cout << "Error @ " << error << endl;
		Exit(EERROR, error);
	}
	catch(pair<int, string> &pairError)
	{
		cout << "Error " << pairError.first << " @ " << pairError.second << endl;
		Exit(pairError.first, pairError.second);
	}
	catch(exception ex)
	{
		cout << "Uncaught Error : " << ex.what() << endl;
		Exit(EERROR, ex.what() );
	}

	Exit(EFINISH);

	return true;
}

