#include "Configuration.h"
#include "FileHelper.h"
#include "ParseHelper.h"
#include "PrintHelper.h"
#include "RunHelper.h"
#include "Trace.h"
#include "HDC.h"
#include "HAR.h"
#include "Prophet.h"
#include "../.project/version.h"


CConfiguration::CConfig configs;


CConfiguration::EnumMacProtocolScheme CConfiguration::CConfig::MAC_PROTOCOL = config::_smac;
CConfiguration::EnumRoutingProtocolScheme CConfiguration::CConfig::ROUTING_PROTOCOL = config::_xhar;
CConfiguration::EnumHotspotSelectScheme CConfiguration::CConfig::HOTSPOT_SELECT = config::_original;

int CConfiguration::CSimulationConfig::RUNTIME = 15000;
int CConfiguration::CSimulationConfig::DATATIME = 15000;
int CConfiguration::CSimulationConfig::SLOT = 1;

int CConfiguration::CLogConfig::SLOT_LOG = 100;  //记录数据投递率和数据投递时延的slot

string CConfiguration::CLogConfig::DIR_ROOT = "../";
string CConfiguration::CLogConfig::DIR_PROJECT = "../.project/";
string CConfiguration::CLogConfig::DIR_RUN = "";
string CConfiguration::CLogConfig::DIR_RESOURCE = "../res/";
string CConfiguration::CLogConfig::DIR_LOG = "../log/";
string CConfiguration::CLogConfig::TIMESTAMP;
string CConfiguration::CLogConfig::PATH_TIMESTAMP = "";  // " YY-MM-DD-HH-MM-SS/ "

string CConfiguration::CLogConfig::INFO_LOG;
string CConfiguration::CLogConfig::FILE_DEFAULT_CONFIG = "default.config";
string CConfiguration::CLogConfig::FILE_PARAMETES = "parameters.log";
string CConfiguration::CLogConfig::FILE_HELP = "help.md";
string CConfiguration::CLogConfig::FILE_VERSION = "sim.version";

string CConfiguration::CLogConfig::FILE_ERROR = "error.log";
string CConfiguration::CLogConfig::FILE_CONFIG = "config.log";
string CConfiguration::CLogConfig::FILE_FINAL = "final.log";
string CConfiguration::CLogConfig::INFO_FINAL = "#DataTime	#RunTime	#TransProb	#Buffer	#Energy	#HOP/TTL	#Cycle	#DefaultDC	(#HotspotDC	#Alpha	#Beta)	#Delivery%	#Delay/	#HOP/	#EnergyConsumption/	#TransmitSuccessful%	#EncounterActive%	(#EncounterAtHotspot%)	(#NetworkTime	#Node)	#Log \n";

string CConfiguration::CLogConfig::FILE_NODE = "node.log";
string CConfiguration::CLogConfig::INFO_NODE = "#Time	#NodeCount \n";
string CConfiguration::CLogConfig::FILE_DEATH = "death.log";
string CConfiguration::CLogConfig::INFO_DEATH = "#Time	#DeathCount	#Delivery #DeliveryRatio \n";
string CConfiguration::CLogConfig::FILE_ENCOUNTER = "encounter.log";
string CConfiguration::CLogConfig::INFO_ENCOUNTER = "#Time	(#EncounterAtHotspot%	#EncounterAtHotspot)	#Encounter	 \n";
string CConfiguration::CLogConfig::FILE_TRANSMIT = "transmit.log";
string CConfiguration::CLogConfig::INFO_TRANSMIT = "#Time	(#TransmitSuccessful%	#TransmitSuccessful	#Transmit \n";
string CConfiguration::CLogConfig::FILE_ACTIVATION = "activation.log";
string CConfiguration::CLogConfig::INFO_ACTIVATION = "#Time	#PercentAwake... \n";
string CConfiguration::CLogConfig::FILE_ENERGY_CONSUMPTION = "energy-consumption.log";
string CConfiguration::CLogConfig::INFO_ENERGY_CONSUMPTION = "#Time	#AvgEC	(#SumEC	#NodeCount	#CurrentEnergy...) \n";
string CConfiguration::CLogConfig::FILE_SINK = "sink.log";
string CConfiguration::CLogConfig::INFO_SINK = "#Time	#EncounterAtSink \n";

