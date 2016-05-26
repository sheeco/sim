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


// TODO: move all func definition into cpp file except for inline func
// TODO: 检查所有类内静态变量，决定 private / protected
// TODO: CConfiguration / CConfigureHelper ?
// TODO: 默认配置参数改为从 XML 读取
// TODO: MulitCast ?

void InitConfiguration()
{
	SLOT = 1;
	SLOT_LOG = 100;

	/************************************** Default Config **************************************/

	CGeneralNode::RANGE_TRANS = 100;  //transmission range
	CGeneralNode::PROB_TRANS = 1.0;

	CSink::SINK_ID = 0; //0为sink节点预留，传感器节点ID从1开始
	CSink::SINK_X = 0;
	CSink::SINK_Y = 0;
	CSink::CAPACITY_BUFFER = 999999999;  //无限制

	CNode::INIT_NUM_NODE = 0;

	CRoutingProtocol::SLOT_DATA_SEND = CCTrace::SLOT_TRACE;  //数据发送slot
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

	CNode::INIT_DELIVERY_PRED = 0.70;  //参考值 0.75
	CNode::RATIO_PRED_DECAY = 0.90;  //参考值 0.98(/s)
	CNode::RATIO_PRED_TRANS = 0.20;  //参考值 0.25
	CProphet::TRANS_STRICT_BY_PRED = false;
	CProphet::CAPACITY_FORWARD = 0;


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
	CNode::CAPACITY_BUFFER = 0;
	CNode::CAPACITY_ENERGY = 0;
	CNode::LIFETIME_SPOKEN_CACHE = 0;

	CNode::SIZE_DATA = 250;  //Up to 250 Bytes
	CGeneralNode::SIZE_CTRL = 10;

	/********************************  DC  ********************************/

	CNode::SLOT_TOTAL = 10 * CCTrace::SLOT_TRACE;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
	CNode::DEFAULT_DISCOVER_CYCLE = 0; 

	/******************************  Prophet  ******************************/

	CProphet::TRANS_STRICT_BY_PRED = false;
	CProphet::CAPACITY_FORWARD = 0;

	/** Opt **/

}

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

void InitLogPath()
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

bool ParseArguments(int argc, char* argv[])
{
	// 将使用的命令行参数输出到文件
	ofstream command(PATH_ROOT + PATH_LOG + FILE_COMMAND, ios::out);
	for( int i = 0; i < argc; ++i )
		command << argv[i] << " ";
	command.close();

	try
	{
		int iField = 0;
		for(iField = 1; iField < argc; )
		{
			string field = argv[iField];

			//<mode> 不带数值的布尔型参数
			if( field == "-hdc" )
			{
				MAC_PROTOCOL = _hdc;
				HOTSPOT_SELECT = _original;
				++iField;
			}
			else if( field == "-prophet" )
			{
				ROUTING_PROTOCOL = _prophet;
				++iField;
			}
//			else if( field == "-epidemic" )
//			{
//				ROUTING_PROTOCOL = _epidemic;
//				++iField;
//			}
			else if( field == "-har" )
			{
				ROUTING_PROTOCOL = _xhar;
				HOTSPOT_SELECT = _original;
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
			else if( field == "-continuous-trace" )
			{
				CCTrace::CONTINUOUS_TRACE = true;
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

			//整型参数
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

				if( SLOT > CCTrace::SLOT_TRACE )
					SLOT = CCTrace::SLOT_TRACE;
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
//				//观测周期不应小于工作周期
//				if( SLOT_LOG < CNode::SLOT_TOTAL )
//					SLOT_LOG = CNode::SLOT_TOTAL;

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
					CHotspotSelect::LIFETIME_POSITION = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-cycle" )
			{
				if( iField < argc - 1 )
					CNode::SLOT_TOTAL = atoi( argv[ iField + 1 ] );
				//观测周期不应小于工作周期
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
//				if( ( CData::MAX_HOP > 0 )
//					&& ( CData::MAX_TTL > 0 ) )
//				{
//					string error = "Error @ ParseArguments() : Argument -hop & -ttl cannot be used both";
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
//					string error = "Error @ ParseArguments() : Argument -hop & -ttl cannot be used both";
//					cout << error << endl;
//					_PAUSE_;
//					Exit(EINVAL, error);
//				}
//
//				iField += 2;
//			}
			else if( field == "-buffer" )
			{
				if( iField < argc - 1 )
					CNode::CAPACITY_BUFFER = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-buffer-ma" )
			{
				if( iField < argc - 1 )
					CMANode::CAPACITY_BUFFER = atoi( argv[ iField + 1 ] );
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
//			else if( field == "-queue" )
//			{
//				if( iField < argc - 1 )
//					CEpidemic::MAX_DATA_RELAY = atoi( argv[ iField + 1 ] );
//				iField += 2;
//			}			
			else if( field == "-spoken" )
			{
				if( iField < argc - 1 )
					CNode::LIFETIME_SPOKEN_CACHE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-capacity-forward" )
			{
				if( iField < argc - 1 )
					CProphet::CAPACITY_FORWARD = atoi( argv[ iField + 1 ] );
				iField += 2;
			}


			//double参数
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
					CHotspotSelect::LAMBDA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-merge" )
			{
				if( iField < argc - 1 )
					CHotspotSelect::RATIO_MERGE_HOTSPOT = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-old" )
			{
				if( iField < argc - 1 )
					CHotspotSelect::RATIO_OLD_HOTSPOT = atof( argv[ iField + 1 ] );
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
			//实际上对WSN而言不会使用到
//			else if( field == "-pred-trans" )
//			{
//				if( iField < argc - 1 )
//					CNode::RATIO_PRED_TRANS = atof( argv[ iField + 1 ] );
//				iField += 2;
//			}


			//带两个或以上数值的参数
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

			//字符串参数
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


			//输出help信息
			else if( field == "-help" )
			{
				Help();
				_PAUSE_;
				Exit(ESKIP);
			}
			else
			{
				stringstream error;
				error << "Error @ ParseArguments() : Command '" << field << "' doesn't exist. ";
				cout << endl << error.str() << endl;
				Help();
				_PAUSE_;
				Exit(EINVAL, error.str());
				return false;
			}

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

	return true;

}

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
	parameters << "DATA_TRANS" << TAB << CProphet::CAPACITY_FORWARD << endl;


	//输出文件为空时，输出文件头
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
		parameters << "PRED_INIT" << TAB << CNode::INIT_DELIVERY_PRED << endl;
		parameters << "PRED_DECAY" << TAB << CNode::RATIO_PRED_DECAY << endl;
		parameters << "PRED_TRANS" << TAB << CNode::RATIO_PRED_TRANS << endl;
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
	// TODO: release 版本中应改为 while(1) 循环

	/***************************** 初始化时间戳和 log 文件夹等******************************/

	InitLogPath();

	/************************************ 参数默认值 *************************************/

	InitConfiguration();

	/********************************** 命令行参数解析 ************************************/

	ParseArguments(argc, argv);

	/*************************** 将 log 信息和参数信息写入文件 ******************************/

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