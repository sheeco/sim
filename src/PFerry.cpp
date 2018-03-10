#include "PFerry.h"


int CPFerryTask::NUM_TASK_MET;
int CPFerryTask::NUM_TASK;

map<int, map<int, CTracePrediction*>> CPFerry::predictions;
int CPFerry::STARTTIME_PREDICTION = INVALID;
vector<int> CPFerry::strides;

vector<CPFerryMANode*> CPFerry::allMAs;
vector<CPFerryMANode*> CPFerry::freeMAs;
vector<CPFerryMANode*> CPFerry::busyMAs;

int CPFerry::INIT_NUM_MA;
int CPFerry::MAX_NUM_MA;

vector<CNode*> CPFerry::targetNodes;
//vector<CNode*> CPFerry::missNodes;
vector<CNode*> CPFerry::candidateNodes; //untargetted nodes
vector<CNode*> CPFerry::urgentNodes;

map<int, CPFerry::CNodeRecord> CPFerry::collectionRecords;  //known latest collection time for nodes


CPFerry::CPFerry()
{
}

CPFerry::~CPFerry()
{
}
