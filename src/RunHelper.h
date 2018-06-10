#pragma once

#ifndef __RUN_HELPER_H__
#define __RUN_HELPER_H__

#include "Global.h"
#include "Process.h"

//仿真运行的辅助类
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

	static void Exit(int code);
	static void Exit(int code, string error);

};

#endif // __RUN_HELPER_H__