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

	CRunHelper() {};
	~CRunHelper() {};

	static void InitLogPath();
	static void Help();

	static void InitConfiguration();
	static void PrintConfiguration();

	static bool RunSimulation();

	static bool Run(int argc, char * argv[]);

};

#endif // __RUN_HELPER_H__