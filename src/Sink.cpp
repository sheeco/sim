#include "Sink.h"

int CSink::SINK_ID = 0; //0为sink节点预留，传感器节点ID从1开始
double CSink::SINK_X = 0;
double CSink::SINK_Y = 0;
int CSink::BUFFER_CAPACITY = 999999;  //无限制
CSink* CSink::sink = nullptr;

