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

//void CSMac::DetectNeighborNode(CNode* node, int currentTime)
//{
//	if( node->skipRTS() )
//		return;
//
//	vector<CNode*> dstNodes;
//	CPackage rts = node->sendRTS(currentTime);
//	dstNodes = CNode::broadcastPackage( rts );
//	for(vector<CNode*>::iterator idst = dstNodes.begin(); idst != dstNodes.end(); ++idst)
//	{
//		(*idst)->receiveRTS( rts );
//	}
//}

bool CSMac::Operate(int currentTime)
{
	CMacProtocol::CommunicateWithNeighbor(currentTime);

	return true;
}
