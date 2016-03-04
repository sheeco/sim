#include "Sink.h"

int CSink::SINK_ID = 0; //0为sink节点预留，传感器节点ID从1开始
int CSink::BUFFER_CAPACITY = 99999999999;  //无限制
CSink* CSink::sink = nullptr;

