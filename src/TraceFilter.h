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
		long double a;
		long double b;
		long double c;
	}Line;

	vector<CPosition *> positions;
	vector<CPosition *> keyPoints;
	vector<CPosition *> noisePoints;

	//根据两点坐标得到过两点的直线
	static Line getLine(CPosition p1, CPosition p2);
	//计算两点间的距离
	//double countDistance(CPosition a, CPosition b);
	//计算点到直线的距离
	static double getDistance(Line line, CPosition p);

	//判定条件一 d 与 r 的关系
	static bool checkPause(CPosition a, CPosition b);
	//判定条件二 h 与 w 的关系
	static bool checkDistance(vector<CPosition *> positions, CPosition newPosition);

	//进行过滤，将得到的驻留点和途径点分别放入 keyPoints 和 noisePoints
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