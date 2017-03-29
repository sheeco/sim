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
		double a;
		double b;
		double c;
	}Line;

	vector<CPosition *> inputs;
	vector<CPosition *> keyPoints;
	vector<CPosition *> noisePoints;

	//������������õ��������ֱ��
	static Line getLine(CPosition p1, CPosition p2);
	//���������ľ���
	//double countDistance(CPosition a, CPosition b);
	//����㵽ֱ�ߵľ���
	static double getDistance(Line line, CPosition p);
	//���������ƽ���ٶ�
	static double getAverageVelocity(CPosition a, CPosition b);

	//�ж�����һ d �� r �Ĺ�ϵ
	static bool hasNoPause(CPosition a, CPosition b);
	//�ж������� h �� w �Ĺ�ϵ
	static bool withinRectangle(vector<CPosition *> flight, CPosition newPosition);
	//check if the new position is within this flight, based on the Rectangle Model
	static bool checkByRectangleModel(vector<CPosition *> flight, CPosition newPosition);

	//�ж��н��Ƿ��ڹ涨��Χ��
	static bool withinAngle(vector<CPosition *> flight, CPosition newPosition);
	//check if the new position is within this flight, based on the Angle Model
	static bool checkByAngleModel(vector<CPosition *> flight, CPosition newPosition);

	//check if the new position is within this flight, based on the Pause-based Model
	static bool checkByPauseModel(vector<CPosition *> flight, CPosition newPosition);

	//save a complete flight to keyPoints & noisePoints
	vector<CPosition*> saveFlight(vector<CPosition *> flight);

	//���й��ˣ����õ���פ�����;����ֱ���� keyPoints �� noisePoints
	vector<CPosition *> filter(vector<CPosition *> flight, bool (*check)( vector<CPosition *> , CPosition ));

public:

	//static double R;
	static double MIN_VELOCITY;
	static double MARGIN;
	static double THETA;
	
	CTraceFilter(vector<CPosition *> inputs);
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