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


map<string, pair<string, vector<CConfiguration::_FIELD_CONFIGURATION> > > CConfiguration::configurations;
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

pair<string, vector<CConfiguration::_FIELD_CONFIGURATION>> CConfiguration::getConfiguration(string keyword)
{
	if( !ifExists(keyword) )
		throw string("CConfiguration::getConfiguration() : Cannot find configuration with keyword \"" + keyword + "\" !");
	return configurations[keyword];
}

void CConfiguration::AddConfiguration(string keyword, string description, vector<_FIELD_CONFIGURATION> fields)
{
	if( ifExists(keyword) )
		throw string("CConfiguration::AddConfiguration() : Configuration with keyword \"" + keyword + "\" already exists.");

	configurations[keyword] = pair<string, vector<_FIELD_CONFIGURATION> >(description, fields);
}

bool CConfiguration::AddConfiguration(string keyword, void * OnValue, bool defaultState, void * dstAttr, string description)
{
	vector<_FIELD_CONFIGURATION> fields;
	_FIELD_CONFIGURATION field = { _none, dstAttr, OnValue, defaultState};
	fields.push_back(field);
	AddConfiguration(keyword, description, fields);
	return true;
}

bool CConfiguration::AddConfiguration(string keyword, _TYPE_FIELD type, void * defaultValue, void * dstAttr, string description)
{
	if( type == _none )
		return false;

	vector<_FIELD_CONFIGURATION> fields;
	_FIELD_CONFIGURATION field = { type, dstAttr, defaultValue };
	fields.push_back(field);
	AddConfiguration(keyword, description, fields);
	return true;
}

