#pragma once

#ifndef __PRINT_HELPER_H__
#define __PRINT_HELPER_H__

#include "Global.h"
#include "Process.h"

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

	static void flashToCout(string str);
	// TODO: call printToFile() for logging in PrintInfo()
	inline static void printToFile(string filepath, string str, bool newLine)
	{
		ofstream file(filepath, ios::app);
		file << str;
		if( newLine )
			file << endl;
		file.close();
	}
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

	inline static void PrintToCurrentLine(string str)
	{
		printToCout(str, false);
	}
	inline static void PrintBrief(string str, bool newline)
	{
		printToCout(toBrief(str), newline);
	}
	inline static void PrintDetailToCout(string str)
	{
		printToCout(toDetail(str), true);
	}
	inline static void FlashDetail(string str)
	{
		flashToCout(toDetail(str));
	}


public:

	CPrintHelper::CPrintHelper() {};
	CPrintHelper::~CPrintHelper() {};

	inline static void PrintNewLine()
	{
		printToCout("", true);
	}
	inline static void PrintHeading(string str)
	{
		printToCout(toHeading(str), true);
	}
	inline static void PrintHeading(int time, string str)
	{
		PrintHeading(toTime(time) + str);
	}
	inline static void PrintAttribute(string des, string value)
	{
		PrintBrief(toAttribute(des) + value, true);
	}
	inline static void PrintAttribute(string des, double value)
	{
		PrintAttribute(des, STRING(NDigitFloat(value, 2)));
	}
	inline static void PrintPercentage(string des, double value)
	{
		double percentage = NDigitFloat(value * 100, 1);
		PrintAttribute(des, STRING(percentage) + " %");
	}
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

	inline static void Alert()
	{
		printToCout(ALERT, false);
	}
	inline static bool Warn(string error)
	{
		PrintNewLine();
		printToCout("[Warning]" + error, true);
		Alert();
		_PAUSE_;
		return true;
	}
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
	inline static void PrintTestError(string error)
	{
		printToCout("Error caught correctly @ " + error, true);
	}
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
	inline static void PrintCommunicationSkipped(int time, string from, string to)
	{
		PrintDetail(time, from + toCommunication("skip") + to, 2);
	}
	inline static void PrintCommunication(int time, string from, string to, int nData)
	{
		PrintDetail(time, from + toCommunication( NDigitString(nData, 2) ) + to, 1);
	}
	static void PrintFile(string filepath, string des);

};

#endif // __PRINT_HELPER_H__
