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
double PROB_CROSSOVER = 0.99;  //�������
double PROB_MUTATION = 1.0;  //�������

int CO_MUTATION_FINAL = 10;
int CO_MUTATION_CHILDREN = ROUND( MAX_SOLUTION_NUM / 3 );
double CO_MUTATION_GRADIENT = 0.1;

//����g_ϵ�б������ƶ��������ľ�̬���������µı�������

//�洢��һ�θ���Hotspotʱ�ľɲ����������ڴ��ͷ�
int g_old_nPositions = 0;
int g_old_nHotspots = 0;

//cover����Ͷ���list������SCP
//FIXME: Ӧ�ý�GA���õ��ľ���Ͷ�����Ϣ����������ػ����������뿪
int** g_coverMatrix = NULL;
int* g_degreeForPositions = NULL;
int* g_degreeForHotspots = NULL;

//�ų�������position�����GA�Ż�Ч�ʣ����͸��Ӷȣ�δ���ã�
vector<CPosition *> g_tmpPositions;  //���GA��preprocess������ɾ����position���ڵ�ǰGA���̽���֮����Ҫ�Ż�CPosition::positions��


/************************************ IHAR ************************************/

//IHAR: Node Repair
double LAMBDA = 0;
int MAX_MEMORY_TIME = 3600;

//IHAR: Node Number Test
int NUM_NODE = NUM_NODE_INIT;


/********************************* merge-HAR ***********************************/

//merge-HAR: 
//�����������д���
double RATIO_MERGE_HOTSPOT = 1.0;
double RATIO_NEW_HOTSPOT = 1.0;
double RATIO_OLD_HOTSPOT = 1.0;
double CO_POSITION_DECAY = 1.0;

bool HEAT_RATIO_EXP = false;
bool HEAT_RATIO_LN = false;

////merge-HAR
////FIXME:�����Ҫʹ�õ����ڵ�position��Ϣ�����ӵ����ڵ�position����Ϊ�����vector
//vector<CPosition *> g_overduePositions;


/********************************* ȫ�ֱ��� ***********************************/

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
              "<mode>            -har;                -ihar;                -mhar;               -hotspot-similarity;        -dynamic-node-number;        -balanced-ratio;        -learn; \n"
              "<time>            -time-data [];       -time-run []; \n"
              "<parameter>       -alpha     [];       -beta     [];         -gama     [];        -heat [] [];                -prob-trans []; \n"
              "<ihar>            -lambda    [];       -lifetime []; \n"
              "<mhar>            -merge     [];       -old      [];         -min-wait [];        -heat-exp;                  -heat-ln;                    -max-hotspot [];        -decay [];        -min-weight []; \n\n";


