#include "Sink.h"

CSink* CSink::sink = nullptr;
int CSink::SINK_ID = 0; //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ
double CSink::SINK_X = 0;
double CSink::SINK_Y = 0;
int CSink::BUFFER_CAPACITY = 999999;  //������

