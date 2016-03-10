#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <afx.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <tchar.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <map>

//TODO: move amap macro def into in-class static attr

//General Node
//int TRANS_RANGE = 100;  //transmission range  ->  CGeneralNode
//double PROB_DATA_FORWARD = 1.0;  ->  CGeneralNode

//Energy
//#define CONSUMPTION_BYTE_SEND 0.008  //( mJ / Byte )  ->  CGeneralNode
//#define CONSUMPTION_BYTE_RECIEVE 0.004  ->  CGeneralNode
//#define CONSUMPTION_LISTEN 13.5  // ( mJ / s )  ->  CGeneralNode
//#define CONSUMPTION_SLEEP 0.015  ->  CGeneralNode

//Sink
//#define SINK_ID 0  //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ  ->  CSink
//#define BUFFER_CAPACITY_SINK 99999999999  //������  ->  CSink
//double SINK_X = 0.0;  ->  CSink
//double SINK_Y = 0.0;  ->  CSink

//MA
//#define SPEED_MANODE 30  ->  CMANode
//#define BUFFER_CAPACITY_MA 100  ->  CMANode

/********************************* ��̬�ڵ�������� ***********************************/

//bool TEST_DYNAMIC_NUM_NODE = false;  ->  CRoutingProtocol

//int SLOT_CHANGE_NUM_NODE = 5 * CHotspot::SLOT_HOTSPOT_UPDATE;  //��̬�ڵ��������ʱ���ڵ���������仯������  ->  CRoutingProtocol
//IHAR: Node Number Test
//int NUM_NODE = NUM_NODE_INIT;  ->  dlt
//#define NUM_NODE_INIT 29  ->  CGeneralNode
//#define NUM_NODE_MIN 14  ->  CGeneralNode
//#define NUM_NODE_MAX 34  ->  CGeneralNode


/*********************************** HS *************************************/

//int startTimeForHotspotSelection = CHotspot::SLOT_HOTSPOT_UPDATE;  //no MA node at first  ->  CHotspot::TIME_HOSPOT_SELECT_START
//double ALPHA = 0.03;  //ratio for post selection  ->  CPostSelect
//double BETA = 0.0025;  //ratio for true hotspot  ->  HAR
//double GAMMA = 0.5;  //ratio for HotspotsAboveAverage  ->  dlt
//double CO_HOTSPOT_HEAT_A1 = 1;  ->  HAR
//double CO_HOTSPOT_HEAT_A2 = 30;  ->  HAR
//bool TEST_HOTSPOT_SIMILARITY = false;  ->  CRoutingProtocol


//TODO: add ifdef to these optional in-class mem attr

/********************************* IHAR **********************************/

//double LAMBDA = 0;  ->  HAR

//IHAR: Node Repair
//int MAX_MEMORY_TIME = 3600;  ->  HAR

/********************************* merge-HAR ***********************************/

//typedef enum TypeHotspot {_new, _old, _merge } TypeHotspot;  ->  CHotspot
//#define TYPE_MERGE_HOTSPOT 1
//#define TYPE_NEW_HOTSPOT 2
//#define TYPE_OLD_HOTSPOT 3

//�����������д���
//double RATIO_MERGE_HOTSPOT = 1.0;  ->  CHotspot
//double RATIO_NEW_HOTSPOT = 1.0;  ->  CHotspot
//double RATIO_OLD_HOTSPOT = 1.0;  ->  CHotspot

//int MIN_WAITING_TIME = 0;  //add minimum waiting time to each hotspot  ->  HAR
//bool TEST_BALANCED_RATIO = false;  ->  HAR

//double CO_POSITION_DECAY = 1.0;  ->  CPosition
//double MIN_POSITION_WEIGHT = 0;  ->  HAR

//bool TEST_LEARN = false;  ->  HAR


//Slot
//TODO: add def of namespace to wrap all defs
#define SLOT 5	// ( s )
//TODO: should be read from .trace file
#define SLOT_MOBILITYMODEL 30  //�ƶ�ģ���е�slot���������ļ��е�����NCSUģ����Ϊ30��
//#define SLOT_LOCATION_UPDATE 100	//������Ϣ�ռ���slot  ->  CHotspot
//#define SLOT_HOTSPOT_UPDATE 900	//�����ȵ�ͷ����slot  ->  CHotspot
//#define SLOT_DATA_SEND SLOT_MOBILITYMODEL	//���ݷ���slot  ->  CRoutingProtocol
#define SLOT_RECORD_INFO 100  //��¼����Ͷ���ʺ�����Ͷ��ʱ�ӵ�slot

//Simple Macro Def
#define PI 3.1415926535
#define ROUND(x) (x - floor(x) >= 0.5) ? (int)ceil(x) : (int)floor(x)
#define EQUAL(x, y) fabs( x - y ) < 0.000001
#define ZERO(x) fabs(x) < 0.000001
//#define AREA_SINGLE_HOTSPOT TRANS_RANGE * TRANS_RANGE * PI  ->  AreaCircle(CGeneralNode::TRANS_RANGE)

//Output & Debug
#define TAB '\t'
#define CR '\r'  //���ڿ���̨���ʱͬ�и�д��ת���ַ�
#define _PAUSE_ _ALERT_; system("pause")
#define _ALERT_ cout << '\a'
#define flash_cout cout << CR  //����̨���λ�ûص����ף��ڶ�̬��ʾ�����ʱʹ�� flash_cout ���� cout ����

/********************************** Namespace Lib ***********************************/

//TODO: ����ʹ���û�namespace������ǰ�����û����Ͷ���

