#pragma once

#ifndef __PARSE_HELPER_H__
#define __PARSE_HELPER_H__

#include "Helper.h"
#include "Global.h"

//
class CParseHelper :
	public CHelper
{
public:

	static string KEYWORD_TRUE;
	static string KEYWORD_FALSE;

	CParseHelper() {};
	~CParseHelper() {};

	static int ParseInt(const char * str);
	static int ParseInt(string str);
	static double ParseDouble(const char * str);
	static double ParseDouble(string str);
	static bool ParseBool(const char * str);
	static bool ParseBool(string str);
	static vector<string> ParseToken(const char * str, const char * delim);
	static vector<string> ParseToken(string str, string delim);

};

#endif // __PARSE_HELPER_H__