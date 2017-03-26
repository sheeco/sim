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
	���ڴ洢�ļ��е����ݵ����ݽṹ
	�ֱ� ��Ӧ ʱ�䣬X�����Y����
*/
typedef struct data {
	long double tcollTime;
	long double coordinateX;
	long double coordinateY;
} data;

/*  ֱ�ߵ����ݽṹ
	ֱ�ߵ���ʽ�� a*x + b*y + c =0
	���ݽṹ�д洢 a, b, c
*/
typedef struct line {
	long double a;
	long double b;
	long double c;
}line;


typedef int Status;
typedef data SElemType;

/*
	��ʾջ��Ԫ��
*/
typedef struct StackNode {
	SElemType data;
	struct StackNode *next;
} *LinkStackPtr;

/*
	ջ�����ݽṹ
*/
typedef struct LinkStack {
	LinkStackPtr top;
	int count;
} *PLinkStack;

/*
	��������֮ǰ�����ݽṹʱд�Ĺ�����ʽջ�ĺ�����
	���ù����޸���һ�£������ˡ�
	��Щ����������Ϊ�˲������ջ
	����ǰ���С�#�����ǳ����������õ��ĺ���
*/
void Menu();
Status visit(SElemType e);//��ӡ��Ԫ�� 
//#
Status InitLStack(PLinkStack *S);    //��ʼ��һ����ջ
//#
Status DestroyLStack(PLinkStack *S);    //����ջ 
//#
Status ClearLStack(PLinkStack S);    //���ջ 
Status LinkStackEmpty(PLinkStack S);    //�ж�ջ�Ƿ�Ϊ��
//#
int LStackLength(PLinkStack S);    //ջ��
//#
Status Push(PLinkStack S, SElemType *e);    //��ջ
//#
Status Pop(PLinkStack S, SElemType *e);    //��ջ
//#
Status GetTop(PLinkStack S, SElemType *e);    //��ȡջ��Ԫ��
Status StackTraverse(PLinkStack S, Status(*visit)(SElemType e));    //��ջ����ʼ���� 


data *loadData();						//��������
int countRows();						//�����ļ������ݵ�����
line getLine(data p1, data p2);			//������������õ��������ֱ��
double countDistance(data a, data b);	//���������ľ���
//�ж� ����һ d �� r�Ĺ�ϵ
Status firstTerm(data a, data b);		
//�ж� ������ h �� w�Ĺ�ϵ
Status secondTerm(PLinkStack flight, data currentPoint); 
//��Ŀ���д���ļ�
Status writeFile(PLinkStack flight);

int testTraceFilter()
{
	/*�����һ���ļ���
	1���ȶ�������
	2�������ݵ�ѭ��
	3��д��Ŀ���
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
					�����һ������d>rΪ��
					*/
					if( secondTerm(flight, points[i]) )
					{
						/*
						�����һ������֮�����ڶ�������
						h>w Ϊ��,����һ��flight������,
						�Ե�ǰflight���д洢,֮���flight���
						������ǰ��point����flight
						*/
						writeFile(flight);
						LinkStackEmpty(flight);
						Push(flight, &points[i]);
					}
					else
					{
						/*
						h<w,����ǰpoint���뵱ǰ��flight��
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


