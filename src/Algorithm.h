/***********************************************************************************************************************************

�μ��� CAlgorithm �� ���̳��Զ����� CProcess �����о����㷨ʵ�ֵĵ������࣬Ӧ�ü̳�������ࣻ��������·�ɻ�MAC��Э�飬��Ӧ�ü̳���CRoutingProtocol��CMacProtocol��

***********************************************************************************************************************************/

#pragma once

#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include "Process.h"


class CAlgorithm : 
	virtual public CProcess
{
public:

	CAlgorithm(){};
	virtual ~CAlgorithm() = 0
	{};

};

#endif // __ALGORITHM_H__
