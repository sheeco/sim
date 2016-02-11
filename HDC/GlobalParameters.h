#pragma once

#include <stdio.h>
#include <afx.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include <time.h>
#include <map>

using namespace std;

//#define NUM_NODE 29  //OLD: sensor��Ŀ�����Ѹ���NUM_NODE_INIT
#define SINK_ID 0  //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ
//#define SINK_X 0.0
//#define SINK_Y 0.0
//#define TRANS_RANGE 1000	//transmission range
#define SPEED_MANODE 30

//buffer
#define INFINITE_BUFFER false  //MA buffer�Ƿ������޴��
#define BUFFER_OVERFLOW_ALLOWED false  //MA buffer����ʱ�Ƿ������������
#define BUFFER_CAPACITY_NODE 100
#define BUFFER_CAPACITY_MA 100
#define BUFFER_CAPACITY_SINK 99999999999  //������

//time

#define SLOT 5	// ( s )
#define SLOT_MOBILITYMODEL 30  //�ƶ�ģ���е�slot���������ļ��е�����NCSUģ����Ϊ30��
#define SLOT_LOCATION_UPDATE 100	//������Ϣ�ռ���slot
#define SLOT_HOTSPOT_UPDATE 900	//�����ȵ�ͷ����slot
#define SLOT_DATA_GENERATE 60	//���ݲ���slot
#define SLOT_DATA_SEND 30	//���ݷ���slot
#define SLOT_RECORD_INFO 100  //��¼����Ͷ���ʺ�����Ͷ��ʱ�ӵ�slot
#define SLOT_CHANGE_NUM_NODE 5 * SLOT_HOTSPOT_UPDATE  //��̬�ڵ��������ʱ���ڵ���������仯������

//data
#define RATE_DATA_GENERATE 1.0 / 300.0  //( package / s )
#define SIZE_DATA 400  //( Byte )
#define SIZE_CONTROL 10
#define CONSUMPTION_DATA_SEND 0.008  //( mJ / Byte )
#define CONSUMPTION_DATA_RECIEVE 0.004
#define CONSUMPTION_LISTEN 13.5  // ( mJ / s )
#define CONSUMPTION_SLEEP 0.015


/********************************* IHAR **********************************/

//IHAR: Node Number Test
//#define TEST_DYNAMIC_NUM_NODE false
#define NUM_NODE_MIN 14
#define NUM_NODE_MAX 34
#define NUM_NODE_INIT 29

/********************************* merge-HAR *********************************/

#define TYPE_MERGE_HOTSPOT 1
#define TYPE_NEW_HOTSPOT 2
#define TYPE_OLD_HOTSPOT 3

#define ROUND(x) (x - floor(x) >= 0.5) ? (int)ceil(x) : (int)floor(x)
#define ZERO(x) fabs(x) < 0.000001
#define TAB '\t'
#define CR '\r'  //���ڿ���̨���ʱͬ�и�д��ת���ַ�
#define _PAUSE _ALERT; system("pause")
#define _ALERT cout << '\a'
#define flash_cout cout << CR  //����̨���λ�ûص����ף��ڶ�̬��ʾ�����ʱʹ�� flash_cout ���� cout ����
#define PI 3.1415926535
#define AREA_SINGLE_HOTSPOT TRANS_RANGE * TRANS_RANGE * PI

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
	return min + (double)rand() / RAND_MAX * (max - min);
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

//���vector���Ƿ��Ѵ���ĳ��Ԫ��
template <class E>
bool ifExists(vector<E> list, E elem)
{
	for(vector<E>::iterator i = list.begin(); i != list.end(); i++)
	{
		if(*i == elem)
			return true;
	}
	return false;
}

template <class E>
bool ifExists(vector<E> list, E elem, bool(*comp)(E, E))
{
	for(vector<E>::iterator i = list.begin(); i != list.end(); i++)
	{
		if(comp(*i, elem))
			return true;
	}
	return false;
}

template <class E>
void addToListUniquely(vector<E> &list, E n)
{
	//for(vector<int>::iterator i = list.begin(); i != list.end(); i++)
	//{
	//	if(*i == n)
	//		return ;
	//}
	if(! ifExists(list, n))
		list.push_back(n);
}

template <class E>
void addToListUniquely(vector<E> &des, vector<E> src)
{
	for(vector<E>::iterator i = src.begin(); i != src.end(); i++)
		addToListUniquely(des, *i);
}

template <class E>
void RemoveFromList(vector<E> &list, E elem)
{
	for(vector<E>::iterator i = list.begin(); i != list.end(); i++)
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
	for(vector<E>::iterator i = list.begin(); i != list.end(); i++)
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
	for(vector<E>::iterator i = src.begin(); i != src.end(); i++)
		RemoveFromList(des, *i);
}

template <class E>
void RemoveFromList(vector<E> &des, vector<E> src, bool(*comp)(E, E))
{
	for(vector<E>::iterator i = src.begin(); i != src.end(); i++)
		RemoveFromList(des, *i, comp);
}

//FIXME: ʹ�ö���mergesort�Ż����Ӷ�
template <class E>
static vector<E> getItemsByID(vector<E> list, vector<int> ids)
{
	vector<E> result;
	for(vector<int>::iterator id = ids.begin(); id != ids.end(); id++)
	{
		for(vector<E>::iterator item = list.begin(); item != list.end(); item++)
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

////OLD: ���¸�����������ת�Ƶ�CPreprocessor����
//vector<int> RandomIntList(int min, int max, int size)
//{
//	vector<int> result;
//	int tmp = -1;
//	if(size > (max - min))
//	{
//		vector<int> tmp_order;
//		for(int i = min; i < max; i++)
//		{
//			tmp_order.push_back(i);
//		}
//		while(! tmp_order.empty())
//		{
//			vector<int>::iterator it = tmp_order.begin();
//			int bet = RandomInt(0, tmp_order.size());
//			tmp = tmp_order.at(bet);
//			result.push_back(tmp);
//			tmp_order.erase(it + bet);
//		}
//		return result;
//	}
//
//	bool duplicate;
//	if(size == 1)
//	{
//		result.push_back(min);
//		return result;
//	}
//
//	for(int i = 0; i < size; i++)
//	{
//		do
//		{
//			duplicate = false;
//			tmp = RandomInt(min, max);
//			for(int j = 0; j < result.size(); j++)
//			{
//				if(result[j] == tmp)
//				{
//					duplicate = true;
//					break;
//				}
//			}
//		}while(duplicate);
//		if(tmp < 0)
//		{
//			cout << "Error @ CPreprocessorRandomIntList() : tmp < 0" << endl;
//			_PAUSE;
//		}
//		else
//			result.push_back(tmp);
//	}
//	return result;
//}
