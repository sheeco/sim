#pragma once

#include "Preprocessor.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "GreedySelection.h"
#include "Prophet.h"
#include "HDC.h"

using namespace std;

_MacProtocol MAC_PROTOCOL = _smac;
_RoutingProtocol ROUTING_PROTOCOL = _prophet;
_HotspotSelect HOTSPOT_SELECT = _original;


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

bool TEST_LEARN = false;
int MAX_NUM_HOTSPOT = 999999;
double MIN_POSITION_WEIGHT = 0;


/********************************* ȫ�ֱ��� ***********************************/

bool TEST_BALANCED_RATIO = false;
bool TEST_HOTSPOT_SIMILARITY = false;
bool TEST_DYNAMIC_NUM_NODE = false;

double SINK_X = 0.0;
double SINK_Y = 0.0;
int TRANS_RANGE = 100;  //transmission range

double ALPHA = 0.03;  //ratio for post selection
double BETA = 0.0025;  //ratio for true hotspot
double GAMMA = 0.5;  //ratio for HotspotsAboveAverage
double CO_HOTSPOT_HEAT_A1 = 1;
double CO_HOTSPOT_HEAT_A2 = 30;
int MIN_WAITING_TIME = 0;  //add minimum waiting time to each hotspot

int startTimeForHotspotSelection = SLOT_HOTSPOT_UPDATE;  //no MA node at first
double PROB_DATA_FORWARD = 1.0;
int DATATIME = 0;
int RUNTIME = 0;
int currentTime = 0;

string INFO_LOG;
ofstream debugInfo("debug.txt", ios::app);

string INFO_HELP = "\n                                 !!!!!! ALL CASE SENSITIVE !!!!!! \n"
 	          "<node>      -sink           [][];  -range           [];   -prob-trans  [];   -energy      [];   -time-data   [];   -time-run   []; \n"
			  "<data>      -buffer           [];  -data-rate       [];   -data-size   [];  \n"
              "<mac>       -hdc;                  -cycle           [];   -dc-default  [];   -dc-hotspot  []; \n"
			  "<route>     -epidemic;             -prophet;              -har;              -hop         [];   -ttl         [];\n"
			  "<prophet>   -spoken           [];  -queue           []; \n"
              "<hs>        -ihar;                 -mhar;                 -alpha       [];   -beta        [];   -heat      [][]; \n"
              "<ihar>      -lambda           [];  -lifetime        []; \n"
			  "<mhar>      -merge            [];  -old             []; \n"
              "<test>      -dynamic-node-number;  -hotspot-similarity;   -balanced-ratio; \n\n" ;

string INFO_DEBUG = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery	#Delay	#EnergyConsumption	(#NetworkTime)	(#EncounterAtHotspot)	#Log \n" ;

