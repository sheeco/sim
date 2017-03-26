#include <stdio.h>
#include <math.h>
#include <string.h>
#include "TraceFilter.h"

void Menu() {
	printf("select 1------------------初始化栈\n");
	printf("select 2--------------------销毁栈\n");
	printf("select 3--------------------清空栈\n");
	printf("select 4----------------栈是否为空\n");
	printf("select 5----------------------栈长\n");
	printf("select 6--------------返回栈顶元素\n");
	printf("select 7----------插入新的栈顶元素\n");
	printf("select 8------------删除的栈顶元素\n");
	printf("select 9------依次访问栈中每个元素\n");
	printf("select 0----------------------退出\n");
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
	if (S->top)/*栈存在且非空*/
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
	从文件中读入数据
	*/
	int row = countRows();
	data *points;
	points = getpch(data, row);
	int i = 0;
	int j = 0;
	FILE *fp;
	/*
	目前是对单文件的测试，如果需要批量处理，这里需要进行改动
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
	计算文件的行数
	*/
	FILE *fp;
	int n = 0;
	int ch;
	//这里需要改，在函数上添加一个参数
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
	//n-1是因为每份文件最后多了一个\n，而算法是根据\n统计的
	return n - 1;
}

double countDistance(data a, data b) {
	/*
	计算两点间的距离
	*/
	long double distance = sqrt((a.coordinateX - b.coordinateX)*(a.coordinateX - b.coordinateX) + (a.coordinateY - b.coordinateY)*(a.coordinateY - b.coordinateY));
	return distance;
}

Status firstTerm(data a, data b) {
	/*
	对需要满足的条件一 的判定
	这里的r是自己定义的，记得自己改，
	secondTerm中的w也同样也可以考虑改为全局变量
	*/
	long double r = 0.005;
	if (countDistance(a, b) > r) {
		return OK;
	}
	return ERROR;
}

long double countDistanceOfPointLine(line fLine, data p) {
	/*
	计算 点到线的距离
	*/
	return fabsl((fLine.a * p.coordinateX + fLine.b * p.coordinateY + fLine.c) / (sqrtl(powl(fLine.a, 2) + powl(fLine.b, 2))));
}

line getLine(data p1, data p2) {
	/*
	根据两点得到一条过这两点的直线，返回该直线
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
	分别存储驻留点数据 和 途经点数据
	如果需要一次性处理文件并对每个*.trace中的数据分别进行储存的话，需要进行改动
	这里是把处理的一份*.trace中的驻留点和途经点分别全部放在了一起
	这段程序分为三段，看下面的注释即可理解
	*/

	/*
	第一段：将得到的flight拿出来，放入一个数组中
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
	第二段：将驻留点写入文件
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
	第三段：将途经点写入文件
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
	对 需要满足的第二个条件 进行判断
	分为三个阶段，看下面的注释即可
	*/

	/*
	第一阶段，将flight中的point放入一个数组中
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
	根据flight的首尾计算出 一条直线。
	a*x + b*y + c = 0
	直线数据结构 存的是 a, b, c
	*/
	line lineFlight = getLine(temFlight[0], temFlight[flight->count]);
	/*
	第三阶段：进行判断
	对中间点（也就是途经点）循环，计算点到线的距离
	得到他们的和与 w 进行比较
	这里记得更改w
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


