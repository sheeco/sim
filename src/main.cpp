#pragma once

#include "SortHelper.h"
#include "PostSelect.h"
#include "Prophet.h"
#include "HDC.h"
#include "Sink.h"


//TODO: move these global config to a global CConfiguration parameter
_MAC_PROTOCOL MAC_PROTOCOL = _smac;
_ROUTING_PROTOCOL ROUTING_PROTOCOL = _prophet;
_HOTSPOT_SELECT HOTSPOT_SELECT = _original;

int DATATIME = 0;
int RUNTIME = 0;

/********************************* Usage & Output ***********************************/

string INFO_LOG;
ofstream debugInfo("debug.txt", ios::app);

string INFO_HELP = "\n                                 !!!!!! ALL CASE SENSITIVE !!!!!! \n"
 	          "<node>      -sink           [][]  -range           []   -prob-trans  []   -energy      []   -time-data   []   -time-run   [] \n"
			  "<data>      -buffer           []  -data-rate       []   -data-size   []  \n"
              "<mac>       -hdc                  -cycle           []   -dc-default  []   -dc-hotspot  [] \n"
			  "<route>     -epidemic             -prophet              -har              -hop         []   -ttl         []\n"
			  "<prophet>   -spoken           []  -queue           [] \n"
              "<hs>        -hs                   -ihs                  -mhs              -alpha       []   -beta        []   -heat      [][] \n"
              "<ihar>      -lambda           []  -lifetime        [] \n"
			  "<mhar>      -merge            []  -old             [] \n"
              "<test>      -dynamic-node-number  -hotspot-similarity   -balanced-ratio \n\n" ;

string INFO_DEBUG = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery	#Delay	#EnergyConsumption	(#NetworkTime)	(#EncounterAtHotspot)	#Log \n" ;


//TODO: 默认配置参数改为从XML读取
void initConfiguration()
{
	CSink::SINK_X = -200;
	CSink::SINK_Y = 200;
	CGeneralNode::TRANS_RANGE = 250;
	CNode::DATA_SIZE = 400;
	CNode::CTRL_SIZE = 10;
	CNode::DEFAULT_DATA_RATE = 1.0 / 150.0;
	CNode::BUFFER_CAPACITY = 200;
	CNode::SLOT_TOTAL = 10 * SLOT_MOBILITYMODEL;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
	CData::MAX_TTL = 0;	
	DATATIME = 15000;
	RUNTIME = 15000;
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
				iField++;
			}
			else if( field == "-prophet" )
			{
				ROUTING_PROTOCOL = _prophet;
				iField++;
			}
			else if( field == "-epidemic" )
			{
				ROUTING_PROTOCOL = _epidemic;
				iField++;
			}
			else if( field == "-har" )
			{
				ROUTING_PROTOCOL = _har;
				iField++;
			}
			else if( field == "-hs" )
			{
				HOTSPOT_SELECT = _original;
				iField++;
			}
			else if( field == "-ihs" )
			{
				HOTSPOT_SELECT = _improved;
				iField++;
			}
			else if( field == "-mhs" )
			{
				HOTSPOT_SELECT = _merge;
				iField++;
			}
			else if( field == "-hotspot-similarity" )
			{
				CRoutingProtocol::TEST_HOTSPOT_SIMILARITY = true;
				iField++;
			}
			else if( field == "-dynamic-node-number" )
			{
				CRoutingProtocol::TEST_DYNAMIC_NUM_NODE = true;
				iField++;
			}
			else if( field == "-balanced-ratio" )
			{
				HAR::TEST_BALANCED_RATIO = true;
				iField++;
			}

			//整型参数
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
			else if( field == "-cycle" )
			{
				if(iField < argc - 1)
					CNode::SLOT_TOTAL = atoi( argv[ iField + 1 ] );
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
					CEpidemic::SPOKEN_MEMORY = atoi( argv[ iField + 1 ] );
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


			//输出help信息
			else if( field == "-help" )
			{
				cout << INFO_HELP;
				_PAUSE_;
				exit(1);
			}
			else
				iField++;

		}

		return true;
	}
	catch(exception e)
	{
		cout << endl << "Error @ ParseParameters() : Wrong Parameter Format!" << endl;
		cout << INFO_HELP;
		return false;
	}
}

