#pragma once

#include "GlobalParameters.h"
#include "FileParser.h"
#include "Hotspot.h"
#include "Node.h"
#include "Sink.h"
#include "GreedySelection.h"
#include "PostSelector.h"
#include "NodeRepair.h"
#include "MacProtocol.h"

using namespace std;

extern bool DO_IHAR;
extern bool TEST_HOTSPOT_SIMILARITY;

extern int startTimeForHotspotSelection;
extern double CO_HOTSPOT_HEAT_A1;
extern double CO_HOTSPOT_HEAT_A2;
extern double BETA;
extern int MAX_MEMORY_TIME;
extern int NUM_NODE;
extern int DATATIME;
extern int RUNTIME;

extern double RATIO_MERGE_HOTSPOT;
extern double RATIO_NEW_HOTSPOT;
extern double RATIO_OLD_HOTSPOT;

//extern string logInfo;
//extern ofstream debugInfo;

class CHDC :
	public CMacProtocol
{
private:

	//���ڼ�������ȡ�����ȵ���������ʷƽ��ֵ
	static int HOTSPOT_COST_SUM;
	static int HOTSPOT_COST_COUNT;
	//���ڼ�������ѡȡ�����ȵ㼯���У�merge��old�ȵ�ı�������ʷƽ��ֵ
	static double MERGE_PERCENT_SUM;
	static int MERGE_PERCENT_COUNT;
	static double OLD_PERCENT_SUM;
	static int OLD_PERCENT_COUNT;
	//���ڼ����ȵ�ǰ�����ƶȵ���ʷƽ��ֵ
	static double SIMILARITY_RATIO_SUM;
	static int SIMILARITY_RATIO_COUNT;

	////��������node��λ�ã�������position��
	//void UpdateNodeLocations();
	//ִ���ȵ�ѡȡ
	static void HotspotSelection(int currentTime);
	//�Ƚϴ˴��ȵ�ѡȡ�Ľ������һ��ѡȡ���֮������ƶ�
	static void CompareWithOldHotspots(int currentTime);
	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);


public:

	CHDC(void);
	~CHDC(void);

	//�������Node�����λ���ȵ����򣬸���ռ�ձ�
	static void UpdateDutyCycleForNodes(int currentTime);

	static void Operate(int currentTime)
	{

		if( currentTime % SLOT_LOCATION_UPDATE == 0 )
			CPreprocessor::CollectNewPositions(currentTime);

		if( currentTime % SLOT_HOTSPOT_UPDATE == 0 )
		{
			cout  <<  endl <<"########  [ " << currentTime << " ]  HOTSPOT SELECTTION" << endl;
			HotspotSelection(currentTime);
		}

		if( currentTime >= startTimeForHotspotSelection && currentTime % SLOT_RECORD_INFO == 0 )
			PrintInfo(currentTime);
	}

};

