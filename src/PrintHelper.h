#pragma once

#ifndef __PRINT_HELPER_H__
#define __PRINT_HELPER_H__

#include "Global.h"
#include "Process.h"

//控制台/文件输出的辅助类
class CPrintHelper :
	virtual public CHelper
{
private:
	static string BLANK_LINE;
	static string HEADER_H_1;
	static string TAIL_H_1;
	static string HEADER_H_2;
	static string TAIL_H_2;
	static string HEADER_H_3;
	static string TAIL_H_3;
	static string HEADER_TIME;
	static string TAIL_TIME;
	static string HEADER_ATTRIBUTE;
	static string TAIL_ATTRIBUTE;
	static string HEADER_COMMUNICATION;
	static string TAIL_COMMUNICATION;
	static string TAIL_DOING;

	static string LINE_END;

	static bool newline;

	static void printToCout(string str, bool newLine);

	//在控制台中闪现
	static void flashToCout(string str);

	//输出到文件
	// TODO: call printToFile() for logging in PrintInfo()
	inline static void printToFile(string filepath, string str, bool newLine)
	{
		ofstream file(filepath, ios::app);
		file << str;
		if( newLine )
			file << endl;
		file.close();
	}

	/** 格式转换辅助函数 **/

	inline static string toHeading(string str)
	{
		return HEADER_H_1 + str + TAIL_H_1;
	}
	inline static string toBrief(string str)
	{
		return HEADER_H_2 + str + TAIL_H_2;
	}
	inline static string toDetail(string str)
	{
		return HEADER_H_3 + str + TAIL_H_3;
	}
	inline static string toTime(int time)
	{
		return HEADER_TIME + STRING(time) + TAIL_TIME;
	}
	inline static string toAttribute(string str)
	{
		return HEADER_ATTRIBUTE + str + TAIL_ATTRIBUTE;
	}
	inline static string toDoing(string str)
	{
		return str + TAIL_DOING;
	}
	inline static string toCommunication(string comm)
	{
		return HEADER_COMMUNICATION + comm + TAIL_COMMUNICATION;
	}

	//直接输出到当前行
	inline static void PrintToCurrentLine(string str)
	{
		printToCout(str, false);
	}
	//输出简要信息
	inline static void PrintBrief(string str, bool newline)
	{
		printToCout(toBrief(str), newline);
	}
	//输出细节信息
	inline static void PrintDetailToCout(string str)
	{
		printToCout(toDetail(str), true);
	}
	//闪现细节信息
	inline static void FlashDetail(string str)
	{
		flashToCout(toDetail(str));
	}


public:

	CPrintHelper::CPrintHelper() {};
	CPrintHelper::~CPrintHelper() {};

	//换行
	inline static void PrintNewLine()
	{
		printToCout("", true);
	}

	//打印标题行
	inline static void PrintHeading(string str)
	{
		printToCout(toHeading(str), true);
	}
	inline static void PrintHeading(int time, string str)
	{
		PrintHeading(toTime(time) + str);
	}

	//打印变量
	inline static void PrintAttribute(string des, string value)
	{
		PrintBrief(toAttribute(des) + value, true);
	}
	inline static void PrintAttribute(string des, double value)
	{
		PrintAttribute(des, STRING(NDigitFloat(value, 2)));
	}

	//打印百分比变量
	inline static void PrintPercentage(string des, double value)
	{
		double percentage = NDigitFloat(value * 100, 1);
		PrintAttribute(des, STRING(percentage) + " %");
	}

	/** 打印正在进行/完成信息 **/

	inline static void PrintDoingHeading(string str)
	{
		str = toDoing(str);
		PrintHeading(str);
	}
	inline static void PrintDoing(string str)
	{
		str = toDoing(str);
		if(newline)
			PrintBrief(str, false);
		else
			PrintToCurrentLine(str);
	}
	inline static void PrintDone()
	{
		PrintToCurrentLine("Done");
		PrintNewLine();
	}

	//输出警示音
	inline static void Alert()
	{
		printToCout(ALERT, false);
	}
	//警告信息
	inline static bool Warn(string error)
	{
		PrintNewLine();
		printToCout("[Warning]" + error, true);
		Alert();
		_PAUSE_;
		return true;
	}
	//错误信息
	inline static void PrintError(string error)
	{
		PrintNewLine();
		printToCout("Error @ " + error, true);
	}
	inline static void PrintError(pair<int, string> &pairError)
	{
		PrintError("Error " + STRING(pairError.first) + " @ " + pairError.second);
	}
	inline static void PrintError(exception ex)
	{
		PrintError("Uncaught Error : " + string(ex.what()));
	}
	//测试中正确捕获的错误信息
	inline static void PrintTestError(string error)
	{
		printToCout("Error caught correctly @ " + error, true);
	}

	//打印简要信息
	inline static void PrintBrief(string str)
	{
		PrintBrief(str, true);
	}
	inline static void PrintBrief(int time, string str)
	{
		PrintBrief(toTime(time) + str);
	}

	//给定该输出信息的detail等级，当该等级高于(小于)当前配置等级时正常输出，低于(大于)配置等级时flash输出
	static void PrintDetail(string str, int detail);
	inline static void PrintDetail(int time, string str, int detail)
	{
		PrintDetail(toTime(time) + str, detail);
	}

	//打印通信结果
	inline static void PrintCommunicationSkipped(int time, string from, string to)
	{
		PrintDetail(time, from + toCommunication("skip") + to, 2);
	}
	inline static void PrintCommunication(int time, string from, string to, int nData)
	{
		PrintDetail(time, from + toCommunication( NDigitString(nData, 2) ) + to, 1);
	}

	//将给定文件的全文打印到控制台
	static void PrintFile(string filepath, string des);

};

#endif // __PRINT_HELPER_H__
