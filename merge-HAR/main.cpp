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

//�洢��һ�θ���Hotspotʱ�ľɲ����������ڴ��ͷ�
int g_old_nPositions = 0;
int g_old_nHotspots = 0;

//cover����Ͷ���list������SCP
int** g_coverMatrix = NULL;
int* g_degreeForPositions = NULL;
int* g_degreeForHotspots = NULL;

//�ų�������position�����GA�Ż�Ч�ʣ����͸��Ӷȣ�δ���ã�
vector<CPosition *> g_tmpPositions;  //���GA��preprocess������ɾ����position���ڵ�ǰGA���̽���֮����Ҫ�Ż�g_positions��


/************************************ IHAR ************************************/

//Node Number Test
int NUM_NODE = NUM_NODE_INIT;


/********************************* merge-HAR ***********************************/

//merge-HAR: 
//�����������д���
double RATIO_MERGE_HOTSPOT = 1.0;
double RATIO_NEW_HOTSPOT = 1.0;
double RATIO_OLD_HOTSPOT = 1.0;

//merge-HAR: 
//��һ��̰��ѡȡ���յõ����ȵ㼯�ϣ�����������һ��ѡȡ��merge����
//ע�⣺merge�����õ������hotspotӦ��ʹ��g_hotspotCandidates�е�ʵ���޸ĵõ������ɱ�����g_oldSelectedHotspots��ʵ�����κ����ã���Ϊ��merge�����󽫱�free
vector<CHotspot *> g_oldSelectedHotspots;

////merge-HAR
////FIXME:�����Ҫʹ�õ����ڵ�position��Ϣ�����ӵ����ڵ�position����Ϊ�����vector
//vector<CPosition *> g_overduePositions;


/********************************* ȫ�ֱ��� ***********************************/

bool DO_IHAR = false;
bool DO_MERGE_HAR = false;

double CO_HOTSPOT_HEAT_A1 = 1;
double CO_HOTSPOT_HEAT_A2 = 30;
double ALPHA = 0.3;  //ratio for post selection
double BETA = 0.0025;  //ratio for true hotspot

int DATATIME = 15300;
int RUNTIME = 20000;
int currentTime = 0;
int startTimeForHotspotSelection = SLOT_HOTSPOT_UPDATE;  //no MA node at first

vector<CPosition *> g_positions;
vector<CHotspot *> g_hotspotCandidates;
vector<CHotspot *> g_selectedHotspots;

int g_nPositions = 0;
int g_nHotspotCandidates = 0;

string logInfo;

string HELP = "( ALL CASE SENSITIVE ) \n"
			  "<mode>        -har;         -ihar;       -mhar; \n"
	          "<parameter>   -alpha [];    -beta [];    -heat [] []; \n"
			  "<merge>       -merge [];    -old []; \n";


int main(int argc, char* argv[])
{
	/********************************* �����в������� ***********************************/
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
				iField++;
			}
			else if( field == "-mhar" )
			{
				DO_MERGE_HAR = true;
				DO_IHAR = false;
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
				break;
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
		logInfo = "\n#" + logtime + "\n";
	
		ofstream parameters("parameters.txt", ios::app);
		parameters << endl << endl << "#" << logtime << endl;

		if(DO_IHAR)
		{
			logInfo += "#IHAR\n\n";
			parameters << "#IHAR" << endl << endl;
			parameters << "LAMBDA" << TAB << LAMBDA(1) << endl;
			parameters << "LIFETIME" << TAB << MAX_MEMORY_TIME << endl << endl;
		}
		else if(DO_MERGE_HAR)
		{
			logInfo += "#merge-HAR\n\n";
			parameters << "#merge-HAR" << endl << endl;
			parameters << "RATIO_MERGE" << TAB << RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << RATIO_OLD_HOTSPOT << endl << endl;
		}
		else
		{
			logInfo += "#HAR\n\n";
			parameters << "#HAR" << endl << endl;
		}
		parameters << "ALPHA" << TAB << ALPHA << endl;
		parameters << "BETA" << TAB << BETA << endl;
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
				har.ChangeNodeNumber(currentTime);
			}
		}


		/********************************* ������Ϣ�ռ� ***********************************/
		if( currentTime % SLOT_LOCATION_UPDATE == 0 )
		{
			cout << endl << "########  [ " << currentTime << " ]  LOCATION UPDATE" << endl;
			CPreprocessor::CollectNewPositions(currentTime);
		}


		/*********************************** �ȵ�ѡȡ *************************************/
		//if( currentTime < startTimeForHotspotSelection )
		//{
		//	cout << endl << "########  [ " << currentTime << " ]  HOTSPOT SELECTTION" << endl;
		//	cout << "##  PASSED" << endl;
		//	currentTime += 5;
		//	continue;
		//}
		if( currentTime % SLOT_HOTSPOT_UPDATE == 0 )
		{
			cout  <<  endl <<"########  [ " << currentTime << " ]  HOTSPOT SELECTTION" << endl;

			//merge-HAR: ����һ��ѡ�е��ȵ㼯�ϱ��浽g_oldSelectedHotspots
			if(DO_MERGE_HAR)
			{
				//�ֶ��ͷžɵ�g_oldSelectedHotspots
				if(! g_oldSelectedHotspots.empty())
					CPreprocessor::freePointerVector(g_oldSelectedHotspots);
				g_oldSelectedHotspots = g_selectedHotspots;
			}


			/******************************** ������ѡ�ȵ㼯�� *********************************/
			CPreprocessor::BuildCandidateHotspots(currentTime);


			/*********************************** ̰��ѡȡ **************************************/
			CGreedySelection greedySelection;

			//merge-HAR: 
			if(DO_MERGE_HAR)
				greedySelection.mergeHotspots(currentTime);
			greedySelection.GreedySelect(currentTime);


			/********************************* ����ѡȡ���� ***********************************/
			CPostSelector postSelector_Greedy(g_selectedHotspots);
			g_selectedHotspots = postSelector_Greedy.PostSelect(currentTime);

			//IHAR: POOR NODE REPAIR
			if(DO_IHAR)
			{
				CNodeRepair repair(g_selectedHotspots, g_hotspotCandidates, currentTime);
				g_selectedHotspots = repair.RepairPoorNodes();
				g_selectedHotspots = postSelector_Greedy.assignPositionsToHotspots(g_selectedHotspots);
			}
			cout << "####  [ Hotspot ]  " << g_selectedHotspots.size() << endl;


			/*********************************** �ȵ���� *************************************/
			har.HotspotClassification(currentTime, g_selectedHotspots);
			cout << "####  [ MA Node ]  " << har.getNClass() << endl;


			/*********************************** ·���滮 *************************************/
			har.MANodeRouteDesign(currentTime);
		}

		if(currentTime % SLOT_MOBILITYMODEL == 0)
			cout << endl << "########  [ " << currentTime << " ]  NODE MOBILITY" << endl;
			har.UpdateNodeLocations(currentTime);

		if(currentTime % SLOT_DATA_GENERATE == 0)
			cout << endl << "########  [ " << currentTime << " ]  DATA GENERATE" << endl;
			har.GenerateData(currentTime);

		if(currentTime % SLOT_DATA_SEND == 0)
			cout << endl << "########  [ " << currentTime << " ]  DATA SEND" << endl;
			har.SendData(currentTime);

		har.PrintInfo(currentTime);

		currentTime += TIMESLOT;
	}

}