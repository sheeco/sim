#include "TraceFilter.h"
#include "SortHelper.h"

double CTraceFilter::R = 0.005;
double CTraceFilter::W = 0.5;


CTraceFilter::Line CTraceFilter::getLine(CPosition p1, CPosition p2)
{
	/*
	根据两点得到一条过这两点的直线，返回该直线
	*/
	Line line;
	if( p1.getX() == p2.getY() )
	{
		line.a = 1;
		line.b = 0;
		line.c = -p1.getX();
	}
	else if( p1.getY() == p2.getY() )
	{
		line.a = 0;
		line.b = 1;
		line.c = -p1.getY();
	}
	else
	{
		line.a = ( p2.getY() - p1.getY() ) / ( p2.getX() - p1.getX() );
		line.b = -1;
		line.c = p1.getY() - line.a * p1.getX();
	}
	return line;
}

double CTraceFilter::getDistance(Line line, CPosition p)
{
	return fabsl(( line.a * p.getX() + line.b * p.getY() + line.c ) / ( sqrtl(powl(line.a, 2) + powl(line.b, 2)) ));
}

bool CTraceFilter::checkPause(CPosition a, CPosition b) {
	/*
	对需要满足的条件一 的判定
	这里的r是自己定义的，记得自己改，
	secondTerm中的w也同样也可以考虑改为全局变量
	*/
	return CBasicEntity::getDistance(a, b) > R;
}

bool CTraceFilter::checkDistance(vector<CPosition *> positions, CPosition newPoint) 
{
	if( positions.size() < 2 )
		return true;

	/*
	根据新的flight的首尾计算出一条直线。
	a*x + b*y + c = 0
	直线数据结构 存的是 a, b, c
	*/
	Line lineFlight = getLine(*positions[0], newPoint);

	/*
	对中间点（也就是途经点）循环，计算点到线的距离
	得到他们的和与 w 进行比较
	*/

	vector<CPosition *>::iterator ipos = positions.begin() + 1;
	for( ; ipos != positions.end(); ++ipos )
	{
		if( getDistance(lineFlight, **ipos) > W )
			return false;
	}

	return true;
}

CTraceFilter::CTraceFilter(vector<CPosition*> positions)
{
	//按照时间排序
	this->positions = CSortHelper::mergeSort(positions, CSortHelper::ascendByTime);

	Filter();
}

vector<CPosition*> CTraceFilter::Filter()
{
	if( positions.size() <= 2 )
	{
		keyPoints = positions;
		return keyPoints;
	}

	vector<CPosition *> flight;
	//bool withinFlight = true;
	for( vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos )
	{
		if( flight.empty() )
		{
			flight.push_back(*ipos);
			continue;
		}

		if( checkPause(*flight[flight.size() - 1], **ipos)
		    && checkDistance(flight, **ipos) )
		{
			flight.push_back(*ipos);
		}
		else
		{

			CPosition * head = flight[0];
			keyPoints.push_back(head);

			CPosition * tail = flight[flight.size() - 1];
			CPosition * newHead = tail;

			if( flight.size() > 1 )
			{
				//save middle points as noise points
				for( int i = 1; i < flight.size() - 1; ++i )
				{
					noisePoints.push_back(flight[i]);
				}
				//newHead = tail;
				//keyPoints.push_back(newHead);

				flight.clear();
				flight.push_back(newHead);
			}
			else
			{
				flight.clear();
			}
			flight.push_back(*ipos);
		}
	}
	keyPoints.push_back(flight[0]);
	if( flight.size() > 1 )
		keyPoints.push_back(flight[flight.size() - 1]);
	for( int i = 1; i < flight.size() - 1; ++i )
	{
		noisePoints.push_back(flight[i]);
	}

	ASSERT( keyPoints.size() + noisePoints.size() == positions.size() );

	return keyPoints;
}
