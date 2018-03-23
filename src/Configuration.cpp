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


map<string, map<string, pair<void*, EnumType>>> CConfiguration::configurations;
string CConfiguration::KEYWORD_HELP = "-help";

//CConfiguration::CConfiguration(string keyword) : keyword(keyword)
//{
//}

bool CConfiguration::has(string group)
{
	map<string, map<string, pair<void*, EnumType>>>::iterator igroup = configurations.find(group);
	return igroup != configurations.end();
}

bool CConfiguration::has(string group, string keyword)
{
	map<string, map<string, pair<void*, EnumType>>>::iterator igroup = configurations.find(group);
	if( igroup == configurations.end() )
		throw string("CConfiguration::ifExists(): Cannot find configuration group with name \"" + group + "\".");
	else
		return igroup->second.find(keyword) != igroup->second.end();
}

void CConfiguration::addGroup(string group)
{
	if( has(group) )
		throw string("CConfiguration::addGroup(): Configuration group with name \"" + group + "\" already exists.");
	configurations[group] = map<string, pair<void*, EnumType>>();
}

void CConfiguration::addConfiguration(string group, string keyword, const type_info & type, void * value)
{
	if( has(group, keyword) )
		throw string("CConfiguration::addConfiguration(): Configuration with keyword \"" + keyword + "\" already exists in group \"" + group + "\".");

	configurations[group][keyword] = pair<void*, EnumType>(value, EnumType(type));
}

pair<void*, EnumType> CConfiguration::getConfiguration(string group, string keyword)
{
	if( !has(group, keyword) )
		throw string("CConfiguration::getConfiguration(): Cannot find configuration with keyword \"" + keyword + "\".");

	return configurations[group][keyword];
}

bool CConfiguration::ParseConfiguration(vector<string> args, string source)
{
	if( args.empty() )
		return false;

	CPrintHelper::PrintNewLine();
	CPrintHelper::PrintHeading("Update Configurations From " + source + " ...");

	vector<string>::iterator iarg = args.begin();

	do
	{
		string keyword = *iarg;

		if( keyword == KEYWORD_HELP )
		{
			Help();
			CRunHelper::Exit(ESKIP);
		}

		size_t pos = keyword.find('.');
		if( pos == keyword.npos )
			throw string("CConfiguration::ParseConfiguration(): Cannot find group name in \"" + keyword + "\".");
		string group = keyword.substr(0, pos);
		keyword = keyword.substr(pos + 1, keyword.npos);


		pair<void*, EnumType> pairConfig = getConfiguration(group, keyword);
		++iarg;
		if( iarg == args.end() )
			throw string("CConfiguration::ParseConfiguration(): Cannot find argument for configuration \"" + keyword + "\".");

		EnumType type = pairConfig.second;
		void *pVoid = pairConfig.first;
		string value = *iarg;

		switch( type.which )
		{
			case bool_type:
				updateConfiguration<bool>(group, keyword, CParseHelper::ParseBool(value));
				break;
			case int_type:
				updateConfiguration<int>(group, keyword, CParseHelper::ParseInt(value));
				break;
			case double_type:
				updateConfiguration<double>(group, keyword, CParseHelper::ParseDouble(value));
				break;
			case string_type:
				updateConfiguration<string>(group, keyword, CParseHelper::ParseString(value));
				break;
			default:
				throw string("CConfiguration::ParseConfiguration(): Unknown type " + string(type.name()) + " for configuration \"" + group + "." + keyword + "\".");
				break;
		}
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

	rtn = ParseConfiguration(args, filename);

	return rtn;
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
		throw pair<int, string>(EMEMORY, string("CConfiguration::ParseConfiguration() : Unvalid access to char* argv[] ! "));
	}
	return configs;
}

