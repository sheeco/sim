#pragma once

#ifndef __HDC_H__
#define __HDC_H__

#include "MacProtocol.h"
#include "DutyCycle.h"


class CHDC :
	virtual public CMacProtocol
{
private:

	CHDC();

	//在热点处提高 dc
	static void raiseDutyCycle(CDutyCycle* node);
	//在非热点处降低 dc
	static void resetDutyCycle(CDutyCycle* node);
	//检查所有Node，如果位于热点区域，更新占空比
	//注意：必须在调用 UpdateAtHotspotForNodes() 之后调用
	static void UpdateDutyCycleForNodes(int currentTime);


public:

	static double HOTSPOT_DUTY_CYCLE;  //HDC中热点区域内的占空比

	~CHDC();

	static bool Prepare(int currentTime);
	static bool Operate(int currentTime);

	//打印相关信息到文件
	static void PrintInfo(int currentTime);
	static void PrintFinal(int currentTime);

};

#endif // __HDC_H__
