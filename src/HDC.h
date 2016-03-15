#pragma once

#include "MacProtocol.h"


class CHDC :
	public CMacProtocol
{
private:

	//检查所有Node，如果位于热点区域，更新占空比
	static void UpdateDutyCycleForNodes(int currentTime);


public:

	CHDC(){};
	~CHDC(){};

	static bool Operate(int currentTime);

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

