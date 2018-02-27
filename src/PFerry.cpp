#include "PFerry.h"


int CPFerryTask::NUM_TASK_MET;
int CPFerryTask::NUM_TASK;

map<int, map<int, CTracePrediction*>> CPFerry::predictions;
int CPFerry::STARTTIME_PREDICTION;
vector<int> CPFerry::strides;

vector<CPFerryMANode*> CPFerry::allMAs;
vector<CPFerryMANode*> CPFerry::freeMAs;
vector<CPFerryMANode*> CPFerry::busyMAs;

int CPFerry::INIT_NUM_MA;
int CPFerry::MAX_NUM_MA;

vector<CPFerryNode*> CPFerry::allNodes;
vector<CPFerryNode*> CPFerry::aliveNodes;
vector<CPFerryNode*> CPFerry::deadNodes;
vector<CPFerryNode*> CPFerry::targetNodes;
//vector<CPFerryNode*> CPFerry::missNodes;
vector<CPFerryNode*> CPFerry::candidateNodes; //untargetted nodes
vector<CPFerryNode*> CPFerry::urgentNodes;

map<CPFerryNode*, vector<CPosition>> CPFerry::collectionHistory;
map<CPFerryNode*, CPFerry::CNodeRecord> CPFerry::collectionRecords;  //known latest collection time for nodes


CPFerry::CPFerry()
{
}

CPFerry::~CPFerry()
{
	FreePointerVector(allNodes);
	FreePointerVector(allMAs);
	for( pair<int, map<int, CTracePrediction*>> p : predictions )
	{
		for( pair<int, CTracePrediction*> pp : p.second )
			FreePointer(pp.second);
	}
}
