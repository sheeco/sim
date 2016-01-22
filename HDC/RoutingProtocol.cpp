#include "RoutingProtocol.h"
#include "Node.h"

//CRoutingProtocol::CRoutingProtocol(void)
//{
//}
//
//
//CRoutingProtocol::~CRoutingProtocol(void)
//{
//}

void CRoutingProtocol::GenerateData(int currentTime)
{
	for(vector<CNode *>::iterator inode = CNode::getNodes().begin(); inode != CNode::getNodes().end(); inode++)
	{
		(*inode)->generateData(currentTime);
	}
};