#pragma once

#include "SortHelper.h"
#include "PostSelect.h"
#include "Prophet.h"
#include "HDC.h"
#include "Sink.h"
#include "SMac.h"
#include "MANode.h"


/********************************* Global Var *********************************/

//TODO: move these global config to a global CConfiguration parameter
_MAC_PROTOCOL MAC_PROTOCOL = _smac;
_ROUTING_PROTOCOL ROUTING_PROTOCOL = _prophet;
_HOTSPOT_SELECT HOTSPOT_SELECT = _original;

int DATATIME = 0;
int RUNTIME = 0;
string DATASET;

/********************************* Usage & Output ***********************************/

string INFO_LOG;
string FILE_DEBUG = "debug.txt";


string INFO_HELP = "\n                                 !!!!!! ALL CASE SENSITIVE !!!!!! \n"
 	          "<node>      -sink           [][]  -range           []   -prob-trans  []   -energy      []   -time-data   []   -time-run    [] \n"
			  "<data>      -buffer           []  -data-rate       []   -data-size   []  \n"
              "<mac>       -hdc                  -cycle           []   -dc-default  []   -dc-hotspot  [] \n"
			  "<route>     -epidemic             -prophet              -har              -hop         []   -ttl         []\n"
			  "<prophet>   -spoken           []  -queue           [] \n"
              "<hs>        -hs                   -ihs                  -mhs              -alpha       []   -beta        []   -heat      [][] \n"
              "<ihar>      -lambda           []  -lifetime        [] \n"
			  "<mhar>      -merge            []  -old             [] \n"
              "<test>      -dynamic-node-number  -hotspot-similarity   -balanced-ratio \n\n" ;

string INFO_DEBUG = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery	#Delay	#EnergyConsumption	(#NetworkTime)	(#EncounterAtHotspot)	#Log \n" ;


//TODO: ����������ھ�̬���������� private / protected
//TODO: CConfiguration / CConfigureHelper ?
//TODO: Ĭ�����ò�����Ϊ�� XML ��ȡ
void initConfiguration()
{
	/************************************** Default Config **************************************/

	CGeneralNode::TRANS_RANGE = 100;  //transmission range
	CGeneralNode::PROB_DATA_FORWARD = 1.0;

	CSink::SINK_ID = 0; //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ
	CSink::SINK_X = 0;
	CSink::SINK_Y = 0;
	CSink::BUFFER_CAPACITY = 999999999;  //������

	CNode::NUM_NODE_INIT = 0;

	CRoutingProtocol::SLOT_DATA_SEND = SLOT_MOBILITYMODEL;  //���ݷ���slot
	CNode::DEFAULT_DATA_RATE = 0;
	CNode::DATA_SIZE = 0;
	CNode::CTRL_SIZE = 0;
	CMacProtocol::MAC_SIZE = 8;  //Mac Header Size

	CNode::BUFFER_CAPACITY = 0;
	CNode::ENERGY = 0;
	CNode::RECEIVE_MODE = CGeneralNode::_loose;
	CNode::SEND_MODE = CGeneralNode::_dump;
	CNode::QUEUE_MODE = CGeneralNode::_fifo;

	CNode::SLOT_TOTAL = 0;
	CNode::DEFAULT_DUTY_CYCLE = 0;
	CNode::HOTSPOT_DUTY_CYCLE = 0; 

	/********** Dynamic Node Number **********/
	CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
	CMacProtocol::SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //��̬�ڵ��������ʱ���ڵ���������仯������
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
	CEpidemic::SPOKEN_MEMORY = 0;

	/******************************  Prophet  ******************************/

	CNode::INIT_DELIVERY_PRED = 0.70;  //0.75
	CNode::DECAY_RATIO = 0.90;  //0.98(/s)
	CNode::TRANS_RATIO = 0.20;  //0.25


	/**************************  Hotspot Select  ***************************/

	CHotspot::SLOT_LOCATION_UPDATE = 100;  //������Ϣ�ռ���slot
	CHotspot::SLOT_HOTSPOT_UPDATE = 900;  //�����ȵ�ͷ����slot
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

	CNode::DATA_SIZE = 250;  //Up to 250 Bytes
	CNode::CTRL_SIZE = 10;

	CNode::SLOT_TOTAL = 10 * SLOT_MOBILITYMODEL;
	CNode::DEFAULT_DUTY_CYCLE = 1.0;
}

