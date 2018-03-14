#pragma once

#ifndef __PARSE_HELPER_H__
#define __PARSE_HELPER_H__

#include "Process.h"
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

	template <class T>
	static T* TryCast(void *pVoid)
	{
		T *pT = nullptr;
		try
		{
			pT = static_cast< T* >( pVoid );
		}
		catch( exception error )
		{
			throw string("CParseHelper::TryCast(): ") + string(error.what());
		}
		return pT;
	}

	static int ParseInt(const char * str);
	static int ParseInt(string str);
	static double ParseDouble(const char * str);
	static double ParseDouble(string str);
	static bool ParseBool(const char * str);
	static bool ParseBool(string str);
	static string ParseString(string str);
	static vector<string> ParseToken(const char * str, const char * delim);
	static vector<string> ParseToken(string str, string delim);

	static void test();
};

#endif // __PARSE_HELPER_H__