#pragma once

#include "GlobalParameters.h"
#include "SortHelper.h"
#include "PostSelect.h"
#include "Prophet.h"
#include "HDC.h"
#include "Sink.h"
#include "MANode.h"
#include "Epidemic.h"


// TODO: move all func definition into cpp file except for inline func
// TODO: 检查所有类内静态变量，决定 private / protected
// TODO: CConfiguration / CConfigureHelper ?
// TODO: 默认配置参数改为从 XML 读取
// TODO: MulitCast ?

void InitConfiguration()
{
	/************************************** Default Config **************************************/

	CGeneralNode::TRANS_RANGE = 100;  //transmission range
	CGeneralNode::PROB_DATA_FORWARD = 1.0;

	CSink::SINK_ID = 0; //0为sink节点预留，传感器节点ID从1开始
	CSink::SINK_X = 0;
	CSink::SINK_Y = 0;
	CSink::BUFFER_CAPACITY = 999999999;  //无限制

	CNode::NUM_NODE_INIT = 0;

	CRoutingProtocol::SLOT_DATA_SEND = SLOT_MOBILITYMODEL;  //数据发送slot
	CNode::DEFAULT_DATA_RATE = 0;
	CNode::DATA_SIZE = 0;
	CGeneralNode::CTRL_SIZE = 0;
	CMacProtocol::MAC_SIZE = 8;  //Mac Header Size

	CNode::BUFFER_CAPACITY = 0;
	CNode::ENERGY = 0;
	CNode::SPOKEN_MEMORY = 0;
	CNode::RECEIVE_MODE = CGeneralNode::_loose;
	CNode::SEND_MODE = CGeneralNode::_dump;
	CNode::QUEUE_MODE = CGeneralNode::_fifo;

	CNode::SLOT_TOTAL = 0;
	CNode::DEFAULT_DUTY_CYCLE = 0;
	CNode::HOTSPOT_DUTY_CYCLE = 0; 
	CNode::DEFAULT_SLOT_DISCOVER = 0; 

	/********** Dynamic Node Number **********/
	CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
	CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //动态节点个数测试时，节点个数发生变化的周期
	CNode::NUM_NODE_MIN = 0;
	CNode::NUM_NODE_MAX = 0;
	/********** ------------------- **********/

	/*********************************  HAR  *******************************/

	CMANode::SPEED = 30;
	CMANode::BUFFER_CAPACITY = 100;
	CMANode::RECEIVE_MODE = CGeneralNode::_selfish;

	/******************************  Epidemic  *****************************/

	CData::MAX_HOP = 0;
	CData::MAX_TTL = 0;

	CEpidemic::MAX_QUEUE_SIZE = -1;

	/******************************  Prophet  ******************************/

	CNode::INIT_DELIVERY_PRED = 0.70;  //0.75
	CNode::DECAY_RATIO = 0.90;  //0.98(/s)
	CNode::TRANS_RATIO = 0.20;  //0.25
	CProphet::MAX_DATA_TRANS = 0;

#ifdef USE_PRED_TOLERANCE

	CProphet::TOLERANCE_PRED = 0;
//	DECAY_RATIO_TOLERANCE_PRED = 1;

#endif

	/**************************  Hotspot Select  ***************************/

	CHotspot::SLOT_POSITION_UPDATE = 100;  //地理信息收集的slot
	CHotspot::SLOT_HOTSPOT_UPDATE = 900;  //更新热点和分类的slot
	CHotspot::TIME_HOSPOT_SELECT_START = CHotspot::SLOT_HOTSPOT_UPDATE;  //no MA node at first

	CPostSelect::ALPHA = 0.03;  //ratio for post selection
	HAR::BETA = 0.0025;  //ratio for true hotspot
	//HAR::GAMMA = 0.5;  //ratio for HotspotsAboveAverage
	HAR::CO_HOTSPOT_HEAT_A1 = 1;
	HAR::CO_HOTSPOT_HEAT_A2 = 30;

	/******************************* IHAR **********************************/

	HAR::LAMBDA = 0;
	HAR::MAX_MEMORY_TIME = 3600;

	/***************************** merge-HAR *******************************/

	CRoutingProtocol::TEST_HOTSPOT_SIMILARITY = true;
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
	CGeneralNode::TRANS_RANGE = 250;
	CSink::SINK_X = -200;  //for KAIST
	CSink::SINK_Y = 200;
	CNode::NUM_NODE_INIT = 29;
	CNode::NUM_NODE_MIN = CNode::NUM_NODE_INIT - 5;
	CNode::NUM_NODE_MAX = CNode::NUM_NODE_INIT + 5;
	/*********** ----------------- ***********/

	CGeneralNode::PROB_DATA_FORWARD = 1.0;

	CNode::DEFAULT_DATA_RATE = 1.0 / 30.0;
	CNode::BUFFER_CAPACITY = 200;
	CNode::ENERGY = 0;
	CNode::SPOKEN_MEMORY = 0;

	CNode::DATA_SIZE = 250;  //Up to 250 Bytes
	CGeneralNode::CTRL_SIZE = 10;

	CNode::SLOT_TOTAL = 10 * SLOT_MOBILITYMODEL;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
	CNode::DEFAULT_SLOT_DISCOVER = 10; 

	/** Opt **/
#ifdef USE_PRED_TOLERANCE

	CProphet::TOLERANCE_PRED = 0.2;
//	DECAY_RATIO_TOLERANCE_PRED = 1;

#endif

}

