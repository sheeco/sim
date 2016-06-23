#include "ParseHelper.h"

string CParseHelper::KEYWORD_TRUE = "on";
string CParseHelper::KEYWORD_FALSE = "off";


int CParseHelper::ParseInt(const char * str)
{
	int rtn = 0;
	char *endptr = nullptr;
	rtn = strtol(str, &endptr, 10);
	if( endptr != nullptr
	   && *endptr != '\0' )
		throw str;

	return rtn;
}

int CParseHelper::ParseInt(string str)
{
	return ParseInt(str.c_str());
}

double CParseHelper::ParseDouble(const char * str)
{
	double rtn = 0;
	char *endptr = nullptr;
	rtn = strtod(str, &endptr);
	if( endptr != nullptr
	   && *endptr != '\0' )
		throw str;

	return rtn;
}

double CParseHelper::ParseDouble(string str)
{
	return ParseDouble(str.c_str());
}

bool CParseHelper::ParseBool(const char * str)
{
	bool rtn = false;
	if( strcmp(str, KEYWORD_TRUE.c_str()) == 0 )
		rtn = true;
	else if( strcmp(str, KEYWORD_FALSE.c_str()) == 0 )
		rtn = false;
	else
		throw str;

	return rtn;
}

bool CParseHelper::ParseBool(string str)
{
	return ParseBool(str.c_str());
}

//parse string into tokens
vector<string> CParseHelper::ParseToken(const char * str, const char * delim)
{
	vector<string> tokens;
	char* pToken = strtok(const_cast<char*>( str ), delim);

	while( pToken != nullptr )
	{
		tokens.push_back(string(pToken));
		pToken = strtok(nullptr, delim);
	}

	if( tokens.empty() )
		throw str;

	return tokens;
}

vector<string> CParseHelper::ParseToken(string str, string delim)
{
	return ParseToken(str.c_str(), delim.c_str());
}