void CConfiguration::InitConfiguration()
{
	addGroup("simulation");
	addConfiguration("simulation", "mac_protocol", typeid(int), new EnumMacProtocolScheme(_smac));
	addConfiguration("simulation", "routing_protocol", typeid(int), new EnumRoutingProtocolScheme(_xhar));
	addConfiguration("simulation", "hotspot_select", typeid(int), new EnumHotspotSelectScheme(_original));

	addConfiguration("simulation", "runtime", typeid(int), new int(15000));
	addConfiguration("simulation", "datatime", typeid(int), new int(15000));
	addConfiguration("simulation", "slot", typeid(int), new int(1));


	addGroup("log");
	addConfiguration("log", "slot_log", typeid( int ), new int(0));  //记录数据投递率和数据投递时延的slot
	addConfiguration("log", "slot_brief", typeid( int ), new int(100));
	addConfiguration("log", "detail", typeid( int ), new int(1));  //控制台输出的细致等级，1~3，数值越大越细致
	addConfiguration("log", "tag", typeid(string), new string(""));

	addConfiguration("log", "dir_root", typeid(string), new string("../"));
	addConfiguration("log", "dir_project", typeid(string), new string("../.project/"));
	addConfiguration("log", "dir_run", typeid(string), new string(""));
	addConfiguration("log", "dir_resource", typeid(string), new string("../res/"));
	addConfiguration("log", "dir_log", typeid(string), new string("../log/"));
	addConfiguration("log", "timestamp", typeid(string), new string(""));
	addConfiguration("log", "path_timestamp", typeid(string), new string(""));  // " YY-MM-DD-HH-MM-SS/ "

	addConfiguration("log", "info_log", typeid(string), new string(""));
	addConfiguration("log", "file_default_config", typeid(string), new string("default.config"));
	addConfiguration("log", "file_help", typeid(string), new string("help.md"));
	addConfiguration("log", "file_version", typeid( string ), new string("sim.version"));

	addConfiguration("log", "file_console", typeid(string), new string("console.log"));
	addConfiguration("log", "file_error", typeid(string), new string("error.log"));
	addConfiguration("log", "file_config", typeid(string), new string("config.log"));

	addConfiguration("log", "file_node", typeid(string), new string("node.log"));
	addConfiguration("log", "info_node", typeid(string), new string("#Time	#NodeCount "));
	addConfiguration("log", "file_death", typeid(string), new string("death.log"));
	addConfiguration("log", "info_death", typeid(string), new string("#Time	#DeathCount	#Delivery #DeliveryRatio "));
	addConfiguration("log", "file_encounter", typeid(string), new string("encounter.log"));
	addConfiguration("log", "info_encounter", typeid(string), new string("#Time	(#EncounterAtHotspot%	#EncounterAtHotspot)	#Encounter	 "));
	addConfiguration("log", "file_transmit", typeid(string), new string("transmit.log"));
	addConfiguration("log", "info_transmit", typeid(string), new string("#Time	(#TransmitSuccessful%	#TransmitSuccessful	#Transmit "));
	addConfiguration("log", "file_activation", typeid(string), new string("activation.log"));
	addConfiguration("log", "info_activation", typeid(string), new string("#Time	#PercentAwake... "));
	addConfiguration("log", "file_energy_consumption", typeid(string), new string("energy-consumption.log"));
	addConfiguration("log", "info_energy_consumption", typeid(string), new string("#Time	#AvgEC	(#SumEC	#NodeCount	#CurrentEnergy...) "));
	addConfiguration("log", "file_sink", typeid(string), new string("sink.log"));
	addConfiguration("log", "info_sink", typeid(string), new string("#Time	#EncounterAtSink "));

	addConfiguration("log", "file_delivery_ratio_brief", typeid(string), new string("delivery-ratio-brief.log"));
	addConfiguration("log", "info_delivery_ratio_brief", typeid(string), new string("#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% "));
	addConfiguration("log", "file_delivery_ratio_detail", typeid(string), new string("delivery-ratio-detail.log"));
	addConfiguration("log", "info_delivery_ratio_detail", typeid(string), new string("#Time	#ArrivalCount	#TotalCount	#DeliveryRatio% "));
	addConfiguration("log", "file_delay", typeid(string), new string("delay.log"));
	addConfiguration("log", "info_delay", typeid(string), new string("#Time	#AvgDelay "));
	addConfiguration("log", "file_hop", typeid(string), new string("hop.log"));
	addConfiguration("log", "info_hop", typeid(string), new string("#Time	#AvgHOP "));
	addConfiguration("log", "file_buffer", typeid(string), new string("buffer.log"));
	addConfiguration("log", "info_buffer", typeid(string), new string("#Time	#BufferStateOfEachNode "));
	addConfiguration("log", "file_buffer_statistics", typeid(string), new string("buffer-statistics.log"));
	addConfiguration("log", "info_buffer_statistics", typeid(string), new string("#Time	#AvgBufferStateInHistoryOfEachNode "));

	addConfiguration("log", "file_hotspot", typeid(string), new string("hotspot.log"));
	addConfiguration("log", "info_hotspot", typeid(string), new string("#Time	#HotspotCount "));
	addConfiguration("log", "file_hotspot_details", typeid(string), new string("hotspot-details.log"));
	addConfiguration("log", "info_hotspot_details", typeid(string), new string("#Time #ID	#X	#Y "));
	addConfiguration("log", "file_hotspot_similarity", typeid(string), new string("hotspot-similarity.log"));
	addConfiguration("log", "info_hotspot_similarity", typeid(string), new string("#Time	#Overlap/Old	#Overlap/New	#OverlapArea	#OldArea	#NewArea "));
	addConfiguration("log", "file_visit", typeid(string), new string("visit.log"));
	addConfiguration("log", "info_visit", typeid(string), new string("#Time	#VisitAtHotspotPercent	#VisitAtHotspot	#VisitSum "));
	addConfiguration("log", "file_hotspot_statistics", typeid(string), new string("hotspot-statistics.log"));
	addConfiguration("log", "info_hotspot_statistics", typeid(string), new string("#Cycle	#ID	#Location	#nPosition, nNode	#Ratio	#Tw	#DeliveryCount "));
	addConfiguration("log", "file_delivery_hotspot", typeid(string), new string("delivery-hotspot.log"));
	addConfiguration("log", "info_delivery_hotspot", typeid(string), new string("#Time	#DeliveryCountForSingleHotspotInThisSlot ... "));
	addConfiguration("log", "file_delivery_statistics", typeid(string), new string("delivery-statistics.log"));
	addConfiguration("log", "info_delivery_statistics", typeid(string), new string("#Time	#DeliveryAtHotspotCount	#DeliveryTotalCount	#DeliveryAtHotspotPercent "));
	addConfiguration("log", "file_merge", typeid(string), new string("merge.log"));
	addConfiguration("log", "info_merge", typeid(string), new string("#Time	#MergeHotspotCount	#MergeHotspotPercent	#OldHotspotCount	#OldHotspotPercent	#NewHotspotCount	#NewHotspotPercent "));
	addConfiguration("log", "file_merge_details", typeid(string), new string("merge-details.log"));
	addConfiguration("log", "info_merge_details", typeid(string), new string("#Time	#HotspotType/#MergeAge ... "));
	addConfiguration("log", "file_ma", typeid(string), new string("ma.log"));
	addConfiguration("log", "info_ma", typeid(string), new string("#Time	#MACount	#AvgMAWayPointCount "));
	addConfiguration("log", "file_ma_route", typeid(string), new string("ma-route.log"));
	addConfiguration("log", "info_ma_route", typeid(string), new string("#Time	#WayPoints ... "));
	addConfiguration("log", "file_buffer_ma", typeid(string), new string("buffer-ma.log"));
	addConfiguration("log", "info_buffer_ma", typeid(string), new string("#Time	#BufferStateOfEachMA "));
	addConfiguration("log", "file_ed", typeid(string), new string("ed.log"));
	addConfiguration("log", "info_ed", typeid(string), new string("#Time	#EstimatedDelay "));
	addConfiguration("log", "file_task", typeid( string ), new string("task.log"));
	addConfiguration("log", "info_task", typeid( string ), new string("#Time	#PercentTaskMet	#CountTaskMet	#CountTask"));
	addConfiguration("log", "file_task_node", typeid( string ), new string("task_node.log"));
	addConfiguration("log", "info_task_node", typeid( string ), new string("#Time"));


	addGroup("data");
	addConfiguration("data", "size_data", typeid(int), new int(200));  //( Byte )
	addConfiguration("data", "size_ctrl", typeid(int), new int(10));  // FIXME:
	addConfiguration("data", "size_header_mac", typeid(int), new int(8));  //Mac Header Size
	addConfiguration("data", "max_hop", typeid(int), new int(INVALID));


	addGroup("mac");
	addConfiguration("mac", "cycle", typeid(int), new int(30));
	addConfiguration("mac", "duty_rate", typeid(double), new double(1.0));
	addConfiguration("mac", "cycle_carrier_sense", typeid(int), new int(0));  //不使用占空比工作时，默认等于 0
	addConfiguration("mac", "sync_cycle", typeid(bool), new bool(true));

	
	addGroup("hdc");
	addConfiguration("hdc", "hotspot_duty_rate", typeid(double), new double(1.0));


	addGroup("trans");
	addConfiguration("trans", "speed", typeid(int), new int(2500));  // Byte / s
	addConfiguration("trans", "range", typeid(int), new int(100));  //transmission range
	addConfiguration("trans", "probability", typeid(double), new double(1.0));
	addConfiguration("trans", "consumption_byte_send", typeid(double), new double(0.008));  //( mJ / Byte )
	addConfiguration("trans", "consumption_byte_receive", typeid(double), new double(0.004));
	addConfiguration("trans", "consumption_wake", typeid(double), new double(13.5));  // ( mJ / s )
	addConfiguration("trans", "consumption_sleep", typeid(double), new double(0.015));

	addConfiguration("trans", "size_window", typeid(int), new int(10));
	addConfiguration("trans", "constant_trans_delay", typeid(double), new double(0));  // delay ignored if 0; realtime calculated if negative


	addGroup("sink");
	addConfiguration("sink", "x", typeid(double), new double(0));
	addConfiguration("sink", "y", typeid(double), new double(0));
	addConfiguration("sink", "id", typeid(int), new int(0)); //0为sink节点预留，传感器节点ID从1开始
	addConfiguration("sink", "buffer", typeid(int), new int(INFINITE_INT));  //无限制


	addGroup("node");
	addConfiguration("node", "default_data_rate", typeid(double), new double(6.8));  //( Byte / s )
	addConfiguration("node", "buffer", typeid(int), new int(0));
	addConfiguration("node", "energy", typeid(int), new int(0));
	addConfiguration("node", "lifetime_communication_history", typeid(int), new int(INVALID));  //在这个时间内交换过数据的节点暂时不再交换数据
	//addConfiguration("node", "scheme_relay", typeid(int), new EnumRelayScheme(_loose));
	//addConfiguration("node", "scheme_forward", typeid(int), new EnumForwardScheme(_dump));
	addConfiguration("node", "scheme_queue", typeid(int), new EnumQueueScheme(_fifo));


	addGroup("ma");
	addConfiguration("ma", "speed", typeid(int), new int(30));
	addConfiguration("ma", "buffer", typeid(int), new int(100));
	//addConfiguration("ma", "scheme_relay", typeid(int), new EnumRelayScheme(_loose));
	addConfiguration("ma", "base_id", typeid(int), new int(100));  //ID的起始值，用于和传感器节点相区分
	addConfiguration("ma", "init_num", typeid( int ), new int(INVALID));
	addConfiguration("ma", "max_num", typeid( int ), new int(INVALID));


	//TODO: move to group "node" ?
	addGroup("trace");
	addConfiguration("trace", "continuous", typeid(bool), new bool(true));
	addConfiguration("trace", "extension_trace_file", typeid(string), new string(".trace"));
	addConfiguration("trace", "path", typeid(string), new string("../res/NCSU"));
	addConfiguration("trace", "interval", typeid(int), new int(30));


	addGroup("hs");
	addConfiguration("hs", "slot_position_update", typeid(int), new int(100));  //地理信息收集的slot
	addConfiguration("hs", "slot_hotspot_update", typeid(int), new int(900));  //更新热点和分类的slot
	addConfiguration("hs", "starttime_hospot_select", typeid(int), new int(getConfig<int>("hs", "slot_hotspot_update")));  //no MA node at first
	addConfiguration("hs", "test_hotspot_similarity", typeid(bool), new bool(true));
	addConfiguration("hs", "alpha", typeid(double), new double(0.03));  //ratio for post selection


	addGroup("ihs");
	addConfiguration("ihs", "lambda", typeid(double), new double(0));
	addConfiguration("ihs", "lifetime_position", typeid(int), new int(3600));


	addGroup("mhs");
	addConfiguration("mhs", "ratio_merge_hotspot", typeid(double), new double(1.0));
	addConfiguration("mhs", "ratio_new_hotspot", typeid(double), new double(1.0));
	addConfiguration("mhs", "ratio_old_hotspot", typeid(double), new double(1.0));
	addConfiguration("mhs", "test_balanced_ratio", typeid(bool), new bool(false));


	addGroup("har");
	addConfiguration("har", "beta", typeid(double), new double(0.0025));  //ratio for true hotspot								   
	addConfiguration("har", "heat_1", typeid(double), new double(1));
	addConfiguration("har", "heat_2", typeid(double), new double(30));
	addConfiguration("har", "min_waiting_time", typeid(int), new int(0));  //add minimum waiting time to each hotspot


	addGroup("prophet");
	addConfiguration("prophet", "init_pred", typeid(double), new double(0.75));  //参考值 0.75
	addConfiguration("prophet", "decay_pred", typeid(double), new double(0.98));  //参考值 0.98(/s)
	addConfiguration("prophet", "trans_pred", typeid(double), new double(0.25));  //参考值 0.25
	addConfiguration("prophet", "trans_strict_by_pred", typeid(bool), new bool(true));

	addGroup("pferry");
	addConfiguration("pferry", "path_predict", typeid( string ), new string(""));  //e.g. ../res/predict
	addConfiguration("pferry", "keyword_predict", typeid( string ), new string(""));
	addConfiguration("pferry", "extension_pan_file", typeid( string ), new string(".pan"));
	addConfiguration("pferry", "prestage_protocol", typeid( int ), new EnumRoutingProtocolScheme(_xhar));
	addConfiguration("pferry", "starttime", typeid( int ), new int(INVALID));
	addConfiguration("pferry", "waiting_time", typeid( int ), new int(INVALID));
	addConfiguration("pferry", "return_once_met", typeid( bool ), new bool(false));
	
}

