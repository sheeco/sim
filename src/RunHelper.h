#pragma once

#ifndef __RUN_HELPER_H__
#define __RUN_HELPER_H__

#include "Helper.h"
#include "Global.h"

//
class CRunHelper :
	virtual public CHelper
{
public:

	CRunHelper();
	virtual ~CRunHelper()
	{};

	static string KEYWORD_TRUE;
	static string KEYWORD_FALSE;


	static int ParseInt(const char * str);
	static int ParseInt(string str);
	static double ParseDouble(const char * str);
	static double ParseDouble(string str);
	static bool ParseBool(const char * str);
	static bool ParseBool(string str);
	static vector<string> ParseToken(const char * str, const char * delim);
	static vector<string> ParseToken(string str, string delim);

	static void InitLogPath();
	static void Help();

	static void InitConfiguration();
	static bool ParseConfiguration(int argc, char * argv[], string description);
	static bool ParseConfiguration(vector<string> args, string description);
	static bool ParseConfiguration(string filename);
	static void PrintConfiguration();

	static bool RunSimulation();

	static bool Run(int argc, char * argv[]);

};

#endif // __RUN_HELPER_H__