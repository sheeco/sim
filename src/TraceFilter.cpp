#include "TraceFilter.h"
#include "SortHelper.h"

//double CTraceFilter::R = 0.005;
double CTraceFilter::MIN_VELOCITY = 1.0;
double CTraceFilter::MARGIN = 5;
double CTraceFilter::THETA = 30;


CTraceFilter::Line CTraceFilter::getLine(CPosition pLeft, CPosition pRight)
{
	/*
	根据两点得到一条过这两点的直线，返回该直线
	*/
	Line line;
	if( pLeft.getX() == pRight.getY() )
	{
		line.a = 1;
		line.b = 0;
		line.c = -pLeft.getX();
	}
	else if( pLeft.getY() == pRight.getY() )
	{
		line.a = 0;
		line.b = 1;
		line.c = -pLeft.getY();
	}
	else
	{
		line.a = ( pRight.getY() - pLeft.getY() ) / ( pRight.getX() - pLeft.getX() );
		line.b = -1;
		line.c = pLeft.getY() - line.a * pLeft.getX();
	}
	return line;
}

double CTraceFilter::getDistance(Line line, CPosition p)
{
	return fabs(( line.a * p.getX() + line.b * p.getY() + line.c ) / ( sqrt(pow(line.a, 2) + pow(line.b, 2)) ));
}

double CTraceFilter::getAverageVelocity(CPosition pLeft, CPosition pRight)
{
	return CBasicEntity::getDistance(pLeft, pRight) / fabs((double)(pRight.getTime() - pLeft.getTime()));
}

bool CTraceFilter::hasNoPause(CPosition pLeft, CPosition pRight) 
{
	return getAverageVelocity(pLeft, pRight) >= MIN_VELOCITY;
}

bool CTraceFilter::withinRectangle(vector<CPosition *> flight, CPosition newPoint) 
{
	if( flight.size() < 2 )
		return true;

	Line lineFlight = getLine(*flight[0], newPoint);

	/*
	对中间点（也就是途经点）循环，计算点到线的距离
	得到他们的和与 w 进行比较
	*/

	vector<CPosition *>::iterator ipos = flight.begin() + 1;
	for( ; ipos != flight.end(); ++ipos )
	{
		if( getDistance(lineFlight, **ipos) > MARGIN )
			return false;
	}

	return true;
}

bool CTraceFilter::checkByRectangleModel(vector<CPosition*> flight, CPosition newPosition)
{
	return hasNoPause(**flight.rbegin(), newPosition)
		&& withinRectangle(flight, newPosition);
}

bool CTraceFilter::withinAngle(vector<CPosition*> flight, CPosition newPosition)
{
	if( flight.size() < 2 )
		return true;

	//TODO:
	return false;
}

bool CTraceFilter::checkByAngleModel(vector<CPosition*> flight, CPosition newPosition)
{
	return hasNoPause(**flight.rbegin(), newPosition)
		&& withinAngle(flight, newPosition);
}

bool CTraceFilter::checkByPauseModel(vector<CPosition*> flight, CPosition newPosition)
{
	return hasNoPause(**flight.rbegin(), newPosition);
}

vector<CPosition*> CTraceFilter::saveFlight(vector<CPosition*> flight)
{
	if( flight.empty() )
		return flight;

	CPosition * head = flight[0];
	CPosition * tail = nullptr;
	if( flight.size() > 1 )
		tail = *flight.rbegin();

	// save the head as key point
	// unless it's already saved
	if( keyPoints.empty()
	   || *keyPoints.rbegin() != head )
		keyPoints.push_back(head);

	// save middle points as noise points
	if( flight.size() > 2 )
	{
		for( int i = 1; i < flight.size() - 1; ++i )
		{
			noisePoints.push_back(flight[i]);
		}
	}

	flight.clear();

	// head of new flight = old tail
	if( tail != nullptr )
	{
		keyPoints.push_back(tail);
		flight.push_back(tail);
	}

	// else if this is a 1-pos flight
	// head of new flight is unknown
	// just return empty flight

	return flight;
}

CTraceFilter::CTraceFilter(vector<CPosition*> positions)
{
	//按照时间排序
	this->inputs = CSortHelper::mergeSort(positions, CSortHelper::ascendByTime);

	vector<CPosition *> flights;
	
	//filter by rectangle model 
	positions = filter(positions, checkByRectangleModel);

	//filter again by angle model
	positions = keyPoints;
	keyPoints.clear();
	positions = filter(positions, checkByAngleModel);

	//filter again by pause-based model
	positions = keyPoints;
	keyPoints.clear();
	positions = filter(positions, checkByPauseModel);

	ASSERT(keyPoints.size() + noisePoints.size() == inputs.size());
}

vector<CPosition*> CTraceFilter::filter(vector<CPosition *> positions, bool(*check)( vector<CPosition *>, CPosition ))
{
	if( positions.size() <= 2 )
	{
		keyPoints = positions;
		return keyPoints;
	}

	vector<CPosition *> current_flight;
	for( vector<CPosition *>::iterator ipos = positions.begin(); ipos != positions.end(); ++ipos )
	{
		if( current_flight.empty() )
		{
			current_flight.push_back(*ipos);
			continue;
		}

		if( check(current_flight, **ipos) )
		{
			current_flight.push_back(*ipos);
		}
		else
		{
			current_flight = saveFlight(current_flight);
		}
	}
	if( ! current_flight.empty() )
		saveFlight(current_flight);

	return keyPoints;
}
