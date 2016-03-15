#pragma once

#include "MacProtocol.h"


class CSMac :
	public CMacProtocol
{
public:

	CSMac(void){};
	~CSMac(void){};

	static bool Operate(int currentTime);

//	//打印相关信息到文件
//	static void PrintInfo(int currentTime);
//	static void PrintFinal(int currentTime);

};

