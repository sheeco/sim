#include "SortHelper.h"


bool CSortHelper::ascendByLocationX(CHotspot *left, CHotspot *right)
{
	return (*left > *right);
}

bool CSortHelper::ascendByRatio(CHotspot *left, CHotspot *right)
{
	//将执行强制类型转换，只能传入CHotspot类
	//包括ratio的计算
	return ( static_cast<CHotspot *>(left)->calculateRatio() ) > ( static_cast<CHotspot *>(right)->calculateRatio() );
}

//template <class E>
//vector<E> CSortHelper::merge(vector<E> left, vector<E> right, bool(*Comp)(E, E))
//{
//	vector<E> result;
//	vector<E>::size_type li = 0;
//	vector<E>::size_type ri = 0;
//	while(li < left.size()
//		&& ri < right.size())
//	{
//		if( ! Comp(left[li], right[ri]) )
//			result.push_back(right[ri++]);
//		else
//			result.push_back(left[li++]);
//	}
//	while(li < left.size())
//		result.push_back(left[li++]);
//	while(ri < right.size())
//		result.push_back(right[ri++]);
//	return result;
//}

//template <class E>
//vector<E> CSortHelper::mergeSort(vector<E> v, bool(*Comp)(E, E))
//{
//	if(v.size() == 0)
//		return vector<E>();
//	if(v.size() == 1)
//		return vector<E>(1, v[0]);
//
//	vector<E>::iterator mid = v.begin() + v.size() / 2;
//	vector<E> left(v.begin(), mid);
//	vector<E> right(mid, v.end());
//	left = mergeSort(left, Comp);
//	right = mergeSort(right, Comp);
//
//	return merge(left, right, Comp);
//}