using std::ios;
using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::string;
using std::vector;
using std::map;
using std::pair;
using std::iterator;
using std::exception;

/******************************** Config Const ********************************/

typedef enum _MAC_PROTOCOL { _smac, _hdc } _MAC_PROTOCOL;
typedef enum _ROUTING_PROTOCOL { _har, _prophet, _epidemic } _ROUTING_PROTOCOL;
typedef enum _HOTSPOT_SELECT { _original, _improved, _merge } _HOTSPOT_SELECT;


//TODO: wrap these funcs with a namespace

/****************************** Global�ĸ������� *******************************/

//Randomly product a float number between min and max
inline double RandomFloat(double min, double max)
{
	if(min == max)
		return min;
	if(min > max)
	{
		double temp = max;
		max = min;
		min = temp;
	}
	return min + double( rand() ) / RAND_MAX * (max - min);
}

//Randomly product a int number between min and max (cannot reach max)
inline int RandomInt(int min, int max)
{
	if (min == max)
		return min;
	if(min > max)
	{
		int temp = max;
		max = min;
		min = temp;
	}
	return min + rand() % (max - min);
}

//��min��max�ķ�Χ������size�����ظ��������
inline vector<int> RandomIntList(int min, int max, int size)
{
	vector<int> result;
	int temp = -1;
	if(size > (max - min))
	{
		vector<int> temp_order;
		for(int i = min; i < max; i++)
		{
			temp_order.push_back(i);
		}
		while(! temp_order.empty())
		{
			vector<int>::iterator it = temp_order.begin();
			int bet = RandomInt(0, temp_order.size());
			temp = temp_order.at(bet);
			result.push_back(temp);
			temp_order.erase(it + bet);
		}
		return result;
	}

	bool duplicate;
	if(size == 1)
	{
		result.push_back(min);
		return result;
	}

	for(int i = 0; i < size; i++)
	{
		do
		{
			duplicate = false;
			temp = RandomInt(min, max);
			for(int j = 0; j < result.size(); j++)
			{
				if(result[j] == temp)
				{
					duplicate = true;
					break;
				}
			}
		}while(duplicate);
		if(temp < 0)
		{
			cout << endl << "Error @ RandomIntList() : temp < 0" << endl;
			_PAUSE_;
		}
		else
			result.push_back(temp);
	}
	return result;
}

//Ϊ����������nλС������������
inline double NDigitFloat(double original, int n)
{
	if( ZERO(original) )
		return 0;
	if( n < 0 )
		return original;
	if( n == 0 )
		return ROUND(original);

	original *= pow(10, n);
	original = ROUND( original );
	original /= pow(10, n);
	return original;
}

inline double AreaCircle(double radius)
{
	return radius * radius * PI;
}

//���vector���Ƿ��Ѵ���ĳ��Ԫ��
template <class E>
bool IfExists(vector<E> list, E elem)
{
	for(typename vector<E>::iterator i = list.begin(); i != list.end(); ++i)
	{
		if(*i == elem)
			return true;
	}
	return false;
}

template <class E>
bool IfExists(vector<E> list, E elem, bool(*comp)(E, E))
{
	for(typename vector<E>::iterator i = list.begin(); i != list.end(); ++i)
	{
		if(comp(*i, elem))
			return true;
	}
	return false;
}

template <class E>
void AddToListUniquely(vector<E> &list, E n)
{
	//for(vector<int>::iterator i = list.begin(); i != list.end(); i++)
	//{
	//	if(*i == n)
	//		return ;
	//}
	if( ! IfExists(list, n))
		list.push_back(n);
}

template <class E>
void AddToListUniquely(vector<E> &des, vector<E> src)
{
	for(typename vector<E>::iterator i = src.begin(); i != src.end(); ++i)
		AddToListUniquely(des, *i);
}

template <class E>
void RemoveFromList(vector<E> &list, E elem)
{
	for(typename vector<E>::iterator i = list.begin(); i != list.end(); ++i)
	{
		if(*i == elem)
		{
			list.erase(i);
			return ;
		}
	}
}

template <class E>
void RemoveFromList(vector<E> &list, E elem, bool(*comp)(E, E))
{
	for(typename vector<E>::iterator i = list.begin(); i != list.end(); ++i)
	{
		if(comp(*i, elem))
		{
			list.erase(i);
			return ;
		}
	}
}

template <class E>
void RemoveFromList(vector<E> &des, vector<E> src)
{
	for(typename vector<E>::iterator i = src.begin(); i != src.end(); ++i)
		RemoveFromList(des, *i);
}

template <class E>
void RemoveFromList(vector<E> &des, vector<E> src, bool(*comp)(E, E))
{
	for(typename vector<E>::iterator i = src.begin(); i != src.end(); ++i)
		RemoveFromList(des, *i, comp);
}

//FIXME: ʹ�ö���mergesort�Ż����Ӷ�
template <class E>
vector<E> GetItemsByID(vector<E> list, vector<int> ids)
{
	vector<E> result;
	for(vector<int>::iterator id = ids.begin(); id != ids.end(); ++id)
	{
		for(typename vector<E>::iterator item = list.begin(); item != list.end(); ++item)
		{
			if( *item == *id )
			{
				result.push_back(*item);
				break;
			}
		}
	}
	return result;
}

//�ͷ�ָ��vector
template <class E>
void FreePointerVector(vector<E *> &v)
{
	vector<E *>::iterator it;
	for(it = v.begin(); it != v.end(); ++it)
	{
		if(*it != nullptr)
			delete *it;
	}
	v.clear();
}
