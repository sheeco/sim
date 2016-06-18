#pragma once

#ifndef __SORT_HELPER_H__
#define __SORT_HELPER_H__

#include "Hotspot.h"
#include "Helper.h"
#include "Node.h"


//一些预处理函数包装成的类
class CSortHelper :
	virtual public CHelper
{
private:

	CSortHelper(){};
	~CSortHelper(){};


public:

	/***************************************** 用于归并排序的函数 *****************************************/

	//template <class E>
	//static vector<E> merge(vector<E> left, vector<E> right, bool(*Comp)(E, E));
	//template <class E>
	//static vector<E> mergeSort(vector<E> v, bool(*Comp)(E, E));

	//CData按照生成时间排序
	static vector<CData> merge(vector<CData> left, vector<CData> right, bool(*Comp)(CData, CData));
	static vector<CData> mergeSort(vector<CData> v, bool(*Comp)(CData, CData));

	//CPosition类按照x坐标排序
	static vector<CPosition *> merge(vector<CPosition *> left, vector<CPosition *> right);
	static vector<CPosition *> mergeSort(vector<CPosition *> v);

	//CNode类按照x坐标排序
	static vector<CNode *> merge(vector<CNode *> left, vector<CNode *> right);
	static vector<CNode *> mergeSort(vector<CNode *> v);
	//CNode类按照ID排序
	static vector<CNode *> merge(vector<CNode *> left, vector<CNode *> right, bool(*Comp)(CNode *, CNode *));
	static vector<CNode *> mergeSort(vector<CNode *> v, bool(*Comp)(CNode *, CNode *));

	//CHotspot类按照x坐标或者ratio排序
	static vector<CHotspot *> merge(vector<CHotspot *> left, vector<CHotspot *> right, bool(*Comp)(CHotspot *, CHotspot *));
	static vector<CHotspot *> mergeSort(vector<CHotspot *> v, bool(*Comp)(CHotspot *, CHotspot *));
	//CHotspot类静态拷贝按照( endTime - 900, endTime )期间的投递计数的降序排列
	static vector<CHotspot> mergeByDeliveryCount(vector<CHotspot> left, vector<CHotspot> right, int endTime);
	static vector<CHotspot> mergeSortByDeliveryCount(vector<CHotspot> v, int endTime);

	//用于作为参数传入mergeSort函数的Comparison函数
	static bool ascendByLocationX(CHotspot *left, CHotspot *right);
	static bool ascendByRatio(CHotspot *left, CHotspot *right);
	static bool descendByInt(int left, int right){	return left > right;	};
	static bool ascendByInt(int left, int right){	return left < right;	};
	static bool ascendByTimeBirth(CData left, CData right){	return left < right; };
	static bool descendByTimeBirth(CData left, CData right){ return left > right; };
	static bool ascendByID(CNode *left, CNode *right){	return left->getID() < right->getID();	};

	template <class E>
	static vector<E> insertIntoSortedList(vector<E> dstList, E src, bool(*compPos)( E, E ), bool(*compNeg)( E, E ));

	template <class E>
	static vector<E> insertIntoSortedList(vector<E> dstList, vector<E> srcList, bool(*compPos)( E, E ), bool(*compNeg)( E, E ));

};

#endif // __SORT_HELPER_H__

template<class E>
inline vector<E> CSortHelper::insertIntoSortedList(vector<E> dstList, E src, bool(*compPos)( E, E ), bool(*compNeg)( E, E ))
{
	vector<E> rtn;
	typename vector<E>::iterator idst = dstList.begin();
	for( ; idst != dstList.end(); ++idst )
	{
		if( !compPos(*idst, src) )
			break;
	}
	for( ; idst != dstList.end(); ++idst )
	{
		if( compNeg(*idst, src) )
			break;
		if( *idst == src )
		{
			idst = dstList.erase(idst);
			break;
		}
	}
	rtn = vector<E>(dstList.begin(), idst);
	rtn.push_back(src);
	rtn.insert(rtn.end(), idst, dstList.end());

	return rtn;
}

template<class E>
inline vector<E> CSortHelper::insertIntoSortedList(vector<E> dstList, vector<E> srcList, bool(*compPos)( E, E ), bool(*compNeg)( E, E ))
{
	for( typename vector<E>::iterator isrc = srcList.begin(); isrc != srcList.end(); ++isrc )
		dstList = insertIntoSortedList(dstList, *isrc, compPos, compNeg);
	return dstList;
}
