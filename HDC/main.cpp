#pragma once

#include "Preprocessor.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "GreedySelection.h"
#include "Epidemic.h"
#include "HDC.h"


/************************************ IHAR ************************************/

//IHAR: Node Repair
double LAMBDA = 0;
int MAX_MEMORY_TIME = 3600;

//IHAR: Node Number Test
int NUM_NODE = NUM_NODE_INIT;


/********************************* merge-HAR ***********************************/

//merge-HAR: 
//参数从命令行传入
double RATIO_MERGE_HOTSPOT = 1.0;
double RATIO_NEW_HOTSPOT = 1.0;
double RATIO_OLD_HOTSPOT = 1.0;
double CO_POSITION_DECAY = 1.0;

bool HEAT_RATIO_EXP = false;
bool HEAT_RATIO_LN = false;


/*********************************** HDC **************************************/

bool DO_HDC = false;


/********************************* 全局变量 ***********************************/

bool DO_IHAR = false;
bool DO_MERGE_HAR = false;
bool TEST_BALANCED_RATIO = false;
bool TEST_HOTSPOT_SIMILARITY = true;
bool TEST_DYNAMIC_NUM_NODE = false;
bool TEST_LEARN = false;
int MAX_NUM_HOTSPOT = 9999;
double MIN_POSITION_WEIGHT = 0;

double ALPHA = 0.03;  //ratio for post selection
double BETA = 0.0025;  //ratio for true hotspot
double GAMA = 0.5;  //ratio for HotspotsAboveAverage
double CO_HOTSPOT_HEAT_A1 = 1;
double CO_HOTSPOT_HEAT_A2 = 30;
int MIN_WAITING_TIME = 0;  //add minimum waiting time to each hotspot

double PROB_DATA_FORWARD = 1.0;
int DATATIME = 15300;
int RUNTIME = 20000;
int currentTime = 0;
int startTimeForHotspotSelection = SLOT_HOTSPOT_UPDATE;  //no MA node at first

string logInfo;
ofstream debugInfo("debug.txt", ios::app);

string HELP = "\n                                                  !!!!!! ALL CASE SENSITIVE !!!!!! \n"
              "<mode>            -har;                  -ihar;                  -hdc;                    -hotspot-similarity;         -dynamic-node-number; \n"
              "<time>            -time-data   [];       -time-run   []; \n"
			  "<energy>          -node-energy []; \n"
              "<har>             -alpha       [];       -beta       [];         -gama       [];          -heat   [] [];               -prob-trans []; \n"
              "<ihar>            -lambda      [];       -lifetime   []; \n"
			  "<epidemic>        -hop         [];       -ttl        [];         -queue      [];          -spoken []; \n"
              "<hdc>             -slot-total  [];       -default-dc [];         -hotspot-dc []; \n\n";


