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
	//Node Number Test:
	if( TEST_DYNAMIC_NUM_NODE )
		CMacProtocol::ChangeNodeNumber(currentTime);

//	//独立开启热点选取操作
//	if( ROUTING_PROTOCOL != _xhar
//		&& HOTSPOT_SELECT != _none )
//	{
//		CHotspotSelect::HotspotSelect(currentTime);
//	}

	CommunicateWithNeighbor(currentTime);

	return true;
}
