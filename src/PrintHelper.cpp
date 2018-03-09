#include "PrintHelper.h"
#include "Global.h"
#include "Configuration.h"

string CPrintHelper::BLANK_LINE = NDigitString("", 60);
string CPrintHelper::HEADER_H_1 = "# ";
string CPrintHelper::TAIL_H_1 = "";
string CPrintHelper::HEADER_H_2 = "  ";
string CPrintHelper::TAIL_H_2 = "";
string CPrintHelper::HEADER_H_3 = "  $ ";
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
	console << LINE_END;
	if( LINE_END == CR )
	{
		cout << BLANK_LINE << CR;
		console << BLANK_LINE << endl;
	}
	cout << str;
	console << str;
	console.close();

	if( newLine )
		LINE_END = LF, CPrintHelper::newline = true;
	else
		LINE_END = "", CPrintHelper::newline = false;
}

void CPrintHelper::PrintDetail(string str)
{
	if(getConfig<bool>("log", "detail"))
		PrintContent(str);
	else
		FlashDetail(str);
}

