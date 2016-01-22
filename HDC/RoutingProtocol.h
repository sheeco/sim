/***********************************************************************************************************************************

次级类 CRoutingProtocol ：（继承自顶级类 CProtocol ）所有路由协议算法的实现类，都应该继承自这个类

***********************************************************************************************************************************/

#pragma once

#include "protocol.h"

using namespace std;

class CRoutingProtocol :
	public CProtocol
{
protected:
	
	static void GenerateData(int currentTime);


public:

	CRoutingProtocol(void){};
	~CRoutingProtocol(void){};
};