string CConfiguration::CLogConfig::FILE_DELIVERY_RATIO_900 = "delivery-ratio.log";
string CConfiguration::CLogConfig::INFO_DELIVERY_RATIO_900 = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% \n";
string CConfiguration::CLogConfig::FILE_DELIVERY_RATIO_100 = "delivery-ratio-100.log";
string CConfiguration::CLogConfig::INFO_DELIVERY_RATIO_100 = "#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% \n";
string CConfiguration::CLogConfig::FILE_DELAY = "delay.log";
string CConfiguration::CLogConfig::INFO_DELAY = "#Time	#AvgDelay \n";
string CConfiguration::CLogConfig::FILE_HOP = "hop.log";
string CConfiguration::CLogConfig::INFO_HOP = "#Time	#AvgHOP \n";
string CConfiguration::CLogConfig::FILE_BUFFER = "buffer.log";
string CConfiguration::CLogConfig::INFO_BUFFER = "#Time	#BufferStateOfEachNode \n";
string CConfiguration::CLogConfig::FILE_BUFFER_STATISTICS = "buffer-statistics.log";
string CConfiguration::CLogConfig::INFO_BUFFER_STATISTICS = "#Time	#AvgBufferStateInHistoryOfEachNode \n";

string CConfiguration::CLogConfig::FILE_HOTSPOT = "hotspot.log";
string CConfiguration::CLogConfig::INFO_HOTSPOT = "#Time	#HotspotCount \n";
string CConfiguration::CLogConfig::FILE_HOTSPOT_DETAILS = "hotspot-details.log";
string CConfiguration::CLogConfig::INFO_HOTSPOT_DETAILS = "#Time #ID	#X	#Y \n";
string CConfiguration::CLogConfig::FILE_HOTSPOT_SIMILARITY = "hotspot-similarity.log";
string CConfiguration::CLogConfig::INFO_HOTSPOT_SIMILARITY = "#Time	#Overlap/Old	#Overlap/New	#OverlapArea	#OldArea	#NewArea \n";
string CConfiguration::CLogConfig::FILE_VISIT = "visit.log";
string CConfiguration::CLogConfig::INFO_VISIT = "#Time	#VisitAtHotspotPercent	#VisitAtHotspot	#VisitSum \n";
string CConfiguration::CLogConfig::FILE_HOTSPOT_STATISTICS = "hotspot-statistics.log";
string CConfiguration::CLogConfig::INFO_HOTSPOT_STATISTICS = "#Cycle	#ID	#Location	#nPosition, nNode	#Ratio	#Tw	#DeliveryCount \n";
string CConfiguration::CLogConfig::FILE_DELIVERY_HOTSPOT = "delivery-hotspot.log";
string CConfiguration::CLogConfig::INFO_DELIVERY_HOTSPOT = "#Time	#DeliveryCountForSingleHotspotInThisSlot ... \n";
string CConfiguration::CLogConfig::FILE_DELIVERY_STATISTICS = "delivery-statistics.log";
string CConfiguration::CLogConfig::INFO_DELIVERY_STATISTICS = "#Time	#DeliveryAtHotspotCount	#DeliveryTotalCount	#DeliveryAtHotspotPercent \n";
string CConfiguration::CLogConfig::FILE_MERGE = "merge.log";
string CConfiguration::CLogConfig::INFO_MERGE = "#Time	#MergeHotspotCount	#MergeHotspotPercent	#OldHotspotCount	#OldHotspotPercent	#NewHotspotCount	#NewHotspotPercent \n";
string CConfiguration::CLogConfig::FILE_MERGE_DETAILS = "merge-details.log";
string CConfiguration::CLogConfig::INFO_MERGE_DETAILS = "#Time	#HotspotType/#MergeAge ... \n";
string CConfiguration::CLogConfig::FILE_MA = "ma.log";
string CConfiguration::CLogConfig::INFO_MA = "#Time	#MACount	#AvgMAWayPointCount \n";
string CConfiguration::CLogConfig::FILE_MA_ROUTE = "ma-route.log";
string CConfiguration::CLogConfig::INFO_MA_ROUTE = "#Time	#WayPoints ... \n";
string CConfiguration::CLogConfig::FILE_BUFFER_MA = "buffer-ma.log";
string CConfiguration::CLogConfig::INFO_BUFFER_MA = "#Time	#BufferStateOfEachMA \n";
string CConfiguration::CLogConfig::FILE_ED = "ed.log";
string CConfiguration::CLogConfig::INFO_ED = "#Time	#EstimatedDelay \n";

int CConfiguration::CDataConfig::SIZE_DATA = 200;  //( Byte )
int CConfiguration::CDataConfig::SIZE_CTRL = 10;  // FIXME:
int CConfiguration::CDataConfig::SIZE_HEADER_MAC = 8;  //Mac Header Size
int CConfiguration::CDataConfig::MAX_HOP = 0;
//int CConfiguration::CDataConfig::MAX_TTL = 0;

