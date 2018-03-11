#include "MANode.h"
#include "HAR.h"

int CMANode::COUNT_ID = 0;  //从getConfig<int>("ma", "base_id")开始，差值等于当前实例总数
int CMANode::INIT_NUM_MA;
int CMANode::MAX_NUM_MA;
int CMANode::CAPACITY_BUFFER;
double CMANode::SPEED;
int CMANode::encounter = 0;
//int CMANode::encounterActive = 0;