int main(int argc, char* argv[])
{
	/********************************* �����в������� ***********************************/
	try
	{
		int iField = 0;
		for(iField = 1; iField < argc; )
		{
			string field = argv[iField];

			//<mode> ������ֵ�Ĳ����Ͳ���
			if( field == "-har" )
			{
				DO_IHAR = DO_MERGE_HAR = false;
				iField++;
			}
			else if( field == "-ihar" )
			{
				DO_IHAR = true;
				DO_MERGE_HAR = false;
				TEST_HOTSPOT_SIMILARITY = true;
				iField++;
			}
			else if( field == "-mhar" )
			{
				DO_MERGE_HAR = true;
				DO_IHAR = false;
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
			else if( field == "-balanced-ratio" )
			{
				TEST_BALANCED_RATIO = true;
				iField++;
			}
			else if( field == "-learn" )
			{
				TEST_LEARN = true;
				iField++;
			}

			//����ʱ������Ͳ���
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
			else if( field == "-min-wait" )
			{
				if(iField < argc - 1)
					MIN_WAITING_TIME = atoi( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-max-hotspot" )
			{
				if(iField < argc - 1)
					MAX_NUM_HOTSPOT = atoi( argv[ iField + 1 ] );
				iField += 2;
			}

			//����double����
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
			else if( field == "-prob-trans" )
			{
				if(iField < argc - 1)
					PROB_DATA_FORWARD = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-decay" )
			{
				if(iField < argc - 1)
					CO_POSITION_DECAY = atof( argv[ iField + 1 ] );
				iField += 2;
			}
			else if( field == "-min-weight" )
			{
				if(iField < argc - 1)
					MIN_POSITION_WEIGHT = atof( argv[ iField + 1 ] );
				iField += 2;
			}

			//��������������ֵ�Ĳ���
			else if( field == "-heat" )
			{
				if(iField < argc - 2)
				{
					CO_HOTSPOT_HEAT_A1 = atof( argv[ iField + 1 ] );
					CO_HOTSPOT_HEAT_A2 = atof( argv[ iField + 2 ] );
				}
				iField += 3;
			}

			//���help��Ϣ
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
  
	/********************************* ��log��Ϣ�Ͳ�����Ϣд���ļ� ***********************************/
	if(currentTime == 0)
	{
		string logtime;  
		time_t t;  //��ʱ��  
		char buffer[65];
		t = time(NULL); //��ȡĿǰ��ʱ��  
		strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
		logtime = string(buffer);
		logInfo = "\n#" + logtime + TAB;
	
		ofstream parameters("parameters.txt", ios::app);
		parameters << endl << endl << "#" << logtime << endl;

		debugInfo << ALPHA << TAB << BETA << TAB << MIN_WAITING_TIME << TAB ;

		if(DO_IHAR)
		{
			logInfo += "#IHAR";
			parameters << endl;
			parameters << "#IHAR" << endl << endl;
			parameters << "LAMBDA" << TAB << LAMBDA << endl;
			parameters << "LIFETIME" << TAB << MAX_MEMORY_TIME << endl << endl;

			debugInfo << LAMBDA << TAB << MAX_MEMORY_TIME << TAB ;
		}
		else if(DO_MERGE_HAR)
		{
			logInfo += "#merge-HAR";
			parameters << endl;
			parameters << "#merge-HAR" << endl << endl;
			parameters << "RATIO_MERGE" << TAB << RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << RATIO_OLD_HOTSPOT << endl;

			debugInfo << RATIO_MERGE_HOTSPOT << TAB << RATIO_OLD_HOTSPOT << TAB ;

			if( HEAT_RATIO_EXP )
			{
				parameters << "HEAT_RATIO" << TAB << "EXP" << endl << endl;
				debugInfo << "EXP" << TAB ;
			}
			else if( HEAT_RATIO_LN )
			{
				parameters << "HEAT_RATIO" << TAB << "LN" << endl << endl;
				debugInfo << "LN" << TAB ;
			}
			else
			{
				parameters << "HEAT_RATIO" << TAB << "FLAT" << endl << endl;
				debugInfo << "FLAT" << TAB ;
			}
		}
		else
		{
			logInfo += "#HAR";
			parameters << "#HAR" << endl << endl;
		}

		if(TEST_BALANCED_RATIO)
		{
			logInfo += "\t#TEST_BALANCED_RATIO";
			parameters << endl;
			parameters << "#TEST_BALANCED_RATIO" << endl;
		}
		if(TEST_LEARN)
		{
			logInfo += "\t#TEST_LEARN";
			parameters << endl;
			parameters << "#TEST_LEARN" << endl;
			parameters << "DECAY" << TAB << CO_POSITION_DECAY << endl;
			parameters << "MAX_NUM_HOTSPOT" << TAB << MAX_NUM_HOTSPOT << endl;
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
		parameters << "MIN_WAITING_TIME" << TAB << MIN_WAITING_TIME << endl;
		parameters << "PROB_DATA_FORWARD" << TAB << PROB_DATA_FORWARD << endl;
		parameters << "DATA GENERATION TIME" << TAB << DATATIME << endl;
		parameters << "RUN TIME" << TAB << RUNTIME << endl;

		parameters.close();
	}

	HAR har;

	while(currentTime <= RUNTIME)
	{
		//IHAR: Node Number Test:
		if( TEST_DYNAMIC_NUM_NODE )
		{
			if(currentTime % SLOT_CHANGE_NUM_NODE == 0 && currentTime > 0)
			{
				har.ChangeNodeNumber();
			}
		}


		/********************************* ������Ϣ�ռ� ***********************************/
		if( currentTime % SLOT_LOCATION_UPDATE == 0)
		{
			cout << endl << "########  [ " << currentTime << " ]  LOCATION UPDATE" << endl;
			CPreprocessor::CollectNewPositions(currentTime);
		}


		/*********************************** �ȵ�ѡȡ *************************************/
		if( currentTime % SLOT_HOTSPOT_UPDATE == 0 
			&& currentTime >= startTimeForHotspotSelection )
		{
			cout  <<  endl <<"########  [ " << currentTime << " ]  HOTSPOT SELECTTION" << endl;


			/******************************** ������ѡ�ȵ㼯�� *********************************/
			CPreprocessor::BuildCandidateHotspots(currentTime);


			/*********************************** ̰��ѡȡ **************************************/
			har.HotspotSelection();
			cout << "####  [ Hotspot ]  " << CHotspot::selectedHotspots.size() << endl;


			/*********************************** �ȵ���� *************************************/
			har.HotspotClassification();
			cout << "####  [ MA Node ]  " << har.getNClass() << endl;


			/*********************************** ·���滮 *************************************/
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
	debugInfo.close();
}