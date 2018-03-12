#pragma once

#ifndef __SORT_HELPER_H__
#define __SORT_HELPER_H__

#include "Hotspot.h"
#include "Helper.h"
#include "Node.h"


//һЩԤ��������װ�ɵ���
class CSortHelper :
	virtual public CHelper
{
private:

	CSortHelper(){};
	~CSortHelper(){};


public:

	/***************************************** ���ڹ鲢����ĺ��� *****************************************/

	//template <class E>
	//static vector<E> merge(vector<E> left, vector<E> right, bool(*Comp)(E, E));
	//template <class E>
	//static vector<E> mergeSort(vector<E> v, bool(*Comp)(E, E));

	//CData��������ʱ������
	static vector<CData> merge(vector<CData> left, vector<CData> right, bool(*Comp)(CData, CData));
	static vector<CData> mergeSort(vector<CData> v, bool(*Comp)(CData, CData));

	//CPosition�ఴ��x��������
	static vector<CPosition *> merge(vector<CPosition *> left, vector<CPosition *> right);
	static vector<CPosition *> mergeSort(vector<CPosition *> v);

	//CNode�ఴ��x��������
	static vector<CNode *> merge(vector<CNode *> left, vector<CNode *> right);
	static vector<CNode *> mergeSort(vector<CNode *> v);
	//CNode�ఴ��ID����
	static vector<CNode *> merge(vector<CNode *> left, vector<CNode *> right, bool(*Comp)(CNode *, CNode *));
	static vector<CNode *> mergeSort(vector<CNode *> v, bool(*Comp)(CNode *, CNode *));

	//CHotspot�ఴ��x�������ratio����
	static vector<CHotspot *> merge(vector<CHotspot *> left, vector<CHotspot *> right, bool(*Comp)(CHotspot *, CHotspot *));
	static vector<CHotspot *> mergeSort(vector<CHotspot *> v, bool(*Comp)(CHotspot *, CHotspot *));
	////CHotspot�ྲ̬��������( endTime - 900, endTime )�ڼ��Ͷ�ݼ����Ľ�������
	//static vector<CHotspot> mergeByDeliveryCount(vector<CHotspot> left, vector<CHotspot> right, int endTime);
	//static vector<CHotspot> mergeSortByDeliveryCount(vector<CHotspot> v, int endTime);

	//������Ϊ��������mergeSort������Comparison����
	static bool ascendByLocationX(CHotspot *left, CHotspot *right);
	static bool ascendByRatio(CHotspot *left, CHotspot *right);
	//�����ȵ����һ���ڵ�Ͷ�ݼ�������
	static bool descendByCountDelivery(CHotspot *left, CHotspot *right){	return left->getCountDelivery() < right->getCountDelivery();	};
	static bool descendByInt(int left, int right){	return left > right;	};
	static bool ascendByInt(int left, int right){	return left < right;	};
	static bool ascendByTimeBirth(CData left, CData right){	return left < right; };
	static bool descendByTimeBirth(CData left, CData right){ return left > right; };
	static bool ascendByID(CNode *left, CNode *right){	return left->getID() < right->getID();	};

	template <class E>
	static vector<E> insertIntoSortedList(vector<E> dstList, E src, bool(*compPos)( E, E ), bool(*compNeg)( E, E ));

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
