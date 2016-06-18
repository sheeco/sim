/***********************************************************************************************************************************

基类 CProtocol ： （继承自 CProcess > CAlgorithm ）与CAlgorithm类似，但专用于网络协议算法，所有协议算法的实现类，都应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "Algorithm.h"


class CProtocol : 
	virtual public CAlgorithm
{
public:

	CProtocol(){};
	virtual ~CProtocol() = 0
	{};

};

#endif // __PROTOCOL_H__
