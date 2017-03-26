#include <stdio.h>
#include <math.h>
#include <string.h>
#include "TraceFilter.h"

void Menu() {
	printf("select 1------------------��ʼ��ջ\n");
	printf("select 2--------------------����ջ\n");
	printf("select 3--------------------���ջ\n");
	printf("select 4----------------ջ�Ƿ�Ϊ��\n");
	printf("select 5----------------------ջ��\n");
	printf("select 6--------------����ջ��Ԫ��\n");
	printf("select 7----------�����µ�ջ��Ԫ��\n");
	printf("select 8------------ɾ����ջ��Ԫ��\n");
	printf("select 9------���η���ջ��ÿ��Ԫ��\n");
	printf("select 0----------------------�˳�\n");
}

Status visit(SElemType e) {
	printf("  %Lf\t %Lf\t  %Lf\t", e.tcollTime, e.coordinateX, e.coordinateY);
	//printf("%c\t", e);
	return OK;
}//visit

Status InitLStack(PLinkStack *S) {
	*S = (PLinkStack)malloc(sizeof(struct LinkStack));
	if (!S)        return ERROR;
	(*S)->top = NULL;
	(*S)->count = 0;
	return OK;
}//InitLStack

Status DestroyLStack(PLinkStack *S)
{
	ClearLStack(*S);
	free(*S);
	return OK;
}//DestroyLStack

Status ClearLStack(PLinkStack S)
{
	LinkStackPtr p;
	while (S->top) {
		p = S->top;
		S->top = S->top->next;
		S->count--;
		free(p);
	}
	return OK;
}//ClearStack 

Status LinkStackEmpty(PLinkStack S)
{
	if (S->top)/*ջ�����ҷǿ�*/
		return FALSE;
	else
		return TRUE;
}//LStackEmpty

int LStackLength(PLinkStack S) {
	return S->count;
}

Status Push(PLinkStack S, SElemType *e) {
	LinkStackPtr s = (LinkStackPtr)malloc(sizeof(struct StackNode));
	s->data = *e;
	s->next = S->top;
	S->top = s;
	S->count++;
	return OK;
}//Push

Status Pop(PLinkStack S, SElemType *e) {
	LinkStackPtr p;
	if (!GetTop(S, e))    return ERROR;
	p = S->top;
	S->top = S->top->next;
	S->count--;
	free(p);
	return OK;
}//Pop

Status GetTop(PLinkStack S, SElemType *e)
{
	if (!S->top)return ERROR;
	*e = S->top->data;
	return OK;
}//GEtTop

Status StackTraverse(PLinkStack S, Status(*visit)(SElemType e)) {
	LinkStackPtr p;
	p = S->top;
	while (p) {
		visit(p->data);
		p = p->next;
	}
	printf("\n");
	return OK;
}//StackTraverse

data *loadData() {
	/*
	���ļ��ж�������
	*/
	int row = countRows();
	data *points;
	points = getpch(data, row);
	int i = 0;
	int j = 0;
	FILE *fp;
	/*
	Ŀǰ�ǶԵ��ļ��Ĳ��ԣ������Ҫ��������������Ҫ���иĶ�
	*/
	fp = fopen("1.trace", "r");
	if (!fp) {
		fprintf(stderr, "open file 1.c error! %s\n", strerror(errno));
		return points;
	}
	for (; i < row; i++) {
		fscanf(fp, "  %Lf\t %Lf\t  %Lf\t", &points[i].tcollTime, &points[i].coordinateX, &points[i].coordinateY);
		//		printf("  %Lf\t %Lf\t  %Lf\t\n", points[i].tcollTime, points[i].coordinateX, points[i].coordinateY);
	}
	return points;
}

int countRows() {
	/*
	�����ļ�������
	*/
	FILE *fp;
	int n = 0;
	int ch;
	//������Ҫ�ģ��ں��������һ������
	fp = fopen("1.trace", "r");
	if (!fp) {
		fprintf(stderr, "open file 1.c error! %s\n", strerror(errno));
	}

	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\n') {
			n++;
		}
	}

	fclose(fp);
	//printf("%d\n", n);
	//n-1����Ϊÿ���ļ�������һ��\n�����㷨�Ǹ���\nͳ�Ƶ�
	return n - 1;
}

double countDistance(data a, data b) {
	/*
	���������ľ���
	*/
	long double distance = sqrt((a.coordinateX - b.coordinateX)*(a.coordinateX - b.coordinateX) + (a.coordinateY - b.coordinateY)*(a.coordinateY - b.coordinateY));
	return distance;
}