int CConfiguration::CMacConfig::CYCLE_TOTAL = 30;
double CConfiguration::CMacConfig::DUTY_RATE = 1.0;
int CConfiguration::CMacConfig::CYCLE_CARRIER_SENSE = 0;  //不使用占空比工作时，默认等于 0
bool CConfiguration::CMacConfig::SYNC_CYCLE = true;

double CConfiguration::CHDCConfig::HOTSPOT_DUTY_RATE = 1.0;

bool CConfiguration::CDynamicNodeNumberConfig::TEST_DYNAMIC_NODE_NUM = false;
int CConfiguration::CDynamicNodeNumberConfig::SLOT_CHANGE_NODE_NUM = 0;  //动态节点个数测试时，节点个数发生变化的周期
int CConfiguration::CDynamicNodeNumberConfig::MIN_NUM_NODE = 0;
int CConfiguration::CDynamicNodeNumberConfig::MAX_NUM_NODE = 0;

int CConfiguration::CTransConfig::SPEED_TRANS = 2500;  // Byte / s
int CConfiguration::CTransConfig::RANGE_TRANS = 100;  //transmission range
double CConfiguration::CTransConfig::PROB_TRANS = 1.0;
double CConfiguration::CTransConfig::CONSUMPTION_BYTE_SEND = 0.008;  //( mJ / Byte )
double CConfiguration::CTransConfig::CONSUMPTION_BYTE_RECEIVE = 0.004;
double CConfiguration::CTransConfig::CONSUMPTION_WAKE = 13.5;  // ( mJ / s )
double CConfiguration::CTransConfig::CONSUMPTION_SLEEP = 0.015;

int CConfiguration::CTransConfig::WINDOW_TRANS = 10;
double CConfiguration::CTransConfig::CONSTANT_TRANS_DELAY = 0;  // delay ignored if 0; realtime calculated if negative

double CConfiguration::CSinkConfig::X = 0;
double CConfiguration::CSinkConfig::Y = 0;
int CConfiguration::CSinkConfig::SINK_ID = 0; //0为sink节点预留，传感器节点ID从1开始
int CConfiguration::CSinkConfig::CAPACITY_BUFFER = 0xfffffff;  //无限制

double CConfiguration::CNodeConfig::DEFAULT_DATA_RATE = 6.8;  //( Byte / s )
int CConfiguration::CNodeConfig::CAPACITY_BUFFER = 0;
int CConfiguration::CNodeConfig::CAPACITY_ENERGY = 0;
int CConfiguration::CNodeConfig::LIFETIME_SPOKEN_CACHE = 0;  //在这个时间内交换过数据的节点暂时不再交换数据
CConfiguration::EnumRelayScheme CConfiguration::CNodeConfig::SCHEME_RELAY = config::_loose;
CConfiguration::EnumForwardScheme CConfiguration::CNodeConfig::SCHEME_FORWARD = config::_dump;
CConfiguration::EnumQueueScheme CConfiguration::CNodeConfig::SCHEME_QUEUE = config::_fifo;

int CConfiguration::CMANodeConfig::SPEED = 30;
int CConfiguration::CMANodeConfig::CAPACITY_BUFFER = 100;
CConfiguration::EnumRelayScheme CConfiguration::CMANodeConfig::SCHEME_RELAY = config::_loose;
int CConfiguration::CMANodeConfig::START_COUNT_ID = 100;  //ID的起始值，用于和传感器节点相区分

bool CConfiguration::CTraceConfig::CONTINUOUS_TRACE = true;
string CConfiguration::CTraceConfig::EXTENSION_TRACE = ".trace";
string CConfiguration::CTraceConfig::PATH_TRACE = "../res/NCSU";
int CConfiguration::CTraceConfig::SLOT_TRACE = 30;

int CConfiguration::CHotspotSelectConfig::SLOT_POSITION_UPDATE = 100;  //地理信息收集的slot
int CConfiguration::CHotspotSelectConfig::SLOT_HOTSPOT_UPDATE = 900;  //更新热点和分类的slot
int CConfiguration::CHotspotSelectConfig::STARTTIME_HOSPOT_SELECT = configs.hs.SLOT_HOTSPOT_UPDATE;  //no MA node at first
bool CConfiguration::CHotspotSelectConfig::TEST_HOTSPOT_SIMILARITY = true;
double CConfiguration::CHotspotSelectConfig::ALPHA = 0.03;  //ratio for post selection

double CConfiguration::CImprovedHotspotSelectConfig::LAMBDA = 0;
int CConfiguration::CImprovedHotspotSelectConfig::LIFETIME_POSITION = 3600;

double CConfiguration::CMergedHotspotSelectConfig::RATIO_MERGE_HOTSPOT = 1.0;
double CConfiguration::CMergedHotspotSelectConfig::RATIO_NEW_HOTSPOT = 1.0;
double CConfiguration::CMergedHotspotSelectConfig::RATIO_OLD_HOTSPOT = 1.0;
bool CConfiguration::CMergedHotspotSelectConfig::TEST_BALANCED_RATIO = false;

