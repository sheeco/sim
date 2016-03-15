#include "SMac.h"


//CSMac::CSMac(void)
//{
//}
//
//
//CSMac::~CSMac(void)
//{
//}

bool CSMac::Operate(int currentTime)
{
	//Node Number Test:
	if( TEST_DYNAMIC_NUM_NODE )
		CMacProtocol::ChangeNodeNumber(currentTime);

	CMacProtocol::UpdateNodeStatus(currentTime);

	return true;
}