void CConfiguration::ValidateConfiguration()
{
	CPrintHelper::PrintNewLine();
	CPrintHelper::PrintHeading("Validate Configurations ...");

	if( CNode::finiteEnergy() )
	{
		updateConfig<int>("simulation", "runtime", INFINITE_INT);
		updateConfig<int>("simulation", "datatime", INFINITE_INT);
	}

	if( ( getConfig<config::EnumRoutingProtocolScheme>("simulation", "routing_protocol") == config::_xhar
		 || getConfig<config::EnumMacProtocolScheme>("simulation", "mac_protocol") == config::_hdc )
	   && getConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select") == config::_skip )
		updateConfig<config::EnumHotspotSelectScheme>("simulation", "hotspot_select", config::EnumHotspotSelectScheme(config::_original));

	if( getConfig<int>("ma", "max_num") < getConfig<int>("ma", "init_num") )
		updateConfig<int>("ma", "max_num", getConfig<int>("ma", "init_num"));

}


void CConfiguration::PrintConfiguration()
{
	ofstream log(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_config"), ios::ate);
	stringstream ss;

	for( pair<string, map<string, pair<void*, EnumType>>> group : configurations )
	{
		string groupname = group.first;
		map<string, pair<void*, EnumType>> configs = group.second;

		for( pair<string, pair<void*, EnumType>> config : configs )
		{
			string keyword = config.first;
			EnumType type = config.second.second;
			void *pVoid = config.second.first;

			ss << groupname << "." << keyword << TAB;
			switch( type.which )
			{
				case bool_type:
					ss << *CParseHelper::TryCast<bool>(pVoid);
					break;
				case int_type:
					ss << *CParseHelper::TryCast<int>(pVoid);
					break;
				case double_type:
					ss << *CParseHelper::TryCast<double>(pVoid);
					break;
				case string_type:
					ss << *CParseHelper::TryCast<string>(pVoid);
					break;
				default:
					throw string("CConfiguration::ParseConfiguration(): Unknown type " + STRING(type.which) + " for configuration \"" + groupname + "." + keyword + "\".");
					break;
			}
			ss << endl;
		}
		ss << endl;
	}

	log << ss.str();
	log.close();


	// 输出版本信息

	ofstream version(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_version"), ios::out);
#define __VERSIONING_CONTROLLED_BUILD__
#ifdef __VERSIONING_CONTROLLED_BUILD__
	ifstream in_version(getConfig<string>("log", "dir_root") + getConfig<string>("log", "file_version"), ios::in);
	version << in_version.rdbuf();
	in_version.close();
#endif
#ifdef __MANUAL_VERSIONING__
	version << _STRING(config.log.FILE_VERSION_STR);
#endif
	version.close();

}

void CConfiguration::Help()
{
	return;
	CPrintHelper::PrintFile(getConfig<string>("log", "dir_run") + getConfig<string>("log", "file_help"), "");
	_PAUSE_;
}

void CConfiguration::test()
{
	addGroup("group");
	addConfiguration("group", "test", typeid(int), new int(1));
	int n = getConfiguration<int>("group", "test");
	ASSERT(n == 1);
	updateConfiguration<int>("group", "test", int(2));
	n = getConfiguration<int>("group", "test");
	ASSERT(n == 2);

	try
	{
		// duplicate group
		addGroup("group");
	}
	catch( string error )
	{
		CPrintHelper::PrintTestError(error);
	}
	try
	{
		// duplicate key
		addConfiguration("group", "test", typeid(int), new int(3));
	}
	catch( string error )
	{
		CPrintHelper::PrintTestError(error);
	}
	try
	{
		// unknown group
		updateConfiguration<int>("unknown-group", "test", int(4));
	}
	catch( string error )
	{
		CPrintHelper::PrintTestError(error);
	}
	try
	{
		// unknown key
		updateConfiguration<int>("group", "unknown_key", int(4));
	}
	catch( string error )
	{
		CPrintHelper::PrintTestError(error);
	}
	try
	{
		// wrong type
		getConfiguration<string>("group", "unknown_key");
	}
	catch( string error )
	{
		CPrintHelper::PrintTestError(error);
	}
}
