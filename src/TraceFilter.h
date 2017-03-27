#pragma once

#ifndef __TRACE_FILTER_H__
#define __TRACE_FILTER_H__

#include "Position.h"
#include "Algorithm.h"


class CTraceFilter :
	virtual public CAlgorithm
{
private:

	/*  直线的数据结构
		直线的形式是 a*x + b*y + c =0
		数据结构中存储 a, b, c
	*/
	typedef struct Line {
		double a;
		double b;
		double c;
	}Line;

	vector<CPosition *> inputs;
	vector<CPosition *> keyPoints;
	vector<CPosition *> noisePoints;

	//根据两点坐标得到过两点的直线
	static Line getLine(CPosition p1, CPosition p2);
	//计算两点间的距离
	//double countDistance(CPosition a, CPosition b);
	//计算点到直线的距离
	static double getDistance(Line line, CPosition p);
	//计算两点间平均速度
	static double getAverageVelocity(CPosition a, CPosition b);

	//判定条件一 d 与 r 的关系
	static bool hasNoPause(CPosition a, CPosition b);
	//判定条件二 h 与 w 的关系
	static bool withinRectangle(vector<CPosition *> flight, CPosition newPosition);
	//check if the new position is within this flight, based on the Rectangle Model
	static bool checkByRectangleModel(vector<CPosition *> flight, CPosition newPosition);

	//判定夹角是否在规定范围内
	static bool withinAngle(vector<CPosition *> flight, CPosition newPosition);
	//check if the new position is within this flight, based on the Angle Model
	static bool checkByAngleModel(vector<CPosition *> flight, CPosition newPosition);

	//check if the new position is within this flight, based on the Pause-based Model
	static bool checkByPauseModel(vector<CPosition *> flight, CPosition newPosition);

	//save a complete flight to keyPoints & noisePoints
	vector<CPosition*> saveFlight(vector<CPosition *> flight);

	//进行过滤，将得到的驻留点和途径点分别放入 keyPoints 和 noisePoints
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