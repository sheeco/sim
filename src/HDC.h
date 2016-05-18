#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"


class CHDC :
	public CMacProtocol
{
private:

	CHDC();

	//检查所有Node，如果位于热点区域，更新占空比
	//注意：必须在调用 UpdateAtHotspotForNodes() 之后调用
	static void UpdateDutyCycleForNodes(int currentTime);


public:

	~CHDC();

	static bool Prepare(int currentTime);
	static bool Operate(int currentTime);

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

#endif // __HDC_H__
