/***********************************************************************************************************************************

次级类 CAlgorithm ： （继承自顶级类 CProcess ）所有具体算法实现的单独成类，应该继承自这个类；除非属于路由或MAC层协议，则应该继承自CRoutingProtocol或CMacProtocol类

***********************************************************************************************************************************/

#pragma once

#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include "Process.h"


class CAlgorithm : 
	virtual public CProcess
{
public:

	CAlgorithm(){};
	virtual ~CAlgorithm() = 0
	{};

};

#endif // __ALGORITHM_H__
