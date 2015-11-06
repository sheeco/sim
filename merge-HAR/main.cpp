#pragma once

#include "Preprocessor.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "GA.h"
#include "GreedySelection.h"
#include "HAR.h"

/************************************* GA **************************************/

int POPULATION_SIZE = 100;
int MAX_UNIMPROVED_NUM = 30;
int MAX_SOLUTION_NUM = 5000;
double PROB_CROSSOVER = 0.99;  //交叉概率
double PROB_MUTATION = 1.0;  //变异概率

int CO_MUTATION_FINAL = 10;
int CO_MUTATION_CHILDREN = ROUND( MAX_SOLUTION_NUM / 3 );
double CO_MUTATION_GRADIENT = 0.1;

//存储上一次更新Hotspot时的旧参数，用于内存释放
int g_old_nPositions = 0;
int g_old_nHotspots = 0;

//cover矩阵和度数list，用于SCP
int** g_coverMatrix = NULL;
int* g_degreeForPositions = NULL;
int* g_degreeForHotspots = NULL;

//排除孤立的position以提高GA优化效率，降低复杂度（未采用）
vector<CPosition *> g_tmpPositions;  //存放GA的preprocess过程中删除的position，在当前GA过程结束之后需要放回g_positions中


/************************************ IHAR ************************************/

//IHAR: Node Repair
double LAMBDA = 0.06;
int MAX_MEMORY_TIME = 3600;

//IHAR: Node Number Test
int NUM_NODE = NUM_NODE_INIT;


/********************************* merge-HAR ***********************************/

//merge-HAR: 
//参数从命令行传入
double RATIO_MERGE_HOTSPOT = 1.0;
double RATIO_NEW_HOTSPOT = 1.0;
double RATIO_OLD_HOTSPOT = 1.0;

bool HEAT_RATIO_EXP = false;
bool HEAT_RATIO_LN = false;

////merge-HAR
////FIXME:如果需要使用到过期的position信息，则不扔掉过期的position，改为放入此vector
//vector<CPosition *> g_overduePositions;


/********************************* 全局变量 ***********************************/

bool DO_IHAR = false;
bool DO_MERGE_HAR = false;
bool DO_COMP = false;

double ALPHA = 0.3;  //ratio for post selection
double BETA = 0.0025;  //ratio for true hotspot
double GAMA = 0.5;  //ratio for HotspotsAboveAverage
double CO_HOTSPOT_HEAT_A1 = 1;
double CO_HOTSPOT_HEAT_A2 = 30;

double PROB_DATA_FORWARD = 1.0;
int DATATIME = 15300;
int RUNTIME = 20000;
int currentTime = 0;
int startTimeForHotspotSelection = SLOT_HOTSPOT_UPDATE;  //no MA node at first

vector<CPosition *> g_positions;
vector<CHotspot *> g_hotspotCandidates;
vector<CHotspot *> g_selectedHotspots;

//上一次贪婪选取最终得到的热点集合，保留
//注意：merge操作得到的输出hotspot应该使用g_hotspotCandidates中的实例修改得到，不可保留对g_oldSelectedHotspots中实例的任何引用，因为在merge结束后将被free
vector<CHotspot *> g_oldSelectedHotspots;

int g_nPositions = 0;
int g_nHotspotCandidates = 0;

string logInfo;

string HELP = "\n       ( ALL CASE SENSITIVE ) \n"
			  "<mode>        -har;          -ihar;       -mhar;    -comp;    -heat-exp;    -heat-ln \n"
			  "<time>		 -data [];	    -run []; \n"
			  "<parameter>   -alpha [];     -beta [];    -gama [];    -heat [] [];    -prob []; \n"
			  "<ihar>		 -lambda [];    -memory []; \n"
			  "<mhar>        -merge [];     -old []; \n\n";


