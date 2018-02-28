#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"


class CHDC :
	virtual public CMacProtocol
{
private:

	CHDC();

	//检查所有Node，如果位于热点区域，更新占空比
	//注意：必须在调用 UpdateAtHotspotForNodes() 之后调用
	static void UpdateDutyCycleForNodes(int now);


public:

	~CHDC();

	static bool Prepare(int now);
	static bool Operate(int now);

	//打印相关信息到文件
	static void PrintInfo(int now);
	static void PrintFinal(int now);

};

#endif // __HDC_H__
