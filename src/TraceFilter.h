#pragma once

#ifndef __TRACE_FILTER_H__
#define __TRACE_FILTER_H__

#include "Position.h"
#include "Algorithm.h"


class CTraceFilter :
	virtual public CAlgorithm
{
private:

	/*  ֱ�ߵ����ݽṹ
		ֱ�ߵ���ʽ�� a*x + b*y + c =0
		���ݽṹ�д洢 a, b, c
	*/
	typedef struct Line {
		long double a;
		long double b;
		long double c;
	}Line;

	vector<CPosition *> positions;
	vector<CPosition *> keyPoints;
	vector<CPosition *> noisePoints;

	//������������õ��������ֱ��
	static Line getLine(CPosition p1, CPosition p2);
	//���������ľ���
	//double countDistance(CPosition a, CPosition b);
	//����㵽ֱ�ߵľ���
	static double getDistance(Line line, CPosition p);

	//�ж�����һ d �� r �Ĺ�ϵ
	static bool checkPause(CPosition a, CPosition b);
	//�ж������� h �� w �Ĺ�ϵ
	static bool checkDistance(vector<CPosition *> positions, CPosition newPosition);

	//���й��ˣ����õ���פ�����;����ֱ���� keyPoints �� noisePoints
	vector<CPosition *> Filter();

public:

	static double R;
	static double W;
	
	CTraceFilter(vector<CPosition *> positions);
	~CTraceFilter() {};

	inline vector<CPosition *> getKeyPoints() const
	{
		return keyPoints;
	}

	inline vector<CPosition *> getNoisePoints() const
	{
		return noisePoints;
	}

};

#endif // !__TRACE_FILTER_H__