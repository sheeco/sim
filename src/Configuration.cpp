#include "Configuration.h"
#include "FileHelper.h"
#include "ParseHelper.h"
#include "RunHelper.h"

map<string, pair<string, vector<CConfiguration::_FIELD_CONFIGURATION> > > CConfiguration::configurations;
string CConfiguration::KEYWORD_HELP = "-help";

//CConfiguration::CConfiguration(string keyword) : keyword(keyword)
//{
//}

void CConfiguration::setValue(void * pAttr, bool value)
{
	bool *pBool = static_cast< bool* >( pAttr );
	*pBool = value;
}

void CConfiguration::setValue(void * pAttr, int value)
{
	int *pInt = static_cast< int* >( pAttr );
	*pInt = value;
}

void CConfiguration::setValue(void * pAttr, double value)
{
	double *pDouble = static_cast< double* >( pAttr );
	*pDouble = value;
}

void CConfiguration::setValue(void * pAttr, string value)
{
	string *pString = static_cast< string* >( pAttr );
	*pString = value;
}

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

vector<string> CConfiguration::getConfiguration(int argc, char * argv[])
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

bool CConfiguration::ParseConfiguration(vector<string> args, string description)
{
	if( args.empty() )
		throw string( "CConfiguration::ParseConfiguration() : Parameters are requested ! ");

	// 将使用的命令行参数输出到文件
	ofstream log(PATH_ROOT + PATH_LOG + FILE_CONFIG, ios::app);
	log << "######  " << description << endl << endl;

	vector<string>::iterator iarg = args.begin();

	do
	{
		string keyword = *iarg;

		if( keyword == KEYWORD_HELP )
		{
			CRunHelper::Help();
			Exit(ESKIP);
		}

		map<string, pair<string, vector<_FIELD_CONFIGURATION> > >::iterator imap = configurations.find(keyword);

		//未定义的关键字
		if( imap == configurations.end() )
			throw string("CConfiguration::ParseConfiguration() : Cannot find command \"" + keyword + "\" !");

		stringstream strConfig;
		strConfig << keyword;
		string descriptionForKeyword = imap->second.first;
		vector<_FIELD_CONFIGURATION> fields = imap->second.second;
		int nField = fields.size();

		for(int i = 0; i < nField; ++i)
		{
			_FIELD_CONFIGURATION field = fields[i];
			string arg;
			if( field.type != _none
			   && (iarg + 1) == args.end() )
				throw string("CConfiguration::ParseConfiguration() : Wrong value for command \"" + keyword + "\" ! ("
							 + "Hint : " + descriptionForKeyword) + ")";
			
			try
			{
				bool *pBool = nullptr;
				int *pInt = nullptr;
				double *pDouble = nullptr;
				string *pString = nullptr;

				switch( field.type )
				{
					//不带参数
					case _none:

						setValue(field.dstAttr, field.defaultValue);

						break;

					//bool 参数
					case _bool:

						iarg++;
						arg = *iarg;
						pBool = static_cast< bool* >( field.dstAttr );
						setValue(pBool, CParseHelper::ParseBool(arg));

						break;

					//int 参数
					case _int:

						iarg++;
						arg = *iarg;
						pInt = static_cast< int* >( field.dstAttr );
						setValue(pInt, CParseHelper::ParseInt(arg));

						break;

					//double 参数
					case _double:

						iarg++;
						arg = *iarg;
						pDouble = static_cast< double* >( field.dstAttr );
						setValue(pDouble, CParseHelper::ParseDouble(arg));

						break;

					//string 参数
					case _string:

						iarg++;
						arg = *iarg;
						pString = static_cast< string* >( field.dstAttr );
						setValue(pString, arg);

						break;

					default:
						break;
				}
			}
			catch( exception e )
			{
				throw string("CConfiguration::ParseConfiguration : Cannot resolve argument \"" + arg + "\" ! \n"
							 + "Hint : " + descriptionForKeyword);
			}

			strConfig << " " << arg;
		}

		log << strConfig.str() << endl;
		iarg++;

	} while( iarg != args.end() );

	log << endl;
	log.close();
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

	rtn = ParseConfiguration(args, filename);

	return rtn;
}
