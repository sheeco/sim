#include "PrintHelper.h"
#include "Global.h"
#include "Configuration.h"

string CPrintHelper::BLANK_LINE = NDigitString("", 80);
string CPrintHelper::HEADER_H_1 = "# ";
string CPrintHelper::TAIL_H_1 = "";
string CPrintHelper::HEADER_H_2 = "  ";
string CPrintHelper::TAIL_H_2 = "";
string CPrintHelper::HEADER_H_3 = "  > ";
string CPrintHelper::TAIL_H_3 = "";
string CPrintHelper::HEADER_TIME = "<";
string CPrintHelper::TAIL_TIME = "> ";
string CPrintHelper::HEADER_ATTRIBUTE = "[";
string CPrintHelper::TAIL_ATTRIBUTE = "] ";
string CPrintHelper::HEADER_COMMUNICATION = " >---- ";
string CPrintHelper::TAIL_COMMUNICATION = " ----> ";
string CPrintHelper::TAIL_DOING = " ... ";

string CPrintHelper::LINE_END = "";

bool CPrintHelper::newline = true;

void CPrintHelper::printToCout(string str, bool newLine)
{
	ofstream console(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_console"), ios::app);
	cout << LINE_END;
	console << endl;
	if( LINE_END == CR )
	{
		cout << BLANK_LINE << CR;
	}
	cout << str;
	console << str;
	console.close();

	if( newLine )
		LINE_END = LF, CPrintHelper::newline = true;
	else
		LINE_END = "", CPrintHelper::newline = false;
}

void CPrintHelper::flashToCout(string str)
{
	cout << LINE_END;
	if( LINE_END == CR )
	{
		cout << BLANK_LINE << CR;
	}
	cout << CR << str;

	LINE_END = CR;
	CPrintHelper::newline = false;
}

//给定该输出信息的detail等级，当该等级高于(小于)当前配置等级时正常输出，低于(大于)配置等级时flash输出

void CPrintHelper::PrintDetail(string str, int detail)
{
	if( detail <= getConfig<int>("log", "detail") )
		PrintDetailToCout(str);
	else
		FlashDetail(str);
}

