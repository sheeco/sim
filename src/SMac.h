#pragma once

#include "MacProtocol.h"


class CSMac :
	public CMacProtocol
{
public:

	CSMac(void){};
	~CSMac(void){};

	static bool Operate(int currentTime);

//	//��ӡ�����Ϣ���ļ�
//	static void PrintInfo(int currentTime);
//	static void PrintFinal(int currentTime);

};