int main(int argc, char* argv[])
{
	///********************************* 命令行参数解析 ***********************************/
	try
	{
		int iField = 0;
		for(iField = 1; iField < argc; )
		{
			string field = argv[iField];

			//<mode> 不带数值的布尔型参数
			if( field == "-har" )
			{
				DO_IHAR = false;
				iField++;
			}
			else if( field == "-ihar" )
			{
				DO_IHAR = true;
				iField++;
			}
			//else if( field == "-mhar" )
			//{
			//	DO_MERGE_HAR = true;
			//	DO_IHAR = false;
			//	iField++;
			//}
			else if( field == "-hdc" )
			{
				DO_HDC = true;
				iField++;
			}
			else if( field == "-hotspot-similarity" )
			{
				TEST_HOTSPOT_SIMILARITY = true;
				iField++;
			}
			else if( field == "-dynamic-node-number" )
			{
				TEST_DYNAMIC_NUM_NODE = true;
				iField++;
			}
			//else if( field == "-balanced-ratio" )
			//{
			//	TEST_BALANCED_RATIO = true;
			//	iField++;
			//}

			//整型参数
			else if( field == "-lifetime" )
			{
				if(iField < argc - 1)
					MAX_MEMORY_TIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-time-data" )
			{
				if(iField < argc - 1)
					DATATIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-time-run" )
			{
				if(iField < argc - 1)
					RUNTIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-default-dc" )
			{
				if(iField < argc - 1)
					CNode::DEFAULT_DUTY_CYCLE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-hotspot-dc" )
			{
				if(iField < argc - 1)
					CNode::HOTSPOT_DUTY_CYCLE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-slot-total" )
			{
				if(iField < argc - 1)
					CNode::SLOT_TOTAL = atoi( argv[ iField + 1 ] );
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
			else if( field == "-node-energy" )
			{
				if(iField < argc - 1)
					CNode::ENERGY = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-queue" )
			{
				if(iField < argc - 1)
					Epidemic::MAX_QUEUE_SIZE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-spoken" )
			{
				if(iField < argc - 1)
					Epidemic::SPOKEN_MEMORY = atoi( argv[ iField + 1 ] );
				iField += 2;
			}

			//double参数
			else if( field == "-alpha" )
			{
				if(iField < argc - 1)
					ALPHA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-beta" )
			{
				if(iField < argc - 1)
					BETA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-gama" )
			{
				if(iField < argc - 1)
					GAMA = atof( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-lambda" )
			{
				if(iField < argc - 1)
					LAMBDA = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			//else if( field == "-merge" )
			//{
			//	if(iField < argc - 1)
			//		RATIO_MERGE_HOTSPOT = atof( argv[ iField + 1 ] );
			//	iField += 2;
			//}
			//else if( field == "-old" )
			//{
			//	if(iField < argc - 1)
			//		RATIO_OLD_HOTSPOT = atof( argv[ iField + 1 ] );
			//	iField += 2;
			//}
			else if( field == "-prob-trans" )
			{
				if(iField < argc - 1)
					PROB_DATA_FORWARD = atof( argv[ iField + 1 ] );
				iField += 2;
			}

			//带两个或以上数值的参数
			else if( field == "-heat" )
			{
				if(iField < argc - 2)
				{
					CO_HOTSPOT_HEAT_A1 = atof( argv[ iField + 1 ] );
					CO_HOTSPOT_HEAT_A2 = atof( argv[ iField + 2 ] );
				}
				iField += 3;
			}

			//输出help信息
			else if( field == "-help" )
			{
				cout << HELP;
				_PAUSE;
				exit(1);
			}
			else
				iField++;

		}

	}
	catch(exception e)
	{
		cout << "Error: main() Wrong Parameter Format!" << endl;
		cout << HELP;
		_PAUSE;
		exit(1);
	}

	srand( (unsigned) time(NULL) ); 
  
	/********************************* 将log信息和参数信息写入文件 ***********************************/
	if(currentTime == 0)
	{
		string logtime;  
		time_t t;  //秒时间  
		char buffer[65];
		t = time(NULL); //获取目前秒时间  
		strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
		logtime = string(buffer);
		logInfo = "\n#" + logtime + TAB;
	
		ofstream parameters("parameters.txt", ios::app);
		parameters << endl << endl << "#" << logtime << endl;

		debugInfo << ALPHA << TAB << BETA << TAB ;

		if(DO_IHAR)
		{
			logInfo += "#IHAR";
			parameters << endl;
			parameters << "#IHAR" << endl << endl;
			parameters << "LAMBDA" << TAB << LAMBDA << endl;
			parameters << "LIFETIME" << TAB << MAX_MEMORY_TIME << endl << endl;

			debugInfo << LAMBDA << TAB << MAX_MEMORY_TIME << TAB ;
		}

		//else if(DO_MERGE_HAR)
		//{
		//	logInfo += "#merge-HAR";
		//	parameters << endl;
		//	parameters << "#merge-HAR" << endl << endl;
		//	parameters << "RATIO_MERGE" << TAB << RATIO_MERGE_HOTSPOT << endl;
		//	parameters << "RATIO_NEW" << TAB << RATIO_NEW_HOTSPOT << endl;
		//	parameters << "RATIO_OLD" << TAB << RATIO_OLD_HOTSPOT << endl;

		//	debugInfo << RATIO_MERGE_HOTSPOT << TAB << RATIO_OLD_HOTSPOT << TAB ;

		//}

		else
		{
			logInfo += "#HAR";
			parameters << "#HAR" << endl << endl;
		}

		if(TEST_DYNAMIC_NUM_NODE)
		{
			logInfo += "\t#TEST_DYNAMIC_NODE_NUMBER";
			parameters << endl;
			parameters << "#TEST_DYNAMIC_NODE_NUMBER" << endl;
		}


		logInfo += "\n";
		parameters << endl;

		parameters << "ALPHA" << TAB << ALPHA << endl;
		parameters << "BETA" << TAB << BETA << endl;
		parameters << "GAMA" << TAB << GAMA << endl;
		parameters << "HEAT_CO_1" << TAB << CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << CO_HOTSPOT_HEAT_A2 << endl;

		parameters << "DEFAULT DC" << TAB << CNode::DEFAULT_DUTY_CYCLE<< endl;
		parameters << "HOTSPOT DC" << TAB << CNode::HOTSPOT_DUTY_CYCLE << endl;
		parameters << "SLOT TOTAL" << TAB << CNode::SLOT_TOTAL << endl;

		parameters << "HOP" << TAB << CData::MAX_HOP << endl;
		parameters << "TTL" << TAB << CData::MAX_TTL << endl;
		parameters << "NODE ENERGY" << TAB << CNode::ENERGY << endl;
		parameters << "MAX QUEUE SIZE" << TAB << Epidemic::MAX_QUEUE_SIZE << endl;
		parameters << "SPOKEN MEMORY" << TAB << Epidemic::SPOKEN_MEMORY << endl;

		parameters << "DATA TIME" << TAB << DATATIME << endl;
		parameters << "RUN TIME" << TAB << RUNTIME << endl;
		parameters << "PROB DATA FORWARD" << TAB << PROB_DATA_FORWARD << endl;

		parameters.close();
	}

	//HAR har;

	while(currentTime <= RUNTIME)
	{

		if( DO_HDC )
			CHDC::Operate(currentTime);

		bool dead = false;

		dead = ! Epidemic::Operate(currentTime);

		if( dead )
			break;

		currentTime += SLOT;

	}

	debugInfo.close();

}