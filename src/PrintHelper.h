#pragma once

#ifndef __PRINT_HELPER_H__
#define __PRINT_HELPER_H__

#include "Helper.h"
#include "Global.h"
#include "FileHelper.h"

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

	static void printToCout(string str, bool newLine);

	inline static void flashToCout(string str)
	{
		printToCout(CR + str, false);
		LINE_END = CR;
	}
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
	inline static string toSubHeading(string str)
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
	inline static void PrintSubHeading(string str)
	{
		printToCout(toSubHeading(str), false);
	}
	inline static void PrintAttribute(string des, string value)
	{
		PrintSubHeading( toAttribute(des) + value );
		PrintNewLine();
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
	inline static void PrintDoing(string str)
	{
		PrintSubHeading(toDoing(str));
	}
	inline static void PrintDone()
	{
		printToCout(" Done", true);
	}

	inline static void Alert()
	{
		printToCout(ALERT, false);
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
	inline static void PrintDetail(string str)
	{
		printToCout(toDetail(str), true);
	}
	inline static void PrintDetail(int time, string str)
	{
		printToCout(toDetail(toTime(time) + str), true);
	}
	inline static void FlashDetail(string str)
	{
		flashToCout(toDetail(str));
	}
	inline static void FlashDetail(int time, string str)
	{
		flashToCout(toDetail( toTime(time) + str ));
	}
	inline static void PrintCommunication(int time, string from, string to, string comm)
	{
		PrintDetail(time, from + toCommunication(comm) + to);
	}
	inline static void PrintCommunication(int time, string from, string to, int nData)
	{
		PrintDetail(time, from + toCommunication( NDigitString(nData, 2) ) + to);
	}
	inline static void PrintFile(string filepath, string des)
	{
		if( ! CFileHelper::IfExists(filepath) )
			return;
		ifstream file(filepath, ios::in);
		printToCout(des, true);
		cout << file.rdbuf();
		file.close();
	}

};

#endif // __PRINT_HELPER_H__
