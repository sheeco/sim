#pragma once

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define _WIN32_WINNT_WIN10 0x0A00
#define _WIN32_WINNT _WIN32_WINNT_WIN10  //Compiled under Win 10


/******************************** Common Header Lib **********************************/

#include <afx.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <iostream>
using std::ios;
using std::cin;
using std::cout;
using std::endl;
using std::exception;


#include <string>
using std::string;
#include <vector>
using std::vector;
using std::iterator;
#include <map>
using std::map;
using std::multimap;
using std::pair;
#include <sstream>
using std::stringstream;
#include <fstream>
using std::fstream;
using std::ofstream;
using std::ifstream;

#include <cmath>
#include <iomanip>
#include <direct.h>  //_mkdir()


/******************************* Common Namespace Lib ********************************/

using std::setw;
using std::setfill;


/*********************************** Simple Macro ***********************************/

#define PI 3.1415926535
#define ROUND(x) (x - floor(x) >= 0.5) ? (int)ceil(x) : (int)floor(x)
#define EQUAL(x, y) fabs( x - y ) < 0.000001
#define ZERO(x) fabs(x) < 0.000001
#define STRING(x) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str()
#define _STRING(x) #x

#define INVALID -1
#define INFINITE_INT 0xfffffff

/********************************** Output & Debug **********************************/

#define TAB "\t"
#define CR "\r"  //用于控制台输出时同行改写的转义字符
#define LF "\n"
#define ALERT "\a"
#define _PAUSE_ system("pause")
#define CURRENT_LOCATION string(__config.log.FILE__) + string(", ") + string(__FUNCTION__) + string(", ") + STRING(__LINE__)

//#define __DEBUG__
//#ifdef __DEBUG__
//#define _DEBUG_PRINT_ (str) cout << "$ DEBUG : " << str << endl;
//#endif


/******************************** User Defined ERRNO ********************************/

#define ESKIP -2  //Skip Without Execution
#define EFINISH 0  //Finish Execution Successfully
#define EERROR 1  //Unspecified Error
#define EDEBUG 2  //Debug Error
#define EFILE 3  //External File Error
#define EPARSE 4  //Wrong Format For Parse
#define EARG 5  //Wrong Argument
#define EMEMORY 6  //Wrong Memory Access


/********************************** Optional Macro **********************************/

//#define USE_PRED_TOLERANCE


/********************************** Global Namespace ***********************************/

namespace global
{
	typedef enum EnumKnownType
	{
		bool_type,
		int_type,
		double_type,
		string_type
	} EnumKnownType;
	typedef struct EnumType
	{
		EnumKnownType which;
		EnumType()
		{
		}
		EnumType(EnumKnownType type) : which(type)
		{
		}

		EnumType(const type_info& id)
		{
			if( id == typeid( bool ) )
			{
				which = bool_type;
			}
			else if( id == typeid( int ) )
			{
				which = int_type;
			}
			else if( id == typeid( double ) )
			{
				which = double_type;
			}
			else if( id == typeid( string ) )
			{
				which = string_type;
			}
			else
			{
				throw string("EnumKnownType::EnumKnownType(): Unknown type.");

			}
		}

		string name()
		{
			switch( which )
			{
				case bool_type:
					return "bool";
				case int_type:
					return "int";
				case double_type:
					return "double";
				case string_type:
					return "string";
				default:
					throw string("EnumKnownType::name(): Unknown type.");
			}
		}
	} EnumType;

	/****************************** Global Func *******************************/

	void Exit(int code);

	void Exit(int code, string error);

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
		if( prob == 1.0 )
			return true;
		return RandomFloat(0, 1) <= prob;
	}

	//Randomly product a int number between [min, max]
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
		max++;
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
			for(int i = min; i < max; ++i)
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

		for(int i = 0; i < size; ++i)
		{
			do
			{
				duplicate = false;
				temp = RandomInt(min, max);
				for(int j = 0; j < result.size(); ++j)
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
				throw string("RandomIntList() : temp < 0");
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

	inline string NDigitString(string input, int n, char ch)
	{
		stringstream ss;
		ss << setw(n) << setfill(ch) << input;
		return ss.str();
	}
	inline string NDigitString(string input, int n)
	{
		return NDigitString(input, n, ' ');
	}
	inline string NDigitString(int input, int n)
	{
		return NDigitString(STRING(input), n);
	}
	inline string NDigitString(double input, int n)
	{
		return NDigitString(STRING(input), n);
	}
	inline string NDigitIntString(int input, int n)
	{
		return NDigitString(STRING(input), n, '0');
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
		for(vector<E>::iterator i = list.begin(); i != list.end(); ++i)
		{
			if(*i == elem)
				return true;
		}
		return false;
	}

	template <class E>
	bool IfExists(vector<E> list, E elem, bool(*comp)(E, E))
	{
		for(vector<E>::iterator i = list.begin(); i != list.end(); ++i)
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
		for(vector<E>::iterator i = src.begin(); i != src.end(); ++i)
			AddToListUniquely(des, *i);
	}

	template <class E>
	void RemoveFromList(vector<E> &list, E elem)
	{
		for(vector<E>::iterator i = list.begin(); i != list.end(); ++i)
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
		for(vector<E>::iterator i = list.begin(); i != list.end(); ++i)
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
		for(vector<E>::iterator i = src.begin(); i != src.end(); ++i)
			RemoveFromList(des, *i);
	}

	template <class E>
	void RemoveFromList(vector<E> &des, vector<E> src, bool(*comp)(E, E))
	{
		for(vector<E>::iterator i = src.begin(); i != src.end(); ++i)
			RemoveFromList(des, *i, comp);
	}

//	template <class E>
//	vector<E> GetItemsByID(vector<E> list, vector<int> ids)  -> CBasicEntity
//	{
//		vector<E> result;
//		for(vector<int>::iterator id = ids.begin(); id != ids.end(); ++id)
//		{
////			for(vector<E>::iterator item = list.begin(); item != list.end(); ++item)
//			for(auto item = list.begin(); item != list.end(); ++item)
//			{
//				if( *item == *id )
//				{
//					result.push_back(*item);
//					break;
//				}
//			}
//		}
//		return result;
//	}

	template <class E>
	void FreePointer(E * &p)
	{
		if( p != nullptr )
			delete p;
		p = nullptr;
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

	template <class E>
	E getObjectForPointer(E* ptr)
	{
		return *ptr;
	}

	template <class E, class RTN>
	vector<RTN> Iterate(vector<E> list, RTN(* operation)(E))
	{
		vector<RTN> rtn;
		for(vector<E>::iterator item = list.begin(); item != list.end(); ++item)
			rtn.push_back( operation(*item) );
		return rtn;
	}

	template <class E>
	double Accumulate(vector<E> list, double(* value)(E))
	{
		double rtn = 0;
		for(vector<E>::iterator item = list.begin(); item != list.end(); ++item)
			rtn += value(*item);
		return rtn;
	}

}

using namespace global;


#endif // __GLOBAL_H__