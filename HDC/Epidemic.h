#pragma once

#include "RoutingProtocol.h"
#include "GlobalParameters.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Node.h"
#include "Sink.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"

extern bool DO_IHAR;
extern bool TEST_HOTSPOT_SIMILARITY;

extern int currentTime;
extern int startTimeForHotspotSelection;
extern double CO_HOTSPOT_HEAT_A1;
extern double CO_HOTSPOT_HEAT_A2;
extern double BETA;
extern int MAX_MEMORY_TIME;
extern int NUM_NODE;
extern double PROB_DATA_FORWARD;
extern int DATATIME;
extern int RUNTIME;

extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

extern string logInfo;
extern ofstream debugInfo;

class Epidemic :
	public CRoutingProtocol
{
public:
	Epidemic(void);
	~Epidemic(void);

	//���ض�ʱ���ϲ�������
	void GenerateData();
	//���ض�ʱ���Ϸ�������
	void SendData();
	//��ӡ�����Ϣ���ļ�
	void PrintInfo();

};

