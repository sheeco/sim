#include "Configuration.h"
#include "FileHelper.h"
#include "ParseHelper.h"

map<string, pair<string, vector<CConfiguration::_FIELD_CONFIGURATION> > > CConfiguration::configurations;


//CConfiguration::CConfiguration(string keyword) : keyword(keyword)
//{
//}

bool CConfiguration::ifExists(string keyword)
{
	if( configurations.find(keyword) != configurations.end() )
		return true;
	return false;
}

//void CConfiguration::addValue(_TYPE_VALUE type, void * dstAttr, int defaultValue)
//{
//	_VALUE_CONFIGURATION value = {type, dstAttr, defaultValue};
//	this->values.push_back(value);
//}

bool CConfiguration::AddConfiguration(string keyword, void * dstAttr, int value, string description)
{
	if( ifExists(keyword) )
		return false;

	vector<_FIELD_CONFIGURATION> fields;
	_FIELD_CONFIGURATION field = { _none, dstAttr, value };
	fields.push_back(field);
	configurations[keyword] = pair<string, vector<_FIELD_CONFIGURATION> >(description, fields);
	return true;
}

bool CConfiguration::AddConfiguration(string keyword, _TYPE_FIELD type, void * dstAttr, string description)
{
	if( ifExists(keyword) )
		return false;
	if( type == _none )
		return false;

	vector<_FIELD_CONFIGURATION> fields;
	_FIELD_CONFIGURATION field = { type, dstAttr, 0 };
	fields.push_back(field);
	configurations[keyword] = pair<string, vector<_FIELD_CONFIGURATION> >(description, fields);
	return true;
}

bool CConfiguration::AddConfiguration(string keyword, _TYPE_FIELD type_1, void * dstAttr_1, _TYPE_FIELD type_2, void * dstAttr_2, string description)
{
	if( ifExists(keyword) )
		return false;
	if( type_1 == _none
	    || type_2 == _none )
		return false;

	vector<_FIELD_CONFIGURATION> fields;
	_FIELD_CONFIGURATION field_1 = { type_1, dstAttr_1, 0 };
	_FIELD_CONFIGURATION field_2 = { type_2, dstAttr_2, 0 };
	fields.push_back(field_1);
	fields.push_back(field_2);
	configurations[keyword] = pair<string, vector<_FIELD_CONFIGURATION> >(description, fields);
	return true;
}

