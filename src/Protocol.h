/***********************************************************************************************************************************

���� CProtocol �� ���̳��� CProcess > CAlgorithm ����CAlgorithm���ƣ���ר��������Э���㷨������Э���㷨��ʵ���࣬��Ӧ�ü̳��������

***********************************************************************************************************************************/

#pragma once

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "Algorithm.h"


class CProtocol : 
	virtual public CAlgorithm
{
public:

	CProtocol(){};
	virtual ~CProtocol() = 0
	{};

};

#endif // __PROTOCOL_H__