bool CConfiguration::AddConfiguration(string keyword, _TYPE_FIELD type_1, void * default_1, void * dstAttr_1, _TYPE_FIELD type_2, void * default_2, void * dstAttr_2, string description)
{
	if( type_1 == _none
	   || type_2 == _none )
		return false;

	vector<_FIELD_CONFIGURATION> fields;
	_FIELD_CONFIGURATION field_1 = { type_1, dstAttr_1, default_1 };
	_FIELD_CONFIGURATION field_2 = { type_2, dstAttr_2, default_2 };
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
	vector<_FIELD_CONFIGURATION> fields = getConfiguration(keyword).second;
	int nargv = fields.size();
	if( nargv == 1 &&
	   fields[0].type == _none )
		nargv = 0;

	auto ifield = fields.begin();
	auto iargv = argvs.begin();
	for( ; ifield != fields.end(); ++ifield )
	{
		_FIELD_CONFIGURATION field = *ifield;

		if(field.type == _none )
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
					case _none:
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
		vector<_FIELD_CONFIGURATION> fields = getConfiguration(keyword).second;
		int nField = fields.size();
		vector<string> argvs;

		for(int i = 0; i < nField; ++i)
		{
			_FIELD_CONFIGURATION field = fields[i];
			string argv;
			if( field.type != _none
			   && (iarg + 1) == args.end() )
				throw string("CConfiguration::ParseConfiguration() : Wrong value for command \"" + keyword + "\" ! ("
							 + "Hint : " + descriptionForKeyword) + ")";
			
			if( field.type != _none )
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
		throw pair<int, string>(EPARSE, string("CConfiguration::ParseConfiguration() : Cannot find configuration in " + filename + "!"));
	}

	rtn = ParseConfiguration(args);

	return rtn;
}

void CConfiguration::InitConfiguration()
{
	//不带参数的命令
	// TODO: define option-like config for these configs
	AddConfiguration("--smac", new _MAC_PROTOCOL(_smac), true, &MAC_PROTOCOL, "");
	AddConfiguration("--hdc", new _MAC_PROTOCOL(_hdc), false, &MAC_PROTOCOL, "");
	AddConfiguration("--prophet", new _ROUTING_PROTOCOL(_prophet), false, &ROUTING_PROTOCOL, "");
	AddConfiguration("--har", new _ROUTING_PROTOCOL(_xhar), true, &ROUTING_PROTOCOL, "");
	AddConfiguration("--hs", new _HOTSPOT_SELECT(_original), true, &HOTSPOT_SELECT, "");
	AddConfiguration("--ihs", new _HOTSPOT_SELECT(_improved), false, &HOTSPOT_SELECT, "");
	AddConfiguration("--mhs", new _HOTSPOT_SELECT(_merge), false, &HOTSPOT_SELECT, "");

	//bool/int 参数
	AddConfiguration("-continuous-trace", _bool, new bool(true), &CCTrace::CONTINUOUS_TRACE, "");
	AddConfiguration("-dc-sync", _bool, new bool(true), &CMacProtocol::SYNC_DC, "");
	AddConfiguration("-dynamic-node-number", _bool, new bool(false), &CMacProtocol::TEST_DYNAMIC_NUM_NODE, "");
	AddConfiguration("-pred-strict", _bool, new bool(true), &CProphet::TRANS_STRICT_BY_PRED, "");
	AddConfiguration("-hotspot-similarity", _bool, new bool(true), &CHotspotSelect::TEST_HOTSPOT_SIMILARITY, "");
	AddConfiguration("-balanced-ratio", _bool, new bool(false), &HAR::TEST_BALANCED_RATIO, "");
	AddConfiguration("-time-data", _int, new int(15000), &DATATIME, "");
	AddConfiguration("-time-run", _int, new int(15000), &RUNTIME, "");
	AddConfiguration("-slot", _int, new int(1), &SLOT, "");
	AddConfiguration("-slot-trace", _int, new int(30), &CCTrace::SLOT_TRACE, "");
	AddConfiguration("-node-min", _int, new int(0), &CNode::MIN_NUM_NODE, "");
	AddConfiguration("-node-max", _int, new int(0), &CNode::MAX_NUM_NODE, "");
	AddConfiguration("-log-slot", _int, new int(100), &SLOT_LOG, "");
	AddConfiguration("-trans-range", _int, new int(100), &CGeneralNode::RANGE_TRANS, "");
	AddConfiguration("-trans-speed", _int, new int(2500), &CNode::SPEED_TRANS, "");
	AddConfiguration("-lifetime", _int, new int(0), &CHotspotSelect::LIFETIME_POSITION, "");
	AddConfiguration("-cycle", _int, new int(0), &CNode::SLOT_TOTAL, "");
	AddConfiguration("-slot-carrier-sense", _int, new int(0), &CNode::DEFAULT_SLOT_CARRIER_SENSE, "");
	AddConfiguration("-hop", _int, new int(0), &CData::MAX_HOP, "");
	AddConfiguration("-buffer", _int, new int(0), &CNode::CAPACITY_BUFFER, "");
	AddConfiguration("-buffer-ma", _int, new int(100), &CMANode::CAPACITY_BUFFER, "");
	AddConfiguration("-data-size", _int, new int(200), &CNode::SIZE_DATA, "");
	AddConfiguration("-energy", _int, new int(0), &CNode::CAPACITY_ENERGY, "");
	AddConfiguration("-spoken", _int, new int(0), &CNode::LIFETIME_SPOKEN_CACHE, "");
	AddConfiguration("-trans-window", _int, new int(10), &CRoutingProtocol::WINDOW_TRANS, "");

	//double 参数
	AddConfiguration("-data-rate", _double, new double(6.8), &CNode::DEFAULT_DATA_RATE, "");  // 200B/30s
	AddConfiguration("-dc-default", _double, new double(1.0), &CNode::DEFAULT_DUTY_CYCLE, "");
	AddConfiguration("-dc-hotspot", _double, new double(0), &CNode::HOTSPOT_DUTY_CYCLE, "");
	AddConfiguration("-alpha", _double, new double(0.03), &CPostSelect::ALPHA, "");
	AddConfiguration("-beta", _double, new double(0.0025), &HAR::BETA, "");
	AddConfiguration("-lambda", _double, new double(0), &CHotspotSelect::LAMBDA, "");
	AddConfiguration("-merge", _double, new double(1.0), &CHotspotSelect::RATIO_MERGE_HOTSPOT, "");
	AddConfiguration("-old", _double, new double(1.0), &CHotspotSelect::RATIO_OLD_HOTSPOT, "");
	AddConfiguration("-trans-prob", _double, new double(1.0), &CGeneralNode::PROB_TRANS, "");
	AddConfiguration("-pred-init", _double, new double(0.7), &CProphet::INIT_PRED, "");
	AddConfiguration("-pred-decay", _double, new double(0.9), &CProphet::RATIO_PRED_DECAY, "");

	//string 参数
	AddConfiguration("-log-path", _string, new string("../log/"), &DIR_LOG, "");
	AddConfiguration("-path-trace", _string, new string("../res/NCSU"), &CCTrace::PATH_TRACE, "");
	AddConfiguration("-ext-trace", _string, new string(".trace"), &CCTrace::EXTENSION_TRACE, "");

	//带双参数的命令
	AddConfiguration("-sink", _double, new double(0), &CSink::SINK_X, _double, new double(0), &CSink::SINK_Y, "");
	AddConfiguration("-heat", _double, new double(1), &HAR::CO_HOTSPOT_HEAT_A1, _double, new double(30), &HAR::CO_HOTSPOT_HEAT_A2, "");


	/*********************************************  按照命令格式解析参数配置  *********************************************/

	ParseConfiguration(DIR_RUN + FILE_DEFAULT_CONFIG);

}

void CConfiguration::ValidateConfiguration()
{
	if( CNode::finiteEnergy() )
		RUNTIME = DATATIME = 999999;

	if( ( ROUTING_PROTOCOL == _xhar
		 || MAC_PROTOCOL == _hdc )
	   && HOTSPOT_SELECT == _none )
		HOTSPOT_SELECT = _original;

}

bool CConfiguration::ApplyConfigurations()
{
	for( auto config : configurations )
	{
		string keyword = config.first;
		string description = config.second.first;
		vector<_FIELD_CONFIGURATION> fields = config.second.second;
		for( _FIELD_CONFIGURATION field : fields )
		{
			int *pInt = nullptr;
			bool *pBool = nullptr;
			double *pDouble = nullptr;
			string *pString = nullptr;

			switch( field.type )
			{
				case _none:
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
	ofstream log(DIR_LOG + PATH_TIMESTAMP + FILE_CONFIG, ios::ate);

	for( auto config : configurations )
	{
		string keyword = config.first;
		string description = config.second.first;

		log << keyword << TAB;
		vector<_FIELD_CONFIGURATION> fields = config.second.second;
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
					case _none:
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
	ofstream parameters(DIR_LOG + PATH_TIMESTAMP + FILE_PARAMETES, ios::app);
	parameters << endl << endl << INFO_LOG << endl << endl;

	parameters << "CYCLE" << TAB << CNode::SLOT_TOTAL << endl;
	parameters << "DEFAULT_DC" << TAB << CNode::DEFAULT_DUTY_CYCLE << endl;

	if( CData::useHOP() )
		parameters << "HOP" << TAB << CData::MAX_HOP << endl;
	//	else
	//		parameters << "TTL" << TAB << CData::MAX_TTL << endl;
	parameters << "DATA_RATE" << TAB << CNode::SIZE_DATA << "B / " << CNode::SIZE_DATA / CNode::DEFAULT_DATA_RATE << "s" << endl;
	parameters << "DATA_SIZE" << TAB << CNode::SIZE_DATA << endl;
	parameters << "BUFFER" << TAB << CNode::CAPACITY_BUFFER << endl;
	parameters << "ENERGY" << TAB << CNode::CAPACITY_ENERGY << endl;

	parameters << "DATA_TIME" << TAB << DATATIME << endl;
	parameters << "RUN_TIME" << TAB << RUNTIME << endl;
	parameters << "PROB_TRANS" << TAB << CGeneralNode::PROB_TRANS << endl;
	parameters << "WINDOW_TRANS" << TAB << CProphet::WINDOW_TRANS << endl;


	//输出文件为空时，输出文件头
	ofstream final(DIR_LOG + PATH_TIMESTAMP + FILE_FINAL, ios::app);
	final.seekp(0, ios::end);
	if( !final.tellp() )
		final << INFO_FINAL;

	final << DATATIME << TAB << RUNTIME << TAB << CGeneralNode::PROB_TRANS << TAB << CNode::CAPACITY_BUFFER << TAB << CNode::CAPACITY_ENERGY << TAB;
	if( CData::useHOP() )
		final << CData::MAX_HOP << TAB;

	final << CNode::SLOT_TOTAL << TAB << CNode::DEFAULT_DUTY_CYCLE << TAB;

	//	if( ROUTING_PROTOCOL == _epidemic )
	//	{
	//		INFO_LOG += "$Epidemic ";
	//		parameters << "$Epidemic " << endl << endl;
	//	}
	//	else 

	if( ROUTING_PROTOCOL == _prophet )
	{
		INFO_LOG += "$Prophet ";
		parameters << endl << "$Prophet " << endl << endl;
		parameters << "PRED_INIT" << TAB << CProphet::INIT_PRED << endl;
		parameters << "PRED_DECAY" << TAB << CProphet::RATIO_PRED_DECAY << endl;
		parameters << "PRED_TRANS" << TAB << CProphet::RATIO_PRED_TRANS << endl;
	}

	if( MAC_PROTOCOL == _hdc )
	{
		INFO_LOG += "$HDC ";
		parameters << endl << "$HDC " << endl << endl;
		parameters << "HOTSPOT_DC" << TAB << CNode::HOTSPOT_DUTY_CYCLE << endl;
		final << CNode::HOTSPOT_DUTY_CYCLE << TAB;
	}

	if( HOTSPOT_SELECT != _none )
	{
		if( HOTSPOT_SELECT == _improved )
		{
			INFO_LOG += "$iHS ";
			parameters << endl << "$iHS " << endl << endl;
			parameters << "POSITION_LIFETIME" << TAB << CHotspotSelect::LIFETIME_POSITION << endl << endl;

			final << CHotspotSelect::LIFETIME_POSITION << TAB;
		}

		else if( HOTSPOT_SELECT == _merge )
		{
			INFO_LOG += "$mHS ";
			parameters << endl << "$mHS " << endl << endl;
			parameters << "RATIO_MERGE" << TAB << CHotspotSelect::RATIO_MERGE_HOTSPOT << endl;
			parameters << "RATIO_NEW" << TAB << CHotspotSelect::RATIO_NEW_HOTSPOT << endl;
			parameters << "RATIO_OLD" << TAB << CHotspotSelect::RATIO_OLD_HOTSPOT << endl;

			final << CHotspotSelect::RATIO_MERGE_HOTSPOT << TAB << CHotspotSelect::RATIO_OLD_HOTSPOT << TAB;

		}

		else
		{
			INFO_LOG += "$HS ";
			parameters << endl << "$HS " << endl << endl;
		}

		parameters << "ALPHA" << TAB << CPostSelect::ALPHA << endl;
		//final << CPostSelect::ALPHA << TAB ;
	}

	if( ROUTING_PROTOCOL == _xhar )
	{
		INFO_LOG += "$xHAR ";
		parameters << endl << "$xHAR " << endl << endl;
		parameters << "BETA" << TAB << HAR::BETA << endl;
		parameters << "HEAT_CO_1" << TAB << HAR::CO_HOTSPOT_HEAT_A1 << endl;
		parameters << "HEAT_CO_2" << TAB << HAR::CO_HOTSPOT_HEAT_A2 << endl;
		//final << HAR::BETA << TAB;
	}

	if( CMacProtocol::TEST_DYNAMIC_NUM_NODE )
	{
		INFO_LOG += "#DYNAMIC_NODE_NUMBER";
		parameters << endl << "#DYNAMIC_NODE_NUMBER" << endl;
	}

	parameters << endl;

	parameters.close();
	final.close();

	// 输出版本信息

	ofstream version(DIR_LOG + PATH_TIMESTAMP + FILE_VERSION, ios::out);
#define __VERSIONING_CONTROLLED_BUILD__
#ifdef __VERSIONING_CONTROLLED_BUILD__
	ifstream in_version(DIR_ROOT + FILE_VERSION, ios::in);
	version << in_version.rdbuf();
	in_version.close();
#endif
#ifdef __MANUAL_VERSIONING__
	version << _STRING(FILE_VERSION_STR);
#endif
	version.close();

}

inline void CConfiguration::Help()
{
	CPrintHelper::PrintFile(DIR_RUN + FILE_HELP, "");
	_PAUSE_;
}