double CConfiguration::CHARConfig::BETA = 0.0025;  //ratio for true hotspot								   
double CConfiguration::CHARConfig::CO_HOTSPOT_HEAT_A1 = 1;
double CConfiguration::CHARConfig::CO_HOTSPOT_HEAT_A2 = 30;
int CConfiguration::CHARConfig::MIN_WAITING_TIME = 0;  //add minimum waiting time to each hotspot

double CConfiguration::CProphetConfig::INIT_PRED = 0.75;  //参考值 0.75
double CConfiguration::CProphetConfig::RATIO_PRED_DECAY = 0.98;  //参考值 0.98(/s)
double CConfiguration::CProphetConfig::RATIO_PRED_TRANS = 0.25;  //参考值 0.25
bool CConfiguration::CProphetConfig::TRANS_STRICT_BY_PRED = true;


map<string, pair<string, vector<CConfiguration::CConfigArgument> > > CConfiguration::configurations;
string CConfiguration::KEYWORD_HELP = "-help";

//CConfiguration::CConfiguration(string keyword) : keyword(keyword)
//{
//}

bool CConfiguration::ifExists(string keyword)
{
	if( configurations.find(keyword) != configurations.end() )
		return true;
	return false;
}

pair<string, vector<CConfiguration::CConfigArgument>> CConfiguration::getConfiguration(string keyword)
{
	if( !ifExists(keyword) )
		throw string("CConfiguration::getConfiguration() : Cannot find configuration with keyword \"" + keyword + "\" !");
	return configurations[keyword];
}

void CConfiguration::AddConfiguration(string keyword, string description, vector<CConfigArgument> fields)
{
	if( ifExists(keyword) )
		throw string("CConfiguration::AddConfiguration() : Configuration with keyword \"" + keyword + "\" already exists.");

	configurations[keyword] = pair<string, vector<CConfigArgument> >(description, fields);
}

bool CConfiguration::AddConfiguration(string keyword, void * OnValue, bool defaultState, void * dstAttr, string description)
{
	vector<CConfigArgument> fields;
	CConfigArgument field = { config::_none, dstAttr, OnValue, defaultState};
	fields.push_back(field);
	AddConfiguration(keyword, description, fields);
	return true;
}

bool CConfiguration::AddConfiguration(string keyword, EnumArgumentType type, void * defaultValue, void * dstAttr, string description)
{
	if( type == config::_none )
		return false;

	vector<CConfigArgument> fields;
	CConfigArgument field = { type, dstAttr, defaultValue };
	fields.push_back(field);
	AddConfiguration(keyword, description, fields);
	return true;
}

bool CConfiguration::AddConfiguration(string keyword, EnumArgumentType type_1, void * default_1, void * dstAttr_1, EnumArgumentType type_2, void * default_2, void * dstAttr_2, string description)
{
	if( type_1 == config::_none
	   || type_2 == config::_none )
		return false;

	vector<CConfigArgument> fields;
	CConfigArgument field_1 = { type_1, dstAttr_1, default_1 };
	CConfigArgument field_2 = { type_2, dstAttr_2, default_2 };
	fields.push_back(field_1);
	fields.push_back(field_2);
	AddConfiguration(keyword, description, fields);
	return true;
}

void CConfiguration::UpdateConfiguration(string keyword, vector<string> argvs)
{
	if( !ifExists(keyword) )
		throw string("CConfiguration::UpdateConfiguration() : Cannot find command \"" + keyword + "\" !");

	string description = getConfiguration(keyword).first;
	vector<CConfigArgument> fields = getConfiguration(keyword).second;
	int nargv = fields.size();
	if( nargv == 1 &&
	   fields[0].type == config::_none )
		nargv = 0;

	auto ifield = fields.begin();
	auto iargv = argvs.begin();
	for( ; ifield != fields.end(); ++ifield )
	{
		CConfigArgument field = *ifield;

		if(field.type == config::_none )
		{ 
			field.state = true;
		}
		else if( iargv == argvs.end() )
		{
			throw string("CConfiguration::UpdateConfiguration() : Expect " + STRING(nargv) + " argument(s) while getting "
						 + STRING(argvs.size()) + " instead.");
		}
		else
		{
			string argv = *iargv;
			try
			{
				int *pInt = nullptr;
				bool *pBool = nullptr;
				double *pDouble = nullptr;
				string *pString = nullptr;

				switch( field.type )
				{
					case config::_none:
						break;

					case _int:
						pInt = static_cast< int* >( field.value );
						*pInt = CParseHelper::ParseInt(argv);
						field.value = pInt;
						break;

					case _bool:
						pBool = static_cast< bool* >( field.value );
						*pBool = CParseHelper::ParseBool(argv);
						field.value = pBool;
						break;

					case _double:
						pDouble = static_cast< double* >( field.value );
						*pDouble = CParseHelper::ParseDouble(argv);
						field.value = pDouble;
						break;

					case _string:
						pString = static_cast< string* >( field.value );
						*pString = CParseHelper::ParseString(argv);
						field.value = pString;
						break;

					default:
						break;
				}
			}
			catch( exception e )
			{
				throw string("CConfiguration::UpdateConfiguration : Cannot resolve argument \"" + argv + "\" ! \n"
							 + "Hint : " + description);
			}
			++iargv;
		}

		*ifield = field;
	}
	if( iargv != argvs.end() )
	{
		throw string("CConfiguration::UpdateConfiguration() : Expect " + STRING(nargv) + " argument(s) while getting "
					 + STRING(argvs.size()) + " instead.");
	}
}

