#pragma once

#ifndef __S_MAC_H__
#define __S_MAC_H__

#include "MacProtocol.h"
#include "Node.h"


class CSMac :
	virtual public CMacProtocol
{
public:

	CSMac(){};
	~CSMac(){};

};

#endif // __S_MAC_H__
