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


//Sink
#define SINK_ID 0  //0Ϊsink�ڵ�Ԥ�����������ڵ�ID��1��ʼ
#define BUFFER_CAPACITY_SINK 99999999999  //������

//MA
#define SPEED_MANODE 30
#define BUFFER_CAPACITY_MA 100

//Slot
#define SLOT 5	// ( s )
#define SLOT_MOBILITYMODEL 30  //�ƶ�ģ���е�slot���������ļ��е�����NCSUģ����Ϊ30��
#define SLOT_LOCATION_UPDATE 100	//������Ϣ�ռ���slot
#define SLOT_HOTSPOT_UPDATE 900	//�����ȵ�ͷ����slot
#define SLOT_DATA_SEND 30	//���ݷ���slot
#define SLOT_RECORD_INFO 100  //��¼����Ͷ���ʺ�����Ͷ��ʱ�ӵ�slot

//Energy
#define CONSUMPTION_BYTE_SEND 0.008  //( mJ / Byte )
#define CONSUMPTION_BYTE_RECIEVE 0.004
#define CONSUMPTION_LISTEN 13.5  // ( mJ / s )
#define CONSUMPTION_SLEEP 0.015


/********************************* IHAR **********************************/

//IHAR: Node Number Test
#define NUM_NODE_MIN 14
#define NUM_NODE_MAX 34
#define NUM_NODE_INIT 29

/********************************* merge-HAR *********************************/

#define TYPE_MERGE_HOTSPOT 1
#define TYPE_NEW_HOTSPOT 2
#define TYPE_OLD_HOTSPOT 3

#define ROUND(x) (x - floor(x) >= 0.5) ? (int)ceil(x) : (int)floor(x)
#define EQUAL(x, y) fabs( x - y ) < 0.000001
#define ZERO(x) fabs(x) < 0.000001
#define TAB '\t'
#define CR '\r'  //���ڿ���̨���ʱͬ�и�д��ת���ַ�
#define _PAUSE _ALERT; system("pause")
#define _ALERT cout << '\a'
#define flash_cout cout << CR  //����̨���λ�ûص����ף��ڶ�̬��ʾ�����ʱʹ�� flash_cout ���� cout ����
#define PI 3.1415926535
#define AREA_SINGLE_HOTSPOT TRANS_RANGE * TRANS_RANGE * PI

/********************************** TypeDef ***********************************/

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

typedef char Mode;

class SEND
{
public:
	static const char COPY = 'C';  //��������ʱ������������
	static const char DUMP = 'D';  //��������ʱ��ɾ��������
};

class RECEIVE
{
public:
	static const char LOOSE = 'L';  //MA buffer����ʱ�������������������
	static const char SELFISH = 'F';  //MA buffer����ʱ�����ٴ������ڵ��������
};

class QUEUE
{
public:
	static const char FIFO = 'H';  //�ɷ����������ʱ�����ȴ�ͷ������
	static const char LIFO = 'S';  //�ɷ����������ʱ�����ȴ�β������
};

typedef enum _MacProtocol { _smac, _hdc } _MacProtocol;
typedef enum _RoutingProtocol { _har, _prophet, _epidemic } _RoutingProtocol;
typedef enum _HotspotSelect { _original, _improved, _merge } _HotspotSelect;



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
	int tmp = -1;
	if(size > (max - min))
	{
		vector<int> tmp_order;
		for(int i = min; i < max; i++)
		{
			tmp_order.push_back(i);
		}
		while(! tmp_order.empty())
		{
			vector<int>::iterator it = tmp_order.begin();
			int bet = RandomInt(0, tmp_order.size());
			tmp = tmp_order.at(bet);
			result.push_back(tmp);
			tmp_order.erase(it + bet);
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
			tmp = RandomInt(min, max);
			for(int j = 0; j < result.size(); j++)
			{
				if(result[j] == tmp)
				{
					duplicate = true;
					break;
				}
			}
		}while(duplicate);
		if(tmp < 0)
		{
			cout << endl << "Error @ RandomIntList() : tmp < 0" << endl;
			_PAUSE;
		}
		else
			result.push_back(tmp);
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
