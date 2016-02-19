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

using namespace std;


//Sink
#define SINK_ID 0  //0为sink节点预留，传感器节点ID从1开始
#define BUFFER_CAPACITY_SINK 99999999999  //无限制

//Node
//#define NUM_NODE 29  //OLD: sensor数目，现已改用NUM_NODE_INIT

//MA
#define SPEED_MANODE 30
#define BUFFER_CAPACITY_MA 100

//Slot
#define SLOT 5	// ( s )
#define SLOT_MOBILITYMODEL 30  //移动模型中的slot，由数据文件中得来（NCSU模型中为30）
#define SLOT_LOCATION_UPDATE 100	//地理信息收集的slot
#define SLOT_HOTSPOT_UPDATE 900	//更新热点和分类的slot
#define SLOT_DATA_SEND 30	//数据发送slot
#define SLOT_RECORD_INFO 100  //记录数据投递率和数据投递时延的slot
#define SLOT_CHANGE_NUM_NODE 5 * 900  //动态节点个数测试时，节点个数发生变化的周期

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
#define CR '\r'  //用于控制台输出时同行改写的转义字符
#define _PAUSE _ALERT; system("pause")
#define _ALERT cout << '\a'
#define flash_cout cout << CR  //控制台输出位置回到行首，在动态显示的输出时使用 flash_cout 代替 cout 即可
#define PI 3.1415926535
#define AREA_SINGLE_HOTSPOT TRANS_RANGE * TRANS_RANGE * PI


/******************************** Mode Const ********************************/

typedef char Mode;

class SEND
{
public:
	static const char COPY = 'C';  //发送数据时，保留自身副本
	static const char DUMP = 'D';  //发送数据时，删除自身副本
	static const char FIFO = 'H';  //可发送配额有限时，优先从头部发送
	static const char LIFO = 'S';  //可发送配额有限时，优先从尾部发送
};

class BUFFER
{
public:
	static const char LOOSE = 'L';  //MA buffer已满时，仍允许继续接收数据
	static const char SELFISH = 'F';  //MA buffer已满时，不再从其他节点接收数据
};

typedef enum _MacProtocol { _smac, _hdc } _MacProtocol;
typedef enum _RoutingProtocol { _har, _prophet, _epidemic } _RoutingProtocol;
typedef enum _HotspotSelect { _original, _improved, _merge } _HotspotSelect;



/****************************** Global的辅助函数 *******************************/
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

//检查vector中是否已存在某个元素
template <class E>
bool ifExists(vector<E> list, E elem)
{
	for(typename vector<E>::iterator i = list.begin(); i != list.end(); ++i)
	{
		if(*i == elem)
			return true;
	}
	return false;
}

template <class E>
bool ifExists(vector<E> list, E elem, bool(*comp)(E, E))
{
	for(typename vector<E>::iterator i = list.begin(); i != list.end(); ++i)
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
	if( ! ifExists(list, n))
		list.push_back(n);
}

template <class E>
void addToListUniquely(vector<E> &des, vector<E> src)
{
	for(typename vector<E>::iterator i = src.begin(); i != src.end(); ++i)
		addToListUniquely(des, *i);
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

//FIXME: 使用二重mergesort优化复杂度
template <class E>
static vector<E> getItemsByID(vector<E> list, vector<int> ids)
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
