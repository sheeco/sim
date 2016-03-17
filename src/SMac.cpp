#include "SMac.h"

//CSMac::CSMac()
//{
//}
//
//
//CSMac::~CSMac()
//{
//}

void CSMac::DetectNeighborNode(CNode* node, int currentTime)
{
//	if( node->hasRTS() )
//		return;
//
//	vector<CNode*> dstNodes;
//	CPackage rts = node->sendRTS(currentTime);
//	dstNodes = CNode::broadcastPackage( rts );
//	for(vector<CNode*>::iterator idst = dstNodes.begin(); idst != dstNodes.end(); ++idst)
//	{
//		(*idst)->receiveRTS( rts );
//	}
}

bool CSMac::Operate(int currentTime)
{
	//Node Number Test:
	if( TEST_DYNAMIC_NUM_NODE )
		CMacProtocol::ChangeNodeNumber(currentTime);

//	for(vector<CNode*>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); ++inode)
//	{
//	}
		
	CMacProtocol::UpdateNodeStatus(currentTime);

	return true;
}