Status firstTerm(data a, data b) {
	/*
	����Ҫ���������һ ���ж�
	�����r���Լ�����ģ��ǵ��Լ��ģ�
	secondTerm�е�wҲͬ��Ҳ���Կ��Ǹ�Ϊȫ�ֱ���
	*/
	long double r = 0.005;
	if (countDistance(a, b) > r) {
		return OK;
	}
	return ERROR;
}

long double countDistanceOfPointLine(line fLine, data p) {
	/*
	���� �㵽�ߵľ���
	*/
	return fabsl((fLine.a * p.coordinateX + fLine.b * p.coordinateY + fLine.c) / (sqrtl(powl(fLine.a, 2) + powl(fLine.b, 2))));
}

line getLine(data p1, data p2) {
	/*
	��������õ�һ�����������ֱ�ߣ����ظ�ֱ��
	*/
	line line;
	if (p1.coordinateX == p2.coordinateY) {
		line.a = 1;
		line.b = 0;
		line.c = -p1.coordinateX;
		return line;
	}
	else if (p1.coordinateY == p2.coordinateY) {
		line.a = 0;
		line.b = 1;
		line.c = -p1.coordinateY;
		return line;
	}
	else {
		line.a = (p2.coordinateY - p1.coordinateY) / (p2.coordinateX - p1.coordinateX);
		line.b = -1;
		line.c = p1.coordinateY - line.a * p1.coordinateX;
		return line;
	}
}

Status writeFile(PLinkStack flight) {
	/*
	�ֱ�洢פ�������� �� ;��������
	�����Ҫһ���Դ����ļ�����ÿ��*.trace�е����ݷֱ���д���Ļ�����Ҫ���иĶ�
	�����ǰѴ����һ��*.trace�е�פ�����;����ֱ�ȫ��������һ��
	��γ����Ϊ���Σ��������ע�ͼ������
	*/

	/*
	��һ�Σ����õ���flight�ó���������һ��������
	*/
	data *temFlight = getpch(data, flight->count);
	LinkStackPtr p;
	p = flight->top;
	int i = 0;
	for (i = 0; i < flight->count; i++) {
		temFlight[i] = p->data;
		p = p->next;
	}
	/*
	�ڶ��Σ���פ����д���ļ�
	*/
	FILE *fpOfPark;
	fpOfPark = fopen("park.txt", "a+");
	if (!fpOfPark) {
		fprintf(stderr, "open file park.txt error! %s\n", strerror(errno));
		return ERROR;
	}
	fprintf(fpOfPark, "  %Lf\t %Lf\t  %Lf\t\n  %Lf\t %Lf\t  %Lf\t\n\n", temFlight[flight->count - 1].tcollTime,
		temFlight[flight->count - 1].coordinateX, temFlight[flight->count - 1].coordinateY,
		temFlight[0].tcollTime, temFlight[0].coordinateX, temFlight[0].coordinateY);
	fclose(fpOfPark);
	/*
	�����Σ���;����д���ļ�
	*/
	FILE *fpOfWay;
	fpOfWay = fopen("way.txt", "a+");
	if (!fpOfWay) {
		fprintf(stderr, "open file park.txt error! %s\n", strerror(errno));
		return ERROR;
	}
	int index = flight->count - 2;
	for (; index > 0; index--) {
		fprintf(fpOfWay, "  %Lf\t %Lf\t  %Lf\t\n", temFlight[index].tcollTime,
			temFlight[index].coordinateX, temFlight[index].coordinateY);
	}
	fputc('\n', fpOfWay);
	fclose(fpOfWay);

	return OK;
}

Status secondTerm(PLinkStack flight, data currentPoint) {
	/*
	�� ��Ҫ����ĵڶ������� �����ж�
	��Ϊ�����׶Σ��������ע�ͼ���
	*/

	/*
	��һ�׶Σ���flight�е�point����һ��������
	*/
	data *temFlight = getpch(data, flight->count + 1);
	LinkStackPtr p;
	p = flight->top;
	int i = 0;
	for (i = 0; i < flight->count; i++) {
		temFlight[i] = p->data;
		p = p->next;
	}
	temFlight[flight->count] = currentPoint;
	/*
	����flight����β����� һ��ֱ�ߡ�
	a*x + b*y + c = 0
	ֱ�����ݽṹ ����� a, b, c
	*/
	line lineFlight = getLine(temFlight[0], temFlight[flight->count]);
	/*
	�����׶Σ������ж�
	���м�㣨Ҳ����;���㣩ѭ��������㵽�ߵľ���
	�õ����ǵĺ��� w ���бȽ�
	����ǵø���w
	*/
	long double w = 0.5;
	long double hSum = 0;
	for (i = 1; i < flight->count; i++) {
		hSum += countDistanceOfPointLine(lineFlight, temFlight[i]);
	}
	if (hSum > w) {
		return OK;
	}
	else {
		return ERROR;
	}
}