int main(int argc, char* argv[])
{


	/************************************ ����Ĭ��ֵ *************************************/

	SINK_X = -200;
	SINK_Y = 200;
	TRANS_RANGE = 250;
	DATATIME = 15000;
	RUNTIME = 15000;
	CNode::DATA_SIZE = 400;
	CNode::CTRL_SIZE = 10;
	CNode::DEFAULT_DATA_RATE = 1.0 / 150.0;
	CNode::BUFFER_CAPACITY = 200;
	CNode::SLOT_TOTAL = 10 * SLOT_MOBILITYMODEL;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
	CData::MAX_TTL = 0;


	/********************************** �����в������� ***********************************/

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
			else if( field == "-ihar" )
			{
				HOTSPOT_SELECT = _improved;
				iField++;
			}
			else if( field == "-mhar" )
			{
				HOTSPOT_SELECT = _merge;
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
			else if( field == "-balanced-ratio" )
			{
				TEST_BALANCED_RATIO = true;
				iField++;
			}

			//���Ͳ���
			else if( field == "-range" )
			{
				if(iField < argc - 1)
					TRANS_RANGE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}			
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
					Epidemic::MAX_QUEUE_SIZE = atoi( argv[ iField + 1 ] );
				iField += 2;
			}			
			else if( field == "-spoken" )
			{
				if(iField < argc - 1)
					Epidemic::SPOKEN_MEMORY = atoi( argv[ iField + 1 ] );
				iField += 2;
			}


			//double����
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

			//��������������ֵ�Ĳ���
			else if( field == "-sink" )
			{
				if(iField < argc - 2)
				{
					SINK_X = atof( argv[ iField + 1 ] );
					SINK_Y = atof( argv[ iField + 2 ] );
				}
				iField += 3;
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


			//���help��Ϣ
			else if( field == "-help" )
			{
				cout << INFO_HELP;
				_PAUSE;
				exit(1);
			}
			else
				iField++;

		}

	}
	catch(exception e)
	{
		cout << endl << "Error @ main() : Wrong Parameter Format!" << endl;
		cout << INFO_HELP;
		_PAUSE;
		exit(1);
	}

	srand( static_cast<unsigned>(time(nullptr)) ); 
  
	/********************************* ��log��Ϣ�Ͳ�����Ϣд���ļ� ***********************************/
	if(currentTime == 0)
	{
		string logtime;  
		time_t t;  //��ʱ��  
		char buffer[65];
		t = time(nullptr); //��ȡĿǰ��ʱ��  
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
		parameters << "DATA RATE" << TAB << CNode::DEFAULT_DATA_RATE << endl;
		parameters << "DATA SIZE" << TAB << CNode::DATA_SIZE << endl;
		parameters << "BUFFER CAPACITY" << TAB << CNode::BUFFER_CAPACITY << endl;
		parameters << "NODE ENERGY" << TAB << CNode::ENERGY << endl;

		parameters << "DATA TIME" << TAB << DATATIME << endl;
		parameters << "RUN TIME" << TAB << RUNTIME << endl;
		parameters << "PROB DATA FORWARD" << TAB << PROB_DATA_FORWARD << endl;

		//����ļ�Ϊ��ʱ������ļ�ͷ
		debugInfo.seekp(0, ios::end);
		if( ! debugInfo.tellp() )
			debugInfo << INFO_DEBUG ;

		debugInfo << DATATIME << TAB << RUNTIME << TAB << PROB_DATA_FORWARD << TAB << CNode::BUFFER_CAPACITY << TAB << CNode::ENERGY << TAB ;
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
				parameters << "LIFETIME" << TAB << MAX_MEMORY_TIME << endl << endl;

				debugInfo << CNode::HOTSPOT_DUTY_CYCLE << TAB << ALPHA << TAB << BETA << TAB << MAX_MEMORY_TIME << TAB ;
			}

			else if( HOTSPOT_SELECT == _merge )
			{
				INFO_LOG += "-mHAR ";
				parameters << "-mHAR " << endl << endl;
				parameters << "RATIO_MERGE" << TAB << RATIO_MERGE_HOTSPOT << endl;
				parameters << "RATIO_NEW" << TAB << RATIO_NEW_HOTSPOT << endl;
				parameters << "RATIO_OLD" << TAB << RATIO_OLD_HOTSPOT << endl;

				debugInfo << RATIO_MERGE_HOTSPOT << TAB << RATIO_OLD_HOTSPOT << TAB ;

			}

			else
			{
				debugInfo << CNode::HOTSPOT_DUTY_CYCLE << TAB << ALPHA << TAB << BETA << TAB ;
				INFO_LOG += "-HAR ";
				parameters << "-HAR " << endl << endl;
			}

			parameters << "ALPHA" << TAB << ALPHA << endl;
			parameters << "BETA" << TAB << BETA << endl;
			parameters << "GAMMA" << TAB << GAMMA << endl;
			parameters << "HEAT_CO_1" << TAB << CO_HOTSPOT_HEAT_A1 << endl;
			parameters << "HEAT_CO_2" << TAB << CO_HOTSPOT_HEAT_A2 << endl;
		}
		//debugInfo.flush();

		if(TEST_DYNAMIC_NUM_NODE)
		{
			INFO_LOG += "-TEST_DYNAMIC_NODE_NUMBER";
			parameters << endl;
			parameters << "-TEST_DYNAMIC_NODE_NUMBER" << endl;
		}

		INFO_LOG += "\n";
		parameters << endl;

		parameters.close();
	}

	//HAR har;

	while(currentTime <= RUNTIME)
	{

		if( MAC_PROTOCOL == _hdc )
			CHDC::Operate(currentTime);

		bool dead = false;

		dead = ! Prophet::Operate(currentTime);

		if( dead )
		{
			RUNTIME = currentTime;
			CHDC::PrintInfo(currentTime);
			Prophet::PrintInfo(currentTime);
			break;
		}

		currentTime += SLOT;

	}

	debugInfo << INFO_LOG.replace(0, 1, "");
	debugInfo.flush();

	debugInfo.close();
	_ALERT;
}