void PrintConfiguration()
{
	string logtime;  
	time_t t;  //秒时间  
	char buffer[65];
	t = time(nullptr); //获取目前秒时间  
	strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
	logtime = string(buffer);
	INFO_LOG = "\n#" + logtime + TAB;

	ofstream parameters("parameters.txt", ios::app);
	parameters << endl << endl << "#" << logtime << endl << endl;

	parameters << "SLOT TOTAL" << TAB << CNode::SLOT_TOTAL << endl;
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
	debugInfo.seekp(0, ios::end);
	if( ! debugInfo.tellp() )
		debugInfo << INFO_DEBUG ;

	debugInfo << DATATIME << TAB << RUNTIME << TAB << CGeneralNode::PROB_DATA_FORWARD << TAB << CNode::BUFFER_CAPACITY << TAB << CNode::ENERGY << TAB ;
	if( CData::useHOP() )
		debugInfo << CData::MAX_HOP << TAB ;
	else 
		debugInfo << CData::MAX_TTL << TAB ;

	debugInfo << CNode::SLOT_TOTAL << TAB << CNode::DEFAULT_DUTY_CYCLE << TAB ;

	parameters << endl;
	if( ROUTING_PROTOCOL == _epidemic )
	{
		INFO_LOG += "-Epidemic ";
		parameters << "-Epidemic ";
	}
	else if( ROUTING_PROTOCOL == _prophet )
	{
		INFO_LOG += "-Prophet ";
		parameters << "-Prophet ";
	}

	if( MAC_PROTOCOL == _hdc )
	{
		INFO_LOG += "-HDC ";
		parameters << "-HDC ";
	}

	if( MAC_PROTOCOL == _hdc || ROUTING_PROTOCOL == _har )
	{
		if( HOTSPOT_SELECT == _improved )
		{
			INFO_LOG += "-IHAR ";
			parameters << "-IHAR " << endl << endl;
			parameters << "LIFETIME" << TAB << HAR::MAX_MEMORY_TIME << endl << endl;

			debugInfo << CNode::HOTSPOT_DUTY_CYCLE << TAB << CPostSelect::ALPHA << TAB << HAR::BETA << TAB << HAR::MAX_MEMORY_TIME << TAB ;
		}

		else if( HOTSPOT_SELECT == _merge )
		{
			INFO_LOG += "-mHAR ";
			parameters << "-mHAR " << endl << endl;
			parameters << "RATIO_MERGE" << TAB << CHotspot::RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << CHotspot::RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << CHotspot::RATIO_OLD_HOTSPOT << endl;

			debugInfo << CHotspot::RATIO_MERGE_HOTSPOT << TAB << CHotspot::RATIO_OLD_HOTSPOT << TAB ;

		}

		else
		{
			debugInfo << CNode::HOTSPOT_DUTY_CYCLE << TAB << CPostSelect::ALPHA << TAB << HAR::BETA << TAB ;
			INFO_LOG += "-HAR ";
			parameters << "-HAR " << endl << endl;
		}

		parameters << "ALPHA" << TAB << CPostSelect::ALPHA << endl;
		parameters << "BETA" << TAB << HAR::BETA << endl;
		parameters << "HEAT_CO_1" << TAB << HAR::CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << HAR::CO_HOTSPOT_HEAT_A2 << endl;
	}
	//debugInfo.flush();

	if( CRoutingProtocol::TEST_DYNAMIC_NUM_NODE)
	{
		INFO_LOG += "-TEST_DYNAMIC_NODE_NUMBER";
		parameters << endl;
		parameters << "-TEST_DYNAMIC_NODE_NUMBER" << endl;
	}

	INFO_LOG += "\n";
	parameters << endl;

	parameters.close();


}

bool Run()
{
	int currentTime = 0;
	while(currentTime <= RUNTIME)
	{

		if( MAC_PROTOCOL == _hdc )
			CHDC::Operate(currentTime);

		bool dead = false;

		dead = ! CProphet::Operate(currentTime);

		if( dead )
		{
			RUNTIME = currentTime;
			CHDC::PrintInfo(currentTime);
			CProphet::PrintInfo(currentTime);
			break;
		}

		currentTime += SLOT;

	}

	debugInfo.close();

	return true;
}

int main(int argc, char* argv[])
{
	/************************************ 参数默认值 *************************************/

	initConfiguration();

	/************************************ 命令行参数解析 *************************************/

	if( ! ParseParameters(argc, argv) )
	{
		_PAUSE_;
		exit(-1);
	}

	/********************************* 将log信息和参数信息写入文件 ***********************************/
	
	PrintConfiguration();

	srand( static_cast<unsigned>(time(nullptr)) ); 

	Run();

	_ALERT_;
}