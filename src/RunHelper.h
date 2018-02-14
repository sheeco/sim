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
	static bool PrepareSimulation(int argc, char* argv[]);
	static bool RunSimulation();

	static bool Run(int argc, char * argv[]);
	static bool Debug();
	//static bool Catch();


};

#endif // __RUN_HELPER_H__