vector<string> CConfiguration::ConvertToConfiguration(int argc, char * argv[])
{
	vector<string> configs;
	try
	{
		for( int i = 0; i < argc; ++i )
		{
			configs.push_back(string(argv[i]));
		}
	}
	catch( exception e )
	{
		throw pair<int, string>(EMEMORY, string( "CConfiguration::ParseConfiguration() : Unvalid access to char* argv[] ! " ) );
	}
	return configs;
}

bool CConfiguration::ParseConfiguration(vector<string> args)
{
	if( args.empty() )
		throw string( "CConfiguration::ParseConfiguration() : Parameters are requested ! ");

	vector<string>::iterator iarg = args.begin();

	do
	{
		string keyword = *iarg;

		if( keyword == KEYWORD_HELP )
		{
			Help();
			Exit(ESKIP);
		}

		stringstream strConfig;
		strConfig << keyword;
		string descriptionForKeyword = getConfiguration(keyword).first;
		vector<CConfigArgument> fields = getConfiguration(keyword).second;
		int nField = fields.size();
		vector<string> argvs;

		for(int i = 0; i < nField; ++i)
		{
			CConfigArgument field = fields[i];
			string argv;
			if( field.type != config::_none
			   && (iarg + 1) == args.end() )
				throw string("CConfiguration::ParseConfiguration() : Wrong value for command \"" + keyword + "\" ! ("
							 + "Hint : " + descriptionForKeyword) + ")";
			
			if( field.type != config::_none )
			{
				iarg++;
				argv = *iarg;
				argvs.push_back(argv);
				strConfig << " " << argv;
			}

		}
		UpdateConfiguration(keyword, argvs);

		iarg++;

	} while( iarg != args.end() );

	return true;
}

bool CConfiguration::ParseConfiguration(string filename)
{
	if( ! CFileHelper::IfExists(filename) )
		throw pair<int, string>(EFILE, string( "CConfiguration::ParseConfiguration() : Cannot find file \"" + filename + "\" ! " ) );

	//read string from file
	ifstream file(filename, ios::in);
	string configs(( std::istreambuf_iterator<char>(file) ), std::istreambuf_iterator<char>());

	//parse string into tokens
	string delim = " \t\n";
	vector<string> args;
	bool rtn = false;
	try
	{
		args = CParseHelper::ParseToken(configs, delim);
	}
	catch( const char * str )
	{
		throw pair<int, string>(EPARSE, string("CConfiguration::ParseConfiguration() : Cannot find configuration in " + filename + "!"));
	}

	rtn = ParseConfiguration(args);

	return rtn;
}

