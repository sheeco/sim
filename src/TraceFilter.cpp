#include "TraceFilter.h"
#include "SortHelper.h"

double CTraceFilter::R = 0.005;
double CTraceFilter::W = 0.5;


CTraceFilter::Line CTraceFilter::getLine(CPosition p1, CPosition p2)
{
	/*
	��������õ�һ�����������ֱ�ߣ����ظ�ֱ��
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
	����Ҫ���������һ ���ж�
	�����r���Լ�����ģ��ǵ��Լ��ģ�
	secondTerm�е�wҲͬ��Ҳ���Կ��Ǹ�Ϊȫ�ֱ���
	*/
	return CBasicEntity::getDistance(a, b) > R;
}

bool CTraceFilter::checkDistance(vector<CPosition *> positions, CPosition newPoint) 
{
	if( positions.size() < 2 )
		return true;

	/*
	�����µ�flight����β�����һ��ֱ�ߡ�
	a*x + b*y + c = 0
	ֱ�����ݽṹ ����� a, b, c
	*/
	Line lineFlight = getLine(*positions[0], newPoint);

	/*
	���м�㣨Ҳ����;���㣩ѭ��������㵽�ߵľ���
	�õ����ǵĺ��� w ���бȽ�
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
	//����ʱ������
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
