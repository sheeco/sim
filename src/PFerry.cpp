#include "PFerry.h"


int CPFerryTask::NUM_TASK_MET;
int CPFerryTask::NUM_TASK;

map<int, map<int, CTracePrediction*>> CPFerry::predictions;
int CPFerry::STARTTIME_PREDICTION = INVALID;
vector<int> CPFerry::strides;

vector<CPFerryMANode*> CPFerry::allMAs;
vector<CPFerryMANode*> CPFerry::freeMAs;
vector<CPFerryMANode*> CPFerry::busyMAs;

vector<CNode*> CPFerry::targetNodes;
//vector<CNode*> CPFerry::missNodes;
vector<CNode*> CPFerry::candidateNodes; //untargetted nodes

map<int, CPFerry::CNodeRecord> CPFerry::collectionRecords;  //known latest collection time for nodes


string CPFerry::PATH_PREDICT;
config::EnumRoutingProtocolScheme CPFerry::PRESTAGE_PROTOCOL = config::_xhar;


CPFerryMANode::~CPFerryMANode()
{
	this->tasks.clear();
	for( pair<int, CPFerryTask*> p : tasks )
		FreePointer(p.second);
	FreePointerVector(this->taskHistory);
}

void CPFerryMANode::updateStatus(int now)
{
	if( this->time < 0 )
		this->time = now;

	/********************************* Vacant MA ************************************/
	
	if( !this->isBusy() )
	{
		if( !CBasicEntity::withinRange(*this, *CSink::getSink(), getConfig<int>("trans", "range_trans")) )
			throw string("CPFerryMANode::updateStatus(): " + this->getName() + " is free but not around Sink.");

		this->setTime(now);
		return;
	}

	/********************************* Busy MA ************************************/

	int duration = now - this->time;

	//updateTimerOccupied(time);

	//如果缓存已满，立即返回sink
	if( !this->isReturningToSink()
	   && this->isFull() )
	{
		CPrintHelper::PrintDetail(time, this->getName() + " is returning to Sink due to buffer filled.", 2);
		this->setReturningToSink();
	}

	/********************* Returning to Sink **********************/

	if( isReturningToSink() )
	{
		this->moveToward(*CSink::getSink(), duration, this->getSpeed());
		this->setTime(now);
		return;
	}

	/********************* Moving on the Route **********************/

	/***************** Waiting ****************/

	int timeLeftAfterWaiting = 0;
	if( this->isWaiting() )
	{
		CBasicEntity* waypoint = getAtWaypoint();
		int copyWaitingWindow = this->waitingWindow;
		timeLeftAfterWaiting = this->wait(duration);

		//如果等待已经结束，记录一次等待
		if( !this->isWaiting() )
		{
			if( waypoint == nullptr )
				throw string("CPFerryMANode::updateStatus(): waypoint = nullptr");

			CPrintHelper::PrintDetail(this->time + duration - timeLeftAfterWaiting
									  , this->getName() + " has waited at " + waypoint->format() + " for " 
									  + STRING(copyWaitingWindow) + "s.", 3);
		}
	}
	else
	{
		timeLeftAfterWaiting = duration;
	}

	if( timeLeftAfterWaiting == 0 )
	{
		this->setTime(now);
		return;
	}

	/***************** Actually Moving ****************/

	this->setTime(now - timeLeftAfterWaiting);  //将时间置于等待结束，移动即将开始的时间点
	atPoint = nullptr;  //离开之前的路点

	CBasicEntity *toPoint = route->getToPoint();
	int minWaitingTime = route->getWaitingTime();
	int timeLeftAfterArrival = this->moveToward(*toPoint, timeLeftAfterWaiting, this->getSpeed());

	//如果已到达目的地
	if( timeLeftAfterArrival >= 0 )
	{
		CSink *psink = nullptr;

		//若目的地的类型是waypoint
		CPosition *ppos = nullptr;
		if( ( ppos = dynamic_cast< CPosition * >( toPoint ) ) != nullptr )
		{
			this->atPoint = toPoint;

			CPrintHelper::PrintDetail(now, this->getName() + " arrives at task position " + ppos->format() + ".", 2);

			CPFerryTask* task = this->findTask(ppos->getNode());
			if( task && !task->isMet() )
			{
				//set waiting time
				int timePred = task->getTime();
				this->setWaiting(max(minWaitingTime, timePred - this->getTime() + minWaitingTime));
			}
			else if( !task )
				throw string("CPFerryMANode::updateStatus(): Cannot find task for node id" + STRING(ppos->getNode()) + ".");
		}
		//若目的地的类型是 sink
		else if( ( psink = dynamic_cast< CSink * >( toPoint ) ) != nullptr )
		{
			CPrintHelper::PrintDetail(time, this->getName() + " has returned to Sink.", 2);
			this->setReturningToSink();
		}
	}
	return;
}

CPFerry::CPFerry()
{
}

CPFerry::~CPFerry()
{
}