void CConfiguration::InitConfiguration()
{
	//不带参数的命令
	// TODO: define option-like config for these configs
	AddConfiguration("--smac", new EnumMacProtocolScheme(config::_smac), true, &configs.MAC_PROTOCOL, "");
	AddConfiguration("--hdc", new EnumMacProtocolScheme(config::_hdc), false, &configs.MAC_PROTOCOL, "");
	AddConfiguration("--prophet", new EnumRoutingProtocolScheme(config::_prophet), false, &configs.ROUTING_PROTOCOL, "");
	AddConfiguration("--har", new EnumRoutingProtocolScheme(config::_xhar), true, &configs.ROUTING_PROTOCOL, "");
	AddConfiguration("--hs", new EnumHotspotSelectScheme(config::_original), true, &configs.HOTSPOT_SELECT, "");
	AddConfiguration("--ihs", new EnumHotspotSelectScheme(config::_improved), false, &configs.HOTSPOT_SELECT, "");
	AddConfiguration("--mhs", new EnumHotspotSelectScheme(config::_merge), false, &configs.HOTSPOT_SELECT, "");

	//bool/int 参数
	AddConfiguration("-continuous-trace", _bool, new bool(true), &configs.trace.CONTINUOUS_TRACE, "");
	AddConfiguration("-dc-sync", _bool, new bool(true), &configs.mac.SYNC_CYCLE, "");
	AddConfiguration("-dynamic-node-number", _bool, new bool(false), &configs.dynamic.TEST_DYNAMIC_NODE_NUM, "");
	AddConfiguration("-pred-strict", _bool, new bool(true), &configs.prophet.TRANS_STRICT_BY_PRED, "");
	AddConfiguration("-hotspot-similarity", _bool, new bool(true), &configs.hs.TEST_HOTSPOT_SIMILARITY, "");
	AddConfiguration("-balanced-ratio", _bool, new bool(false), &configs.mhs.TEST_BALANCED_RATIO, "");
	AddConfiguration("-time-data", _int, new int(15000), &configs.simulation.DATATIME, "");
	AddConfiguration("-time-run", _int, new int(15000), &configs.simulation.RUNTIME, "");
	AddConfiguration("-slot", _int, new int(1), &configs.simulation.SLOT, "");
	AddConfiguration("-slot-trace", _int, new int(30), &configs.trace.SLOT_TRACE, "");
	AddConfiguration("-node-min", _int, new int(0), &configs.dynamic.MIN_NUM_NODE, "");
	AddConfiguration("-node-max", _int, new int(0), &configs.dynamic.MAX_NUM_NODE, "");
	AddConfiguration("-log-slot", _int, new int(100), &configs.log.SLOT_LOG, "");
	AddConfiguration("-trans-range", _int, new int(100), & configs.trans.RANGE_TRANS, "");
	AddConfiguration("-trans-speed", _int, new int(2500), & configs.trans.SPEED_TRANS, "");
	AddConfiguration("-lifetime", _int, new int(0), &configs.ihs.LIFETIME_POSITION, "");
	AddConfiguration("-cycle", _int, new int(0), &configs.mac.CYCLE_TOTAL, "");
	AddConfiguration("-slot-carrier-sense", _int, new int(0), &configs.mac.CYCLE_CARRIER_SENSE, "");
	AddConfiguration("-hop", _int, new int(0), &configs.data.MAX_HOP, "");
	AddConfiguration("-buffer", _int, new int(0), &configs.node.CAPACITY_BUFFER, "");
	AddConfiguration("-buffer-ma", _int, new int(100), &configs.ma.CAPACITY_BUFFER, "");
	AddConfiguration("-data-size", _int, new int(200), &configs.data.SIZE_DATA, "");
	AddConfiguration("-energy", _int, new int(0), &configs.node.CAPACITY_ENERGY, "");
	AddConfiguration("-spoken", _int, new int(0), &configs.node.LIFETIME_SPOKEN_CACHE, "");
	AddConfiguration("-trans-window", _int, new int(10), &configs.trans.WINDOW_TRANS, "");

	//double 参数
	AddConfiguration("-data-rate", _double, new double(6.8), &configs.node.DEFAULT_DATA_RATE, "");  // 200B/30s
	AddConfiguration("-dc-default", _double, new double(1.0), &configs.mac.DUTY_RATE, "");
	AddConfiguration("-dc-hotspot", _double, new double(0), &configs.hdc.HOTSPOT_DUTY_RATE, "");
	AddConfiguration("-alpha", _double, new double(0.03), &configs.hs.ALPHA, "");
	AddConfiguration("-beta", _double, new double(0.0025), &configs.har.BETA, "");
	AddConfiguration("-lambda", _double, new double(0), &configs.ihs.LAMBDA, "");
	AddConfiguration("-merge", _double, new double(1.0), &configs.mhs.RATIO_MERGE_HOTSPOT, "");
	AddConfiguration("-old", _double, new double(1.0), &configs.mhs.RATIO_OLD_HOTSPOT, "");
	AddConfiguration("-trans-prob", _double, new double(1.0), & configs.trans.PROB_TRANS, "");
	AddConfiguration("-pred-init", _double, new double(0.7), &configs.prophet.INIT_PRED, "");
	AddConfiguration("-pred-decay", _double, new double(0.9), &configs.prophet.RATIO_PRED_DECAY, "");

	//string 参数
	AddConfiguration("-log-path", _string, new string("../log/"), &configs.log.DIR_LOG, "");
	AddConfiguration("-path-trace", _string, new string("../res/NCSU"), &configs.trace.PATH_TRACE, "");
	AddConfiguration("-ext-trace", _string, new string(".trace"), &configs.trace.EXTENSION_TRACE, "");

	//带双参数的命令
	AddConfiguration("-sink", _double, new double(0), &configs.sink.X, _double, new double(0), &configs.sink.Y, "");
	AddConfiguration("-heat", _double, new double(1), &configs.har.CO_HOTSPOT_HEAT_A1, _double, new double(30), &configs.har.CO_HOTSPOT_HEAT_A2, "");


	/*********************************************  按照命令格式解析参数配置  *********************************************/

	ParseConfiguration(configs.log.DIR_RUN + configs.log.FILE_DEFAULT_CONFIG);

}