bool CConfiguration::ParseConfiguration(int argc, char* argv[], string description)
{
	//if( argc <= 0 )
	//{
	//	stringstream error;
	//	error << "Error @ ParseConfiguration() : Parameters are requested ! ";
	//	cout << endl << error.str() << endl;
	//	Help();
	//	_PAUSE_;
	//	Exit(EINVAL, error.str());
	//	return false;
	//}

	//// 将使用的命令行参数输出到文件
	//ofstream config_log(PATH_ROOT + PATH_LOG + FILE_CONFIG, ios::app);
	//config_log << "######  " << description << endl;
	//for( int i = 0; i < argc; ++i )
	//{
	//	if( argv[i][0] == '-'
	//	   && isalpha(argv[i][1]) )
	//		config_log << endl;
	//	config_log << argv[i] << " ";
	//}
	//config_log << endl << endl;

	//config_log.close();

	//try
	//{
	//	for( int iField = 0; iField < argc; )
	//	{
	//		string field = argv[iField];
	//		char *endptr = nullptr;

	//		//<mode> 不带值域的参数
	//		if( field == "--hdc" )
	//		{
	//			MAC_PROTOCOL = _hdc;
	//			HOTSPOT_SELECT = _original;
	//			++iField;
	//		}
	//		else if( field == "--prophet" )
	//		{
	//			ROUTING_PROTOCOL = _prophet;
	//			++iField;
	//		}
	//		//			else if( field == "-epidemic" )
	//		//			{
	//		//				ROUTING_PROTOCOL = _epidemic;
	//		//				++iField;
	//		//			}
	//		else if( field == "--har" )
	//		{
	//			ROUTING_PROTOCOL = _xhar;
	//			HOTSPOT_SELECT = _original;
	//			++iField;
	//		}
	//		else if( field == "--hs" )
	//		{
	//			HOTSPOT_SELECT = _original;
	//			++iField;
	//		}
	//		else if( field == "--ihs" )
	//		{
	//			HOTSPOT_SELECT = _improved;
	//			++iField;
	//		}
	//		else if( field == "--mhs" )
	//		{
	//			HOTSPOT_SELECT = _merge;
	//			++iField;
	//		}

	//		//bool 参数
	//		else if( field == "-continuous-trace" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			string val(argv[iField + 1]);
	//			if( val == "on" )
	//				CCTrace::CONTINUOUS_TRACE = true;
	//			else if( val == "off" )
	//				CCTrace::CONTINUOUS_TRACE = false;
	//			else
	//				throw field;
	//			iField += 2;
	//		}
	//		else if( field == "-dc-sync" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			string val(argv[iField + 1]);
	//			if( val == "on" )
	//				CMacProtocol::SYNC_DC = true;
	//			else if( val == "off" )
	//				CMacProtocol::SYNC_DC = false;
	//			else
	//				throw field;
	//			iField += 2;
	//		}
	//		else if( field == "-dynamic-node-number" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			string val(argv[iField + 1]);
	//			if( val == "on" )
	//				CMacProtocol::TEST_DYNAMIC_NUM_NODE = true;
	//			else if( val == "off" )
	//				CMacProtocol::TEST_DYNAMIC_NUM_NODE = false;
	//			else
	//				throw field;
	//			iField += 2;
	//		}
	//		else if( field == "-pred-strict" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			string val(argv[iField + 1]);
	//			if( val == "on" )
	//				CProphet::TRANS_STRICT_BY_PRED = true;
	//			else if( val == "off" )
	//				CProphet::TRANS_STRICT_BY_PRED = false;
	//			else
	//				throw field;
	//			iField += 2;
	//		}
	//		else if( field == "-hotspot-similarity" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			string val(argv[iField + 1]);
	//			if( val == "on" )
	//				CHotspotSelect::TEST_HOTSPOT_SIMILARITY = true;
	//			else if( val == "off" )
	//				CHotspotSelect::TEST_HOTSPOT_SIMILARITY = false;
	//			else
	//				throw field;
	//			iField += 2;
	//		}
	//		else if( field == "-balanced-ratio" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			string val(argv[iField + 1]);
	//			if( val == "on" )
	//				HAR::TEST_BALANCED_RATIO = true;
	//			else if( val == "off" )
	//				HAR::TEST_BALANCED_RATIO = false;
	//			else
	//				throw field;
	//			iField += 2;
	//		}

	//		//int 参数
	//		else if( field == "-time-data" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			DATATIME = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;

	//			if( CNode::finiteEnergy() )
	//				RUNTIME = DATATIME = 999999;
	//		}
	//		else if( field == "-time-run" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			RUNTIME = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;

	//			if( CNode::finiteEnergy() )
	//				RUNTIME = DATATIME = 999999;
	//		}
	//		//else if( field == "-slot" )
	//		//{
	//		//	if( iField < argc - 1 )
	//		//		SLOT = atoi( argv[ iField + 1 ] );

	//		//	if( SLOT > CCTrace::SLOT_TRACE )
	//		//		SLOT = CCTrace::SLOT_TRACE;
	//		//	iField += 2;
	//		//}
	//		else if( field == "-node" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::INIT_NUM_NODE = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-log-slot" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			SLOT_LOG = strtol(argv[iField + 1], &endptr, 10);
	//			//				//观测周期不应小于工作周期
	//			//				if( SLOT_LOG < CNode::SLOT_TOTAL )
	//			//					SLOT_LOG = CNode::SLOT_TOTAL;

	//			iField += 2;
	//		}
	//		else if( field == "-trans-range" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CGeneralNode::RANGE_TRANS = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-trans-speed" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::SPEED_TRANS = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-lifetime" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CHotspotSelect::LIFETIME_POSITION = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-cycle" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::SLOT_TOTAL = strtol(argv[iField + 1], &endptr, 10);

	//			//观测周期不应小于工作周期
	//			if( SLOT_LOG < CNode::SLOT_TOTAL )
	//				SLOT_LOG = CNode::SLOT_TOTAL;

	//			iField += 2;
	//		}
	//		//else if( field == "-slot-carrier-sense" )
	//		//{
	//		//	if( ( iField + 1 ) >= argc )
	//		//		throw field;
	//		//	CNode::DEFAULT_SLOT_CARRIER_SENSE = strtol( argv[iField + 1], &endptr, 10);
	//		//	iField += 2;
	//		//}
	//		else if( field == "-dc-default" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::DEFAULT_DUTY_CYCLE = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-dc-hotspot" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::HOTSPOT_DUTY_CYCLE = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-hop" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CData::MAX_HOP = strtol(argv[iField + 1], &endptr, 10);
	//			//				if( ( CData::MAX_HOP > 0 )
	//			//					&& ( CData::MAX_TTL > 0 ) )
	//			//				{
	//			//					string error = "Error @ ParseConfiguration() : Argument -hop & -ttl cannot be used both";
	//			//					cout << error << endl;
	//			//					_PAUSE_;
	//			//					Exit(EINVAL, error);
	//			//				}

	//			iField += 2;
	//		}
	//		//			else if( field == "-ttl" )
	//		//			{
	//		//				if( iField < argc - 1 )
	//		//					CData::MAX_TTL = atoi( argv[ iField + 1 ] );
	//		//				if( ( CData::MAX_HOP > 0 )
	//		//					&& ( CData::MAX_TTL > 0 ) )
	//		//				{
	//		//					string error = "Error @ ParseConfiguration() : Argument -hop & -ttl cannot be used both";
	//		//					cout << error << endl;
	//		//					_PAUSE_;
	//		//					Exit(EINVAL, error);
	//		//				}
	//		//
	//		//				iField += 2;
	//		//			}
	//		else if( field == "-buffer" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::CAPACITY_BUFFER = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-buffer-ma" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CMANode::CAPACITY_BUFFER = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-data-rate" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::DEFAULT_DATA_RATE = double(1) / strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-data-size" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::SIZE_DATA = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-energy" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::CAPACITY_ENERGY = 1000 * strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;

	//			if( CNode::finiteEnergy() )
	//				RUNTIME = DATATIME = 999999;
	//		}
	//		//			else if( field == "-queue" )
	//		//			{
	//		//				if( iField < argc - 1 )
	//		//					CEpidemic::MAX_DATA_RELAY = atoi( argv[ iField + 1 ] );
	//		//				iField += 2;
	//		//			}			
	//		else if( field == "-spoken" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CNode::LIFETIME_SPOKEN_CACHE = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}
	//		else if( field == "-trans-window" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CRoutingProtocol::WINDOW_TRANS = strtol(argv[iField + 1], &endptr, 10);
	//			iField += 2;
	//		}


	//		//double 参数
	//		else if( field == "-alpha" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CPostSelect::ALPHA = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-beta" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			HAR::BETA = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-lambda" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CHotspotSelect::LAMBDA = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-merge" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CHotspotSelect::RATIO_MERGE_HOTSPOT = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-old" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CHotspotSelect::RATIO_OLD_HOTSPOT = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-trans-prob" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CGeneralNode::PROB_TRANS = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-pred-init" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CProphet::INIT_PRED = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		else if( field == "-pred-decay" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;
	//			CProphet::RATIO_PRED_DECAY = strtod(argv[iField + 1], &endptr);
	//			iField += 2;
	//		}
	//		//实际上对WSN而言不会使用到
	//		//			else if( field == "-pred-trans" )
	//		//			{
	//		//				if( iField < argc - 1 )
	//		//					CProphet::RATIO_PRED_TRANS = atof( argv[ iField + 1 ] );
	//		//				iField += 2;
	//		//			}

	//		//字符串参数
	//		else if( field == "-dataset" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;

	//			char arg[20] = { '\0' };
	//			strcpy(arg, argv[iField + 1]);
	//			DATASET = string(arg);

	//			iField += 2;
	//		}
	//		else if( field == "-log-path" )
	//		{
	//			if( ( iField + 1 ) >= argc )
	//				throw field;

	//			char arg[20] = { '\0' };
	//			strcpy(arg, argv[iField + 1]);
	//			PATH_ROOT = "../" + string(arg) + "/";

	//			iField += 2;
	//		}


	//		//带两个或以上数值的参数
	//		else if( field == "-sink" )
	//		{
	//			if( ( iField + 2 ) >= argc )
	//				throw field;
	//			CSink::SINK_X = strtod(argv[iField + 1], &endptr);
	//			CSink::SINK_Y = strtod(argv[iField + 2], &endptr);
	//			iField += 3;
	//		}
	//		else if( field == "-heat" )
	//		{
	//			if( ( iField + 2 ) >= argc )
	//				throw field;
	//			HAR::CO_HOTSPOT_HEAT_A1 = strtod(argv[iField + 1], &endptr);
	//			HAR::CO_HOTSPOT_HEAT_A2 = strtod(argv[iField + 2], &endptr);
	//			iField += 3;
	//		}


	//		//输出help信息
	//		else if( field == "-help" )
	//		{
	//			Help();
	//			_PAUSE_;
	//			Exit(ESKIP);
	//		}
	//		else
	//			throw argv[iField];

	//		if( endptr != nullptr
	//		   && *endptr != '\0' )
	//		{
	//			throw field;
	//		}

	//	}
	//}

	//// Unkown command name
	//catch( char* arg )
	//{
	//	stringstream error;
	//	error << "Error @ ParseConfiguration() : Cannot find command '" << arg << "' ! ";
	//	cout << endl << error.str() << endl;
	//	Help();
	//	_PAUSE_;
	//	Exit(EINVAL, error.str());
	//	return false;
	//}

	//// Wrong value format for command
	//catch( string field )
	//{
	//	stringstream error;
	//	error << "Error @ ParseConfiguration() : Wrong value for command '" << field << "' ! ";
	//	cout << endl << error.str() << endl;
	//	_PAUSE_;
	//	Exit(EINVAL, error.str());
	//	return false;
	//}

	//catch( exception e )
	//{
	//	stringstream error;
	//	error << "Error @ ParseConfiguration() : Unknown parse error ! ";
	//	cout << endl << error.str() << endl;
	//	Help();
	//	_PAUSE_;
	//	Exit(EINVAL, error.str());
	//	return false;
	//}
	return true;

}

bool CConfiguration::ParseConfiguration(vector<string> args, string description)
{
	return false;
}

bool CConfiguration::ParseConfiguration(string filename)
{
	if( ! CFileHelper::IfExists(filename) )
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Cannot find file \"" << filename << "\" ! ";
		cout << endl << error.str() << endl;
		_PAUSE_;
		Exit(ENOENT, error.str());
	}

	//read string from file
	ifstream file(filename, ios::in);
	string config(( std::istreambuf_iterator<char>(file) ), std::istreambuf_iterator<char>());

	//parse string into tokens
	string delim = " \t\n";
	vector<string> args;
	bool rtn = false;
	try
	{
		args = CParseHelper::ParseToken(config, delim);
	}
	catch( const char * str )
	{
		stringstream error;
		error << "Error @ ParseConfiguration() : Cannot find configuration in " << filename << "!";
		cout << endl << error.str() << endl;
		_PAUSE_;
		Exit(ENOEXEC, error.str());
		return false;
	}

	rtn = ParseConfiguration(args, filename);

	return rtn;
}
