#include "SMac.h"
#include "HotspotSelect.h"

//CSMac::CSMac()
//{
//}
//
//
//CSMac::~CSMac()
//{
//}

//void CSMac::DetectNeighborNode(CNode* node, int now)
//{
//	if( node->skipRTS() )
//		return;
//
//	vector<CNode*> dstNodes;
//	CFrame rts = node->sendRTS(now);
//	dstNodes = CNode::broadcastFrame( rts );
//	for(vector<CNode*>::iterator idst = dstNodes.begin(); idst != dstNodes.end(); ++idst)
//	{
//		(*idst)->receiveRTS( rts );
//	}
//}

bool CSMac::Operate(int now)
{
	CMacProtocol::CommunicateBetweenNeighbors(now);

	return true;
}
