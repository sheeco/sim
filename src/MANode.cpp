#include "MANode.h"
#include "HAR.h"

int CMANode::COUNT_ID = 0;  //从getConfig<int>("ma", "base_id")开始，差值等于当前实例总数
int CMANode::INIT_NUM_MA = INVALID;
int CMANode::MAX_NUM_MA = INVALID;
int CMANode::CAPACITY_BUFFER = INVALID;
double CMANode::SPEED = INVALID;
int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;