vector<CData> CSortHelper::merge(vector<CData> left, vector<CData> right, bool(*Comp)(CData, CData))
{
	vector<CData> result;
	vector<CData>::size_type li = 0;
	vector<CData>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if( ! Comp(left[li], right[ri]) )
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CData> CSortHelper::mergeSort(vector<CData> v, bool(*Comp)(CData, CData))
{
	if(v.size() == 0)
		return vector<CData>();
	if(v.size() == 1)
		return vector<CData>(1, v[0]);

	vector<CData>::iterator mid = v.begin() + v.size() / 2;
	vector<CData> left(v.begin(), mid);
	vector<CData> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

vector<CPosition *> CSortHelper::merge(vector<CPosition *> left, vector<CPosition *> right)
{
	vector<CPosition *> result;
	vector<CPosition *>::size_type li = 0;
	vector<CPosition *>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if(*left[li] > *right[ri])
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CPosition *> CSortHelper::mergeSort(vector<CPosition *> v)
{
	if(v.size() == 0)
		return vector<CPosition *>();
	if(v.size() == 1)
		return vector<CPosition *>(1, v[0]);

	vector<CPosition *>::iterator mid = v.begin() + v.size() / 2;
	vector<CPosition *> left(v.begin(), mid);
	vector<CPosition *> right(mid, v.end());
	left = mergeSort(left);
	right = mergeSort(right);

	return merge(left, right);
}

vector<CPosition*> CSortHelper::merge(vector<CPosition*> left, vector<CPosition*> right, bool(*Comp)( CBasicEntity &, CBasicEntity & ))
{
	vector<CPosition *> result;
	vector<CPosition *>::size_type li = 0;
	vector<CPosition *>::size_type ri = 0;
	while( li < left.size()
		  && ri < right.size() )
	{
		if( Comp(*left[li], *right[ri]) )
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while( li < left.size() )
		result.push_back(left[li++]);
	while( ri < right.size() )
		result.push_back(right[ri++]);
	return result;
}

vector<CPosition*> CSortHelper::mergeSort(vector<CPosition*> v, bool(*Comp)( CBasicEntity &, CBasicEntity & ))
{
	if( v.size() == 0 )
		return vector<CPosition *>();
	if( v.size() == 1 )
		return vector<CPosition *>(1, v[0]);

	vector<CPosition *>::iterator mid = v.begin() + v.size() / 2;
	vector<CPosition *> left(v.begin(), mid);
	vector<CPosition *> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

vector<CNode *> CSortHelper::merge(vector<CNode *> left, vector<CNode *> right)
{
	vector<CNode *> result;
	vector<CNode *>::size_type li = 0;
	vector<CNode *>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if(*left[li] > *right[ri])
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CNode *> CSortHelper::mergeSort(vector<CNode *> v)
{
	if(v.size() == 0)
		return vector<CNode *>();
	if(v.size() == 1)
		return vector<CNode *>(1, v[0]);

	vector<CNode *>::iterator mid = v.begin() + v.size() / 2;
	vector<CNode *> left(v.begin(), mid);
	vector<CNode *> right(mid, v.end());
	left = mergeSort(left);
	right = mergeSort(right);

	return merge(left, right);
}

vector<CNode *> CSortHelper::merge(vector<CNode *> left, vector<CNode *> right, bool(*Comp)(CNode *, CNode *))
{
	vector<CNode *> result;
	vector<CNode *>::size_type li = 0;
	vector<CNode *>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if( ! Comp(left[li], right[ri]) )
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CNode *> CSortHelper::mergeSort(vector<CNode *> v, bool(*Comp)(CNode *, CNode *))
{
	if(v.size() == 0)
		return vector<CNode *>();
	if(v.size() == 1)
		return vector<CNode *>(1, v[0]);

	vector<CNode *>::iterator mid = v.begin() + v.size() / 2;
	vector<CNode *> left(v.begin(), mid);
	vector<CNode *> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

vector<CHotspot *> CSortHelper::merge(vector<CHotspot *> left, vector<CHotspot *> right, bool(*Comp)(CHotspot *, CHotspot *))
{
	vector<CHotspot *> result;
	vector<CHotspot *>::size_type li = 0;
	vector<CHotspot *>::size_type ri = 0;
	while(li < left.size()
		&& ri < right.size())
	{
		if( Comp(left[li], right[ri]) )
			result.push_back(right[ri++]);
		else
			result.push_back(left[li++]);
	}
	while(li < left.size())
		result.push_back(left[li++]);
	while(ri < right.size())
		result.push_back(right[ri++]);
	return result;
}

vector<CHotspot *> CSortHelper::mergeSort(vector<CHotspot *> v, bool(*Comp)(CHotspot *, CHotspot *))
{
	if(v.size() == 0)
		return vector<CHotspot *>();
	if(v.size() == 1)
		return vector<CHotspot *>(1, v[0]);

	vector<CHotspot *>::iterator mid = v.begin() + v.size() / 2;
	vector<CHotspot *> left(v.begin(), mid);
	vector<CHotspot *> right(mid, v.end());
	left = mergeSort(left, Comp);
	right = mergeSort(right, Comp);

	return merge(left, right, Comp);
}

//vector<CHotspot> CSortHelper::mergeByDeliveryCount(vector<CHotspot> left, vector<CHotspot> right, int endTime)
//{
//	vector<CHotspot> result;
//	vector<CHotspot>::size_type li = 0;
//	vector<CHotspot>::size_type ri = 0;
//	while(li < left.size()
//		&& ri < right.size())
//	{
//		int lv = left[li].getCountDelivery(endTime);
//		int rv = right[ri].getCountDelivery(endTime);
//		if( lv < rv )
//			result.push_back(right[ri++]);
//		else
//			result.push_back(left[li++]);
//	}
//	while(li < left.size())
//		result.push_back(left[li++]);
//	while(ri < right.size())
//		result.push_back(right[ri++]);
//	return result;
//}
//
//vector<CHotspot> CSortHelper::mergeSortByDeliveryCount(vector<CHotspot> v, int endTime)
//{
//	if(v.size() == 0)
//		return vector<CHotspot>();
//	if(v.size() == 1)
//		return vector<CHotspot>(1, v[0]);
//
//	vector<CHotspot>::iterator mid = v.begin() + v.size() / 2;
//	vector<CHotspot> left(v.begin(), mid);
//	vector<CHotspot> right(mid, v.end());
//	left = mergeSortByDeliveryCount(left, endTime);
//	right = mergeSortByDeliveryCount(right, endTime);
//
//	return mergeByDeliveryCount(left, right, endTime);
//}

