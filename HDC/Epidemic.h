#pragma once

#include "GlobalParameters.h"
#include "RoutingProtocol.h"

extern int NUM_NODE;
extern double PROB_DATA_FORWARD;
extern int DATATIME;
extern int RUNTIME;
extern bool DO_HDC;

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

	static int MAX_QUEUE_SIZE;  //ͬ��洢�����������ڵ��data�������������������Ŀ���������Ƿ���Request֮ǰ��飿��Ĭ��ֵ����buffer����
	static int SPOKEN_MEMORY;  //�����ʱ���ڽ��������ݵĽڵ���ʱ���ٽ�������

	Epidemic(void)
	{

	}

	~Epidemic(void){};

	static bool Operate(int currentTime);

};