void CConfiguration::ValidateConfiguration()
{
	if( CNode::finiteEnergy() )
		configs.simulation.RUNTIME = configs.simulation.DATATIME = 999999;

	if( ( configs.ROUTING_PROTOCOL == config::_xhar
		 || configs.MAC_PROTOCOL == config::_hdc )
	   && configs.HOTSPOT_SELECT == config::_skip )
		configs.HOTSPOT_SELECT = config::_original;

}

// TODO: print config update
bool CConfiguration::ApplyConfigurations()
{
	for( auto configs : configurations )
	{
		string keyword = configs.first;
		string description = configs.second.first;
		vector<CConfigArgument> fields = configs.second.second;
		for( CConfigArgument field : fields )
		{
			int *pInt = nullptr;
			bool *pBool = nullptr;
			double *pDouble = nullptr;
			string *pString = nullptr;

			switch( field.type )
			{
				case config::_none:
					if( !field.state )
						break;

				case _int:
					pInt = static_cast< int* >( field.dstAttr );
					*pInt = *static_cast< int* >( field.value );
					break;

				case _bool:
					pBool = static_cast< bool* >( field.dstAttr );
					*pBool = *static_cast< bool* >( field.value );
					break;

				case _double:
					pDouble = static_cast< double* >( field.dstAttr );
					*pDouble = *static_cast< double* >( field.value );
					break;

				case _string:
					pString = static_cast< string* >( field.dstAttr );
					*pString = *static_cast< string* >( field.value );
					break;

				default:
					break;
			}
		}

	}
	return true;
}

