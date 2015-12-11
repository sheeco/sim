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
#include "HDC.h"

extern int NUM_NODE;
extern double PROB_DATA_FORWARD;
extern int DATATIME;
extern int RUNTIME;

extern string logInfo;
extern ofstream debugInfo;

class Epidemic :
	public CRoutingProtocol
{
private:

	//��������node�����ꡢռ�ձȺ͹���״̬
	static void UpdateNodeStatus(int currentTime);
	//���ض�ʱ���ϲ�������
	static void GenerateData(int currentTime);
	//���ض�ʱ���Ϸ�������
	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
	static void SendData(int currentTime);
	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);


public:
	Epidemic(void)
	{

	}

	~Epidemic(void){};

	static void Operate(int currentTime)
	{
		if( currentTime % SLOT_MOBILITYMODEL == 0 )
			UpdateNodeStatus(currentTime);
		
		if( currentTime % SLOT_DATA_GENERATE == 0 )
			GenerateData(currentTime);

		if( currentTime % SLOT_DATA_SEND == 0 )
			SendData(currentTime);

		if( currentTime % SLOT_RECORD_INFO )
			PrintInfo(currentTime);
	}

};