int main(int argc, char* argv[])
{
	/********************************* 命令行参数解析 ***********************************/
	try
	{
		int iField = 0;
		for(iField = 1; iField < argc; )
		{
			string field = argv[iField];
			if( field == "-har" )
			{
				DO_IHAR = DO_MERGE_HAR = false;
				iField++;
			}
			else if( field == "-ihar" )
			{
				DO_IHAR = true;
				DO_MERGE_HAR = false;
				DO_COMP = true;
				iField++;
			}
			else if( field == "-mhar" )
			{
				DO_MERGE_HAR = true;
				DO_IHAR = false;
				iField++;
			}
			else if( field == "-comp" )
			{
				DO_COMP = true;
				iField++;
			}
			else if( field == "-heat-exp" )
			{
				HEAT_RATIO_EXP = true;
				HEAT_RATIO_LN = false;
				iField++;
			}
			else if( field == "-heat-ln" )
			{
				HEAT_RATIO_LN = true;
				HEAT_RATIO_EXP = false;
				iField++;
			}
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
			else if( field == "-memory" )
			{
				if(iField < argc - 1)
					MAX_MEMORY_TIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-merge" )
			{
				if(iField < argc - 1)
					RATIO_MERGE_HOTSPOT = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-old" )
			{
				if(iField < argc - 1)
					RATIO_OLD_HOTSPOT = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-heat" )
			{
				if(iField < argc - 2)
				{
					CO_HOTSPOT_HEAT_A1 = atof( argv[ iField + 1 ] );
					CO_HOTSPOT_HEAT_A2 = atof( argv[ iField + 2 ] );
				}
				iField += 3;
			}
			else if( field == "-prob" )
			{
				if(iField < argc - 1)
					PROB_DATA_FORWARD = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-data" )
			{
				if(iField < argc - 1)
					DATATIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-run" )
			{
				if(iField < argc - 1)
					RUNTIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
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
		logInfo = "\n#" + logtime + "\n";
	
		ofstream parameters("parameters.txt", ios::app);
		parameters << endl << endl << "#" << logtime << endl;

		if(DO_IHAR)
		{
			logInfo += "#IHAR\n\n";
			parameters << "#IHAR" << endl << endl;
			parameters << "LAMBDA" << TAB << LAMBDA << endl;
			parameters << "LIFETIME" << TAB << MAX_MEMORY_TIME << endl << endl;
		}
		else if(DO_MERGE_HAR)
		{
			logInfo += "#merge-HAR\n\n";
			parameters << "#merge-HAR" << endl << endl;
			parameters << "RATIO_MERGE" << TAB << RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << RATIO_OLD_HOTSPOT << endl;
			if( HEAT_RATIO_EXP )
				parameters << "HEAT_RATIO" << TAB << "EXP" << endl << endl;
			else if( HEAT_RATIO_LN )
				parameters << "HEAT_RATIO" << TAB << "LN" << endl << endl;
			else
				parameters << "HEAT_RATIO" << TAB << "FLAT" << endl << endl;
		}
		else
		{
			logInfo += "#HAR\n\n";
			parameters << "#HAR" << endl << endl;
		}
		parameters << "ALPHA" << TAB << ALPHA << endl;
		parameters << "BETA" << TAB << BETA << endl;
		parameters << "GAMA" << TAB << GAMA << endl;
		parameters << "HEAT_CO_1" << TAB << CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << CO_HOTSPOT_HEAT_A2 << endl;
		parameters << "PROB_DATA_FORWARD" << TAB << PROB_DATA_FORWARD << endl;
		parameters << "DATA GENERATION TIME" << TAB << DATATIME << endl;
		parameters << "RUN TIME" << TAB << RUNTIME << endl;
		parameters.close();
	}

	HAR har;

	while(currentTime <= RUNTIME)
	{
		//IHAR: Node Number Test:
		if( TEST_CHANGE_NUM_NODE )
		{
			if(currentTime % SLOT_CHANGE_NUM_NODE == 0 && currentTime > 0)
			{
				har.ChangeNodeNumber();
			}
		}


		/********************************* 地理信息收集 ***********************************/
		if( currentTime % SLOT_LOCATION_UPDATE == 0)
		{
			cout << endl << "########  [ " << currentTime << " ]  LOCATION UPDATE" << endl;
			CPreprocessor::CollectNewPositions(currentTime);
		}


		/*********************************** 热点选取 *************************************/
		if( currentTime % SLOT_HOTSPOT_UPDATE == 0 
			&& currentTime >= startTimeForHotspotSelection )
		{
			cout  <<  endl <<"########  [ " << currentTime << " ]  HOTSPOT SELECTTION" << endl;


			/******************************** 构建候选热点集合 *********************************/
			CPreprocessor::BuildCandidateHotspots(currentTime);


			/*********************************** 贪婪选取 **************************************/
			har.HotspotSelection();
			cout << "####  [ Hotspot ]  " << g_selectedHotspots.size() << endl;


			/*********************************** 热点分类 *************************************/
			har.HotspotClassification();
			cout << "####  [ MA Node ]  " << har.getNClass() << endl;


			/*********************************** 路径规划 *************************************/
			har.MANodeRouteDesign();
		}

		if(currentTime % SLOT_MOBILITYMODEL == 0)
		{
			cout << endl << "########  [ " << currentTime << " ]  NODE MOVEMENT" << endl;
			har.UpdateNodeLocations();
		}

		if(currentTime % SLOT_DATA_GENERATE == 0)
		{
			cout << endl << "########  [ " << currentTime << " ]  DATA GENERATION" << endl;
			har.GenerateData();
		}

		if(currentTime % SLOT_DATA_SEND == 0)
		{
			cout << endl << "########  [ " << currentTime << " ]  DATA DELIVERY" << endl;
			har.SendData();
		}

		if( currentTime >= startTimeForHotspotSelection )
			har.PrintInfo();

		currentTime += TIMESLOT;
	}

}