void CConfiguration::PrintConfiguration()
{
	ofstream log(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_CONFIG, ios::ate);

	for( auto configs : configurations )
	{
		string keyword = configs.first;
		string description = configs.second.first;

		log << keyword << TAB;
		vector<CConfigArgument> fields = configs.second.second;
		for( auto field : fields )
		{
			string str;
			switch( field.type )
			{
				int *pInt = nullptr;
				bool *pBool = nullptr;
				double *pDouble = nullptr;
				string *pString = nullptr;

				switch( field.type )
				{
					case config::_none:
						if( field.state )
							str = "on";
						else
							str = "off";
						break;

					case _int:
						pInt = static_cast< int* >( field.value );
						str = STRING(*pInt);
						break;

					case _bool:
						pBool = static_cast< bool* >( field.value );
						// TODO: better way of toString（） ?
						if( *pBool )
							str = "true";
						else
							str = "false";
						break;

					case _double:
						pDouble = static_cast< double* >( field.value );
						str = STRING(*pDouble);
						break;

					case _string:
						pString = static_cast< string* >( field.value );
						str = *pString;
						break;

					default:
						break;
				}
			}
			log << str << TAB;
		}
		log << endl;
	}

	log.close();


	// TODO: remove parameter.log & move logging of all the configs into PrintConfiguration
	ofstream parameters(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_PARAMETES, ios::app);
	parameters << endl << endl << configs.log.INFO_LOG << endl << endl;

	parameters << "CYCLE" << TAB << configs.mac.CYCLE_TOTAL << endl;
	parameters << "DEFAULT_DC" << TAB << configs.mac.DUTY_RATE << endl;

	if( CData::useHOP() )
		parameters << "HOP" << TAB << configs.data.MAX_HOP << endl;
	//	else
	//		parameters << "TTL" << TAB << configs.data.MAX_TTL << endl;
	parameters << "DATA_RATE" << TAB << configs.data.SIZE_DATA << "B / " << configs.data.SIZE_DATA / configs.node.DEFAULT_DATA_RATE << "s" << endl;
	parameters << "DATA_SIZE" << TAB << configs.data.SIZE_DATA << endl;
	parameters << "BUFFER" << TAB << configs.node.CAPACITY_BUFFER << endl;
	parameters << "ENERGY" << TAB << configs.node.CAPACITY_ENERGY << endl;

	parameters << "DATA_TIME" << TAB << configs.simulation.DATATIME << endl;
	parameters << "RUN_TIME" << TAB << configs.simulation.RUNTIME << endl;
	parameters << "PROB_TRANS" << TAB << configs.trans.PROB_TRANS << endl;
	parameters << "configs.trans.WINDOW_TRANS" << TAB << configs.trans.WINDOW_TRANS << endl;


	//输出文件为空时，输出文件头
	ofstream final(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_FINAL, ios::app);
	final.seekp(0, ios::end);
	if( !final.tellp() )
		final << configs.log.INFO_FINAL;

	final << configs.simulation.DATATIME << TAB << configs.simulation.RUNTIME << TAB << configs.trans.PROB_TRANS << TAB << configs.node.CAPACITY_BUFFER << TAB << configs.node.CAPACITY_ENERGY << TAB;
	if( CData::useHOP() )
		final << configs.data.MAX_HOP << TAB;

	final << configs.mac.CYCLE_TOTAL << TAB << configs.mac.DUTY_RATE << TAB;

	//	if( config.ROUTING_PROTOCOL == _epidemic )
	//	{
	//		config.log.INFO_LOG += "$Epidemic ";
	//		parameters << "$Epidemic " << endl << endl;
	//	}
	//	else 

	if( configs.ROUTING_PROTOCOL == config::_prophet )
	{
		configs.log.INFO_LOG += "$Prophet ";
		parameters << endl << "$Prophet " << endl << endl;
		parameters << "PRED_INIT" << TAB << configs.prophet.INIT_PRED << endl;
		parameters << "PRED_DECAY" << TAB << configs.prophet.RATIO_PRED_DECAY << endl;
		parameters << "PRED_TRANS" << TAB << configs.prophet.RATIO_PRED_TRANS << endl;
	}

	if( configs.MAC_PROTOCOL == config::_hdc )
	{
		configs.log.INFO_LOG += "$HDC ";
		parameters << endl << "$HDC " << endl << endl;
		parameters << "HOTSPOT_DC" << TAB << configs.hdc.HOTSPOT_DUTY_RATE << endl;
		final << configs.hdc.HOTSPOT_DUTY_RATE << TAB;
	}

	if( configs.HOTSPOT_SELECT != config::_skip )
	{
		if( configs.HOTSPOT_SELECT == config::_improved )
		{
			configs.log.INFO_LOG += "$iHS ";
			parameters << endl << "$iHS " << endl << endl;
			parameters << "POSITION_LIFETIME" << TAB << configs.ihs.LIFETIME_POSITION << endl << endl;

			final << configs.ihs.LIFETIME_POSITION << TAB;
		}

		else if( configs.HOTSPOT_SELECT == config::_merge )
		{
			configs.log.INFO_LOG += "$mHS ";
			parameters << endl << "$mHS " << endl << endl;
			parameters << "RATIO_MERGE" << TAB << configs.mhs.RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << configs.mhs.RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << configs.mhs.RATIO_OLD_HOTSPOT << endl;

			final << configs.mhs.RATIO_MERGE_HOTSPOT << TAB << configs.mhs.RATIO_OLD_HOTSPOT << TAB;

		}

		else
		{
			configs.log.INFO_LOG += "$HS ";
			parameters << endl << "$HS " << endl << endl;
		}

		parameters << "ALPHA" << TAB << configs.hs.ALPHA << endl;
		//final << configs.hs.ALPHA << TAB ;
	}

	if( configs.ROUTING_PROTOCOL == config::_xhar )
	{
		configs.log.INFO_LOG += "$xHAR ";
		parameters << endl << "$xHAR " << endl << endl;
		parameters << "configs.har.BETA" << TAB << configs.har.BETA << endl;
		parameters << "HEAT_CO_1" << TAB << configs.har.CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << configs.har.CO_HOTSPOT_HEAT_A2 << endl;
		//final << configs.har.BETA << TAB;
	}

	if( configs.dynamic.TEST_DYNAMIC_NODE_NUM )
	{
		configs.log.INFO_LOG += "#DYNAMIC_NODE_NUMBER";
		parameters << endl << "#DYNAMIC_NODE_NUMBER" << endl;
	}

	parameters << endl;

	parameters.close();
	final.close();

	// 输出版本信息

	ofstream version(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_VERSION, ios::out);
#define __VERSIONING_CONTROLLED_BUILD__
#ifdef __VERSIONING_CONTROLLED_BUILD__
	ifstream in_version(configs.log.DIR_ROOT + configs.log.FILE_VERSION, ios::in);
	version << in_version.rdbuf();
	in_version.close();
#endif
#ifdef __MANUAL_VERSIONING__
	version << _STRING(config.log.FILE_VERSION_STR);
#endif
	version.close();

}

inline void CConfiguration::Help()
{
	return;
	CPrintHelper::PrintFile(configs.log.DIR_RUN + configs.log.FILE_HELP, "");
	_PAUSE_;
}