void Help()
{
	cout << INFO_HELP << endl;
	ofstream help(FILE_HELP, ios::out);
	help << INFO_HELP;
	help.close();
}

bool ParseParameters(int argc, char* argv[])
{
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
				CRoutingProtocol::TEST_HOTSPOT_SIMILARITY = true;
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

			//整型参数
			else if( field == "-time-data" )
			{
				if(iField < argc - 1)
					DATATIME = atoi( argv[ iField + 1 ] );
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			else if( field == "-time-run" )
			{
				if(iField < argc - 1)
					RUNTIME = atoi( argv[ iField + 1 ] );
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			else if( field == "-slot" )
			{
				if(iField < argc - 1)
					SLOT = atoi( argv[ iField + 1 ] );

				if( SLOT > SLOT_MOBILITYMODEL )
					SLOT = SLOT_MOBILITYMODEL;
				iField += 2;
			}
			else if( field == "-log-slot" )
			{
				if(iField < argc - 1)
					SLOT_LOG = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-range" )
			{
				if(iField < argc - 1)
					CGeneralNode::TRANS_RANGE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-lifetime" )
			{
				if(iField < argc - 1)
					HAR::MAX_MEMORY_TIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-cycle" )
			{
				if(iField < argc - 1)
					CNode::SLOT_TOTAL = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-discover" )
			{
				if(iField < argc - 1)
					CNode::DEFAULT_SLOT_DISCOVER = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-dc-default" )
			{
				if(iField < argc - 1)
					CNode::DEFAULT_DUTY_CYCLE = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-dc-hotspot" )
			{
				if(iField < argc - 1)
					CNode::HOTSPOT_DUTY_CYCLE = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-hop" )
			{
				if(iField < argc - 1)
					CData::MAX_HOP = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-ttl" )
			{
				if(iField < argc - 1)
					CData::MAX_TTL = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-buffer" )
			{
				if(iField < argc - 1)
					CNode::BUFFER_CAPACITY = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-data-rate" )
			{
				if(iField < argc - 1)
					CNode::DEFAULT_DATA_RATE = double(1) / atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-data-size" )
			{
				if(iField < argc - 1)
					CNode::DATA_SIZE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-energy" )
			{
				if(iField < argc - 1)
					CNode::ENERGY = 1000 * atoi( argv[ iField + 1 ] );
				iField += 2;

				if( CNode::finiteEnergy() )
					RUNTIME = DATATIME = 999999;
			}
			else if( field == "-queue" )
			{
				if(iField < argc - 1)
					CEpidemic::MAX_QUEUE_SIZE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-spoken" )
			{
				if(iField < argc - 1)
					CNode::SPOKEN_MEMORY = atoi( argv[ iField + 1 ] );
				iField += 2;
			}


			//double参数
			else if( field == "-alpha" )
			{
				if(iField < argc - 1)
					CPostSelect::ALPHA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-beta" )
			{
				if(iField < argc - 1)
					HAR::BETA = atof( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-lambda" )
			{
				if(iField < argc - 1)
					HAR::LAMBDA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-merge" )
			{
				if(iField < argc - 1)
					CHotspot::RATIO_MERGE_HOTSPOT = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-old" )
			{
				if(iField < argc - 1)
					CHotspot::RATIO_OLD_HOTSPOT = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-prob-trans" )
			{
				if(iField < argc - 1)
					CGeneralNode::PROB_DATA_FORWARD = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-pred-tolerance" )
			{
#ifdef USE_PRED_TOLERANCE
				if(iField < argc - 1)
					CProphet::TOLERANCE_PRED = atof( argv[ iField + 1 ] );
				iField += 2;
#endif
			}

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
					DATASET = string( _strupr( arg ) );
				}
				iField += 2;
			}			
			else if( field == "-log-path" )
			{
				if(iField < argc - 2)
				{
					char arg[20] = {'\0'};
					strcpy(arg, argv[ iField + 1 ]);
					PATH_ROOT = "../" + string( _strupr( arg ) ) + "/";
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
				++iField;

		}
	}
	catch(exception e)
	{
		cout << endl << "Error @ ParseParameters() : Wrong Parameter Format!" << endl;
		Help();
		_PAUSE_;
		Exit(EINVAL);
		return false;
	}

	// Generate timestamp & output path

	// Create root path (../test/) if doesn't exist
	if( access(PATH_ROOT.c_str(), 00) != 0 )
		_mkdir(PATH_ROOT.c_str());

	time_t seconds;  //秒时间  
	char temp[65] = {'\0'};
	seconds = time(nullptr); //获取目前秒时间  
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
	parameters << "DEFAULT DC" << TAB << CNode::DEFAULT_DUTY_CYCLE<< endl;
	if( MAC_PROTOCOL == _hdc )
		parameters << "HOTSPOT DC" << TAB << CNode::HOTSPOT_DUTY_CYCLE << endl;

	if( CData::useHOP() )
		parameters << "HOP" << TAB << CData::MAX_HOP << endl;
	else
		parameters << "TTL" << TAB << CData::MAX_TTL << endl;
	parameters << "DATA RATE" << TAB << "1 / " << int( 1 / CNode::DEFAULT_DATA_RATE ) << endl;
	parameters << "DATA SIZE" << TAB << CNode::DATA_SIZE << endl;
	parameters << "BUFFER CAPACITY" << TAB << CNode::BUFFER_CAPACITY << endl;
	parameters << "NODE ENERGY" << TAB << CNode::ENERGY << endl;

	parameters << "DATA TIME" << TAB << DATATIME << endl;
	parameters << "RUN TIME" << TAB << RUNTIME << endl;
	parameters << "PROB DATA FORWARD" << TAB << CGeneralNode::PROB_DATA_FORWARD << endl;

	//输出文件为空时，输出文件头
	ofstream final( PATH_ROOT + PATH_LOG + FILE_FINAL, ios::app);
	final.seekp(0, ios::end);
	if( ! final.tellp() )
		final << INFO_FINAL ;

	final << DATATIME << TAB << RUNTIME << TAB << CGeneralNode::PROB_DATA_FORWARD << TAB << CNode::BUFFER_CAPACITY << TAB << CNode::ENERGY << TAB ;
	if( CData::useHOP() )
		final << CData::MAX_HOP << TAB ;
	else 
		final << CData::MAX_TTL << TAB ;

	final << CNode::SLOT_TOTAL << TAB << CNode::DEFAULT_DUTY_CYCLE << TAB ;

	parameters << endl;
	if( ROUTING_PROTOCOL == _epidemic )
	{
		INFO_LOG += "$Epidemic ";
		parameters << "$Epidemic ";
	}
	else if( ROUTING_PROTOCOL == _prophet )
	{
		INFO_LOG += "$Prophet ";
		parameters << "$Prophet ";
	}

	if( MAC_PROTOCOL == _hdc )
	{
		INFO_LOG += "$HDC ";
		parameters << "$HDC ";
	}

	if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
	{
		if( HOTSPOT_SELECT == _improved )
		{
			INFO_LOG += "$IHAR ";
			parameters << "$IHAR " << endl << endl;
			parameters << "LIFETIME" << TAB << HAR::MAX_MEMORY_TIME << endl << endl;

			final << HAR::MAX_MEMORY_TIME << TAB ;
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
	// TODO: release 版本中应改为 while(1) 循环

	/************************************ 参数默认值 *************************************/

	InitConfiguration();

	/************************************ 命令行参数解析 *************************************/

	ParseParameters(argc, argv);

	/********************************* 将log信息和参数信息写入文件 ***********************************/
	
	PrintConfiguration();

	srand( static_cast<unsigned>(time(nullptr)) ); 

	Run();

	Exit(EFINISH);

	_ALERT_;
}