bool ParseParameters(int argc, char* argv[])
{
	ofstream debug(FILE_DEBUG, ios::app);
	string logtime;  
	time_t t;  //��ʱ��  
	char buffer[65];
	t = time(nullptr); //��ȡĿǰ��ʱ��  
	strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
	logtime = string(buffer);
	INFO_LOG = "#" + logtime + TAB;

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
				CMacProtocol::TEST_DYNAMIC_NUM_NODE = true;
				iField++;
			}
			else if( field == "-balanced-ratio" )
			{
				HAR::TEST_BALANCED_RATIO = true;
				iField++;
			}

			//���Ͳ���
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


			//double����
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

			//��������������ֵ�Ĳ���
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


			//���help��Ϣ
			else if( field == "-help" )
			{
				cout << INFO_HELP;
				ofstream help("help.txt", ios::out);
				help << INFO_HELP;
				help.close();
				_PAUSE_;
				Exit(0);
			}
			else
				iField++;

		}
		debug.close();
		return true;
	}
	catch(exception e)
	{
		cout << endl << "Error @ ParseParameters() : Wrong Parameter Format!" << endl;
		cout << INFO_HELP;
		debug.close();
		return false;
	}
}

void PrintConfiguration()
{
	ofstream parameters("parameters.txt", ios::app);
	parameters << endl << endl << INFO_LOG << endl << endl;

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

	//����ļ�Ϊ��ʱ������ļ�ͷ
	ofstream debug(FILE_DEBUG, ios::app);
	debug.seekp(0, ios::end);
	if( ! debug.tellp() )
		debug << INFO_DEBUG ;

	debug << DATATIME << TAB << RUNTIME << TAB << CGeneralNode::PROB_DATA_FORWARD << TAB << CNode::BUFFER_CAPACITY << TAB << CNode::ENERGY << TAB ;
	if( CData::useHOP() )
		debug << CData::MAX_HOP << TAB ;
	else 
		debug << CData::MAX_TTL << TAB ;

	debug << CNode::SLOT_TOTAL << TAB << CNode::DEFAULT_DUTY_CYCLE << TAB ;

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

			debug << HAR::MAX_MEMORY_TIME << TAB ;
		}

		else if( HOTSPOT_SELECT == _merge )
		{
			INFO_LOG += "-mHAR ";
			parameters << "-mHAR " << endl << endl;
			parameters << "RATIO_MERGE" << TAB << CHotspot::RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << CHotspot::RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << CHotspot::RATIO_OLD_HOTSPOT << endl;

			debug << CHotspot::RATIO_MERGE_HOTSPOT << TAB << CHotspot::RATIO_OLD_HOTSPOT << TAB ;

		}

		else
		{
			debug << CNode::HOTSPOT_DUTY_CYCLE << TAB << CPostSelect::ALPHA << TAB << HAR::BETA << TAB ;
		}

		parameters << "ALPHA" << TAB << CPostSelect::ALPHA << endl;
		parameters << "BETA" << TAB << HAR::BETA << endl;
		parameters << "HEAT_CO_1" << TAB << HAR::CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << HAR::CO_HOTSPOT_HEAT_A2 << endl;
	}

	if( CMacProtocol::TEST_DYNAMIC_NUM_NODE)
	{
		INFO_LOG += "-TEST_DYNAMIC_NODE_NUMBER";
		parameters << endl;
		parameters << "-TEST_DYNAMIC_NODE_NUMBER" << endl;
	}

	INFO_LOG += "\n";
	parameters << endl;

	parameters.close();
	debug.close();

}

bool Run()
{
	int currentTime = 0;
	while( currentTime <= RUNTIME )
	{
		bool dead = false;

		dead = ! CProphet::Operate(currentTime);

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
	//TODO: release �汾��Ӧ��Ϊ while(1) ѭ��

	/************************************ ����Ĭ��ֵ *************************************/

	initConfiguration();

	/************************************ �����в������� *************************************/

	if( ! ParseParameters(argc, argv) )
	{
		_PAUSE_;
		Exit(-1);
	}

	/********************************* ��log��Ϣ�Ͳ�����Ϣд���ļ� ***********************************/
	
	PrintConfiguration();

	srand( static_cast<unsigned>(time(nullptr)) ); 

	Run();

	Exit(0);

	_ALERT_;
}