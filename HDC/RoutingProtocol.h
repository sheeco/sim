/***********************************************************************************************************************************

�μ��� CRoutingProtocol �����̳��Զ����� CProtocol ������·��Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#include "protocol.h"

class CRoutingProtocol :
	public CProtocol
{
protected:

	//���޶���Χ�������ɾһ��������node����ɾ����������node��position��¼�����������ȵ�ѡȡ
	static void ChangeNodeNumber(int currentTime);
	//��������node�����ꡢռ�ձȺ͹���״̬
	static void UpdateNodeStatus(int currentTime);
	//����Node�����Լ���ge��������
	static void GenerateData(int currentTime);
	//��ӡ�����Ϣ���ļ�
	static void PrintInfo(int currentTime);

public:

	CRoutingProtocol(void){};
	~CRoutingProtocol(void){};
};

