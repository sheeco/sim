#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <afx.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>


//Simple Macro Def
#define PI 3.1415926535
#define ROUND(x) (x - floor(x) >= 0.5) ? (int)ceil(x) : (int)floor(x)
#define EQUAL(x, y) fabs( x - y ) < 0.000001
#define ZERO(x) fabs(x) < 0.000001
//#define AREA_SINGLE_HOTSPOT TRANS_RANGE * TRANS_RANGE * PI  ->  AreaCircle(CGeneralNode::TRANS_RANGE)

//Output & Debug
#define TAB '\t'
#define CR '\r'  //用于控制台输出时同行改写的转义字符
#define _PAUSE_ _ALERT_; system("pause")
#define _ALERT_ cout << '\a'
#define flash_cout cout << CR  //控制台输出位置回到行首，在动态显示的输出时使用 flash_cout 代替 cout 即可

/********************************** Namespace Lib ***********************************/

// TODO: 尝试使用用户namespace包裹当前所有用户类型定义

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


/********************************** Global Namespace ***********************************/

namespace global
{
	/******************************** Config Const ********************************/

	typedef enum _MAC_PROTOCOL { _smac, _hdc } _MAC_PROTOCOL;
	typedef enum _ROUTING_PROTOCOL { _har, _prophet, _epidemic } _ROUTING_PROTOCOL;
	typedef enum _HOTSPOT_SELECT { _original, _improved, _merge } _HOTSPOT_SELECT;


	/********************************* Global Var *********************************/

	// TODO: move these global config to a global CConfiguration parameter
	extern _MAC_PROTOCOL MAC_PROTOCOL;
	extern _ROUTING_PROTOCOL ROUTING_PROTOCOL;
	extern _HOTSPOT_SELECT HOTSPOT_SELECT;

	extern int DATATIME;
	extern int RUNTIME;
	extern string DATASET;
	extern int SLOT;  // ( s )
	extern int SLOT_MOBILITYMODEL;  //移动模型中的slot，由数据文件中得来（NCSU模型中为30）
	extern int SLOT_RECORD_INFO;  //记录数据投递率和数据投递时延的slot

	/********************************* Usage & Output ***********************************/

	extern string INFO_LOG;
	extern string INFO_HELP;

	extern string FILE_DEBUG;
	extern string INFO_DEBUG;

	extern string INFO_DELIVERY_RATIO;
	extern string INFO_DELAY;
	extern string INFO_ENERGY_CONSUMPTION;
	extern string INFO_ENCOUNTER;
	extern string INFO_SINK;
	extern string INFO_BUFFER;
	extern string INFO_BUFFER_STATISTICS;

	extern string INFO_HOTSPOT;
	extern string INFO_AT_HOTSPOT;
	extern string INFO_HOTSPOT_STATISTICS;
	extern string INFO_MERGE;
	extern string INFO_MERGE_DETAILS;
	extern string INFO_MA;
	extern string INFO_BUFFER_MA;
	extern string INFO_ED;


	/****************************** Global Func *******************************/

	inline void Exit(int code)
	{
		ofstream debug(FILE_DEBUG, ios::app);
		debug << INFO_LOG;	
		debug.close();
		exit(code);
	}

	// TODO: find all refs & replace with Bet()
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

	// prob between [0, 1]
	inline bool Bet(double prob)
	{
		return RandomFloat(0, 1) <= prob;
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

	//在min到max的范围内生成size个不重复的随机数
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

	//为浮点数保留n位小数，四舍五入
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

	//圆形区域面积
	inline double AreaCircle(double radius)
	{
		return radius * radius * PI;
	}

	//检查vector中是否已存在某个元素
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

	//释放指针vector
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

}

using namespace global;