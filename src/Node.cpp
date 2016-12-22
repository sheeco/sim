#include "Node.h"
#include "SortHelper.h"

vector<CNode &> CNode::nodes;
vector<int> CNode::idNodes;
vector<CNode &> CNode::deadNodes;
vector<CNode &> CNode::deletedNodes;


CNode::CNode()
{
}


CNode::~CNode()
{
}

vector<CNode &>& CNode::getNodes()
{
	assertInitialization();
	return nodes;
}

int CNode::getNNodes()
{
	return nodes.size();
}

vector<CNode &> CNode::getAllNodes(bool sort)
{
	vector<CNode &> allNodes = getNodes();
	allNodes.insert(allNodes.end(), deadNodes.begin(), deadNodes.end());
	allNodes.insert(allNodes.end(), deletedNodes.begin(), deletedNodes.end());
	if( sort )
		allNodes = CSortHelper::mergeSort(allNodes, CSortHelper::ascendByID);
	return allNodes;
}

vector<int>& CNode::getIdNodes()
{
	assertInitialization();
	return idNodes;
}

bool CNode::hasNodes(int currentTime)
{
	assertInitialization();

	idNodes.clear();

	bool death = false;
	for( vector<CNode &>::iterator inode = nodes.begin(); inode != nodes.end(); )
	{
		if( inode->isAlive() )
		{
			idNodes.push_back(inode->getID());
		}
		else
		{
			inode->die(currentTime, true);  //因节点能量耗尽而死亡的节点，仍可回收
			death = true;
		}
		++inode;
	}
	ClearDeadNodes(currentTime);
	if( death )
		flash_cout << "######  [ Node ]  " << CNode::getNodes().size() << "                                     " << endl;

	return ( !nodes.empty() );
}

void CNode::ClearDeadNodes(int currentTime)
{
	bool death = false;
	for( vector<CNode &>::iterator inode = nodes.begin(); inode != nodes.end(); )
	{
		if( inode->timeDeath > 0 )
		{
			death = true;
			deadNodes.push_back(*inode);
			inode = eraseNode(inode);
		}
		else
			++inode;
	}

	if( death )
	{
		ofstream death(PATH_ROOT + PATH_LOG + FILE_DEATH, ios::app);
		if( currentTime == 0 )
		{
			death << endl << INFO_LOG << endl;
			death << INFO_DEATH;
		}
		death << currentTime << TAB << CNode::getAllNodes(false).size() - CNode::getNNodes()
			<< TAB << CData::getCountDelivery() << TAB << CData::getDeliveryRatio() << endl;
		death.close();
	}

}

CNode & CNode::getNodeByID(int id)
{
	for( auto inode = nodes.begin(); inode != nodes.end(); ++inode )
	{
		if( inode->getID() == id )
			return *inode;
	}
	throw string("CNode::getNodeByID() : Cannot find Node " + STRING(id));
}

double CNode::getSumEnergyConsumption()
{
	double sumEnergyConsumption = 0;
	auto allNodes = getAllNodes(false);
	for( auto inode = allNodes.begin(); inode != allNodes.end(); ++inode )
		sumEnergyConsumption += inode->getEnergyConsumption();
	return sumEnergyConsumption;
}

void CNode::newNodes(int n)
{
	//优先恢复之前被删除的节点
	// TODO: 恢复时重新充满能量？
	for( int i = nodes.size(); i < nodes.size() + n; ++i )
	{
		if( deletedNodes.empty() )
			break;

		pushBackNode(deletedNodes[0]);
		--n;
	}
	//如果仍不足数，构造新的节点
	for( int i = nodes.size(); i < nodes.size() + n; ++i )
	{
		double dataRate = DEFAULT_DATA_RATE;
		if( i % 5 == 0 )
			dataRate *= 5;
		CNode* node = new CNode(dataRate);
		(*node).init();
		node->initDeliveryPreds();
		pushBackNode(*node);
		--n;
	}
}

void CNode::removeNodes(int n)
{
	//FIXME: Random selected ?
	vector<CNode &>::iterator start = nodes.begin();
	vector<CNode &>::iterator end = nodes.end();
	vector<CNode &>::iterator fence = nodes.begin();
	fence += nodes.size() - n;
	vector<CNode &> leftNodes(start, fence);

	//Remove invalid positoins belonging to the deleted nodes
	vector<CNode &> deletedNodes(fence, end);
	vector<int> deletedIDs;
	for( auto inode = deletedNodes.begin(); inode != deletedNodes.end(); ++inode )
		deletedIDs.push_back(inode->getID());

	for( vector<CPosition *>::iterator ipos = CPosition::positions.begin(); ipos != CPosition::positions.end(); )
	{
		if( IfExists(deletedIDs, ( *ipos )->getNode()) )
			ipos = CPosition::positions.erase(ipos);
		else
			++ipos;
	}

	nodes = leftNodes;
	idNodes.clear();
	for( auto inode = nodes.begin(); inode != nodes.end(); ++inode )
		idNodes.push_back(inode->getID());
	CNode::deletedNodes.insert(CNode::deletedNodes.end(), deletedNodes.begin(), deletedNodes.end());
}

void CNode::pushBackNode(CNode & node)
{
	nodes.push_back(node);
	idNodes.push_back(node.getID());
}

vector<CNode &>::iterator CNode::eraseNode(vector<CNode &>::iterator inode)
{
	vector<CNode &>::iterator ret;
	ret = nodes.erase(inode);
	RemoveFromList(idNodes, ( *inode ).getID());
	return ret;
}

bool CNode::checkInitialization()
{
	if( nodes.empty() && deadNodes.empty() )
		return false;
	return true;
}

bool CNode::assertInitialization()
{
	if( nodes.empty() && deadNodes.empty() )
		throw string("CNode::getNodes() : Un-initialized nodes");
	return true;
}

bool CNode::ifNodeExists(int id)
{
	for( vector<CNode &>::iterator inode = nodes.begin(); inode != nodes.end(); ++inode )
	{
		if( inode->getID() == id )
			return true;
	}
	return false;
}

int CNode::ChangeNodeNumber()
{
	int delta = 0;
	float bet = 0;
	do
	{
		bet = RandomFloat(-1, 1);
		if( bet > 0 )
			bet = 0.2 + bet / 2;  //更改比例至少 0.2
		else
			bet = -0.2 + bet / 2;
		delta = ROUND(bet * ( MAX_NUM_NODE - MIN_NUM_NODE ));
	} while( delta != 0 );

	if( delta < MIN_NUM_NODE - nodes.size() )
	{
		delta = nodes.size() - MIN_NUM_NODE;
		removeNodes(delta);
	}
	else if( delta > MAX_NUM_NODE - nodes.size() )
	{
		delta = MAX_NUM_NODE - nodes.size();
		newNodes(delta);
	}

	return delta;
}
