#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"


//HDC协议(基于热点的Duty Cycle)
class CHDC :
	virtual public CMacProtocol
{
private:

	static double HOTSPOT_DUTY_RATE;
	CHDC();

	//检查所有Node，如果位于热点区域，更新占空比
	//注意：必须在调用 UpdateAtHotspotForNodes() 之后调用
	static void UpdateDutyCycleForNodes(int now);
	static bool isUsingHotspotDutyCycle(CNode* node);


public:

	~CHDC();

	static bool Init();
	static bool Prepare(int now);

	//打印相关信息到文件
	static void PrintInfo(int now);
	static void PrintFinal(int now);

};

#endif // __HDC_H__
