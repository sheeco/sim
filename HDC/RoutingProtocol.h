/***********************************************************************************************************************************

�μ��� CRoutingProtocol �����̳��Զ����� CProtocol ������·��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#include "protocol.h"

using namespace std;

class CRoutingProtocol :
	public CProtocol
{
protected:
	
	static void GenerateData(int currentTime);


public:

	CRoutingProtocol(void){};
	~CRoutingProtocol(void){};
};

