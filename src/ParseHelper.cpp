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
		throw string("CParseHelper::ParseInt : Cannot parse \"" + string(str) + "\" into int !");

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
		throw string("CParseHelper::ParseDouble : Cannot parse \"" + string(str) + "\" into double !");

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
		throw string("CParseHelper::ParseBool : Cannot parse \"" + string(str) + "\" into bool !");

	return rtn;
}

bool CParseHelper::ParseBool(string str)
{
	return ParseBool(str.c_str());
}

string CParseHelper::ParseString(string str)
{
	if( str.size() >= 2 )
	{
		if( ( str[0] == '\'' && str[str.size() - 1] == '\'' )
		   || ( str[0] == '"' && str[str.size() - 1] == '"' ) )
		{
			string ret = str.substr(1, str.size() - 2);
			return ret;
		}
	}
	return str;
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
		throw string("CParseHelper::ParseToken : Cannot parse any token from \"" + string(str) + "\" !");

	return tokens;
}

vector<string> CParseHelper::ParseToken(string str, string delim)
{
	return ParseToken(str.c_str(), delim.c_str());
}

void CParseHelper::test()
{
	string str1 = "test string", str2 = "\'test string\'", str3 = "\"test string\"", str4 = "test string\"";
	ASSERT(CParseHelper::ParseString(str1) == str1);
	ASSERT(CParseHelper::ParseString(str2) == str1);
	ASSERT(CParseHelper::ParseString(str3) == str1);
	ASSERT(CParseHelper::ParseString(str4) == str4);
}
