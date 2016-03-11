#pragma once

#include "GlobalParameters.h"
#include "RoutingProtocol.h"


class CEpidemic :
	public CRoutingProtocol
{
private:

	//���ض�ʱ���Ϸ�������
	//ע�⣺�����ڵ���UpdateNodeStatus֮����ô˺���
	static void SendData(int currentTime);


public:

	static int MAX_QUEUE_SIZE;  //ͬ��洢�����������ڵ��data�������������������Ŀ���������Ƿ���Request֮ǰ��飿��Ĭ��ֵ����buffer����
	static int SPOKEN_MEMORY;  //�����ʱ���ڽ��������ݵĽڵ���ʱ���ٽ�������

	CEpidemic(){};

	~CEpidemic(){};

	static bool Operate(int currentTime);

};

