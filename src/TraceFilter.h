#pragma once

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define OVERFLOW -1

#define getpch(type, n) (type*)malloc(n * sizeof(type))

/*
	用于存储文件中的数据的数据结构
	分别 对应 时间，X坐标和Y坐标
*/
typedef struct data {
	long double tcollTime;
	long double coordinateX;
	long double coordinateY;
} data;

/*  直线的数据结构
	直线的形式是 a*x + b*y + c =0
	数据结构中存储 a, b, c
*/
typedef struct line {
	long double a;
	long double b;
	long double c;
}line;


typedef int Status;
typedef data SElemType;

/*
	表示栈的元素
*/
typedef struct StackNode {
	SElemType data;
	struct StackNode *next;
} *LinkStackPtr;

/*
	栈的数据结构
*/
typedef struct LinkStack {
	LinkStackPtr top;
	int count;
} *PLinkStack;

/*
	以下是我之前做数据结构时写的关于链式栈的函数，
	我拿过来修改了一下，就用了。
	有些方法保留是为了测试这个栈
	函数前标有“#”的是程序中真正用到的函数
*/
void Menu();
Status visit(SElemType e);//打印该元素 
//#
Status InitLStack(PLinkStack *S);    //初始化一个空栈
//#
Status DestroyLStack(PLinkStack *S);    //销毁栈 
//#
Status ClearLStack(PLinkStack S);    //清空栈 
Status LinkStackEmpty(PLinkStack S);    //判断栈是否为空
//#
int LStackLength(PLinkStack S);    //栈长
//#
Status Push(PLinkStack S, SElemType *e);    //入栈
//#
Status Pop(PLinkStack S, SElemType *e);    //出栈
//#
Status GetTop(PLinkStack S, SElemType *e);    //获取栈顶元素
Status StackTraverse(PLinkStack S, Status(*visit)(SElemType e));    //从栈顶开始遍历 


data *loadData();						//读入数据
int countRows();						//计算文件中数据的行数
line getLine(data p1, data p2);			//根据两点坐标得到过两点的直线
double countDistance(data a, data b);	//计算两点间的距离
//判定 条件一 d 与 r的关系
Status firstTerm(data a, data b);		
//判定 条件二 h 与 w的关系
Status secondTerm(PLinkStack flight, data currentPoint); 
//将目标点写入文件
Status writeFile(PLinkStack flight);

int testTraceFilter()
{
	/*（针对一份文件）
	1、先读入数据
	2、对数据点循环
	3、写入目标点
	*/
	//	printf("start");
	data *points = loadData();
	int length = countRows();
	//	int length = sizeof(points) / sizeof(data*);
	//	printf("%d", lenght);
	int i = 0;
	PLinkStack flight;
	InitLStack(&flight);
	for( ; i < length; i++ )
	{
		//Push(flight, &points[i]);
		if( flight->count > 1 )
		{
			SElemType top;
			if( GetTop(flight, &top) )
			{
				if( firstTerm(top, points[i]) )
				{
					/*
					满足第一个条件d>r为真
					*/
					if( secondTerm(flight, points[i]) )
					{
						/*
						满足第一个条件之后进入第二个条件
						h>w 为真,满足一个flight的条件,
						对当前flight进行存储,之后对flight清空
						并将当前的point放入flight
						*/
						writeFile(flight);
						LinkStackEmpty(flight);
						Push(flight, &points[i]);
					}
					else
					{
						/*
						h<w,将当前point放入当前的flight中
						*/
						Push(flight, &points[i]);
					}
				}
				else
				{
					writeFile(flight);
					LinkStackEmpty(flight);
					Push(flight, &points[i]);
				}
			}
		}

		//printf("  %Lf\t %Lf\t  %Lf\t", points[i].tcollTime, points[i].coordinateX, points[i].coordinateY);

		//printf("\n");
	}
	DestroyLStack(&flight);
	return 0;
}


