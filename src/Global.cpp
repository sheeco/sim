#include "Global.h"
#include "Configuration.h"
#include "PrintHelper.h"


namespace global
{

}

void global::Exit(int code)
{
	time_t seconds;  //秒时间  
	char temp_time[65];
	seconds = time(nullptr); //获取目前秒时间  
	strftime(temp_time, 64, "%Y-%m-%d %H:%M:%S", localtime(&seconds));
	string finalTime(temp_time);

	//		// Remove entire folder if empty & exit directly
	//
	//		LPWSTR pathContent = CString( (config.log.DIR_LOG + config.log.PATH_TIMESTAMP + "*.*").c_str()).AllocSysString();
	//		LPWSTR pathFolder = CString( (config.log.DIR_LOG + config.log.PATH_TIMESTAMP).c_str()).AllocSysString();
	//		CFileFind tempFind;
	//		bool anyContentFound = (bool) tempFind.FindFile(pathContent);
	//		if( ! anyContentFound )
	//		{
	//			//去掉文件的系统和隐藏属性
	//			SetFileAttributes(pathFolder, config.log.FILE_ATTRIBUTE_NORMAL);
	//			remove( (config.log.DIR_LOG + config.log.PATH_TIMESTAMP).c_str() );
	//
	//			exit(code);
	//		}

	// Copy final file to father folder

	if( code == EFINISH )
	{
		ifstream finalInput(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_final"), ios::in);
		if( finalInput.is_open()
		   && ( !finalInput.eof() ) )
		{

			ofstream copy(getConfig<string>("log", "dir_log") + getConfig<string>("log", "file_final"), ios::app);
			char temp[310] = { '\0' };
			copy.seekp(0, ios::end);
			if( !copy.tellp() )
			{
				copy << getConfig<string>("log", "info_final") << endl;
			}
			finalInput.getline(temp, 300);  //skip head line
			finalInput.getline(temp, 300);
			string finalInfo(temp);
			copy << finalInfo << getConfig<string>("log", "info_log") << TAB << "@" << finalTime << endl;
			copy.close();
		}
	}

	// Print final time

	if( code >= EFINISH )
	{
		fstream final(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_final"), ios::app | ios::in);
		final << getConfig<string>("log", "info_log") << TAB << "@" << finalTime << endl;
		final.close();
	}

	// Remove name prefix '.' in front of log folder

	if( code == EFINISH )
	{

		if( _access(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str(), 02) == 0
		   && ( getConfig<string>("log", "path_timestamp").find(".") != getConfig<string>("log", "path_timestamp").npos ) )   //if writeable & '.' found in filename
		{
			string newPathLog = getConfig<string>("log", "path_timestamp").substr(1, getConfig<string>("log", "path_timestamp").npos);
			if( _access(( getConfig<string>("log", "dir_log") + newPathLog ).c_str(), 00) != 0 )  //if no collision
			{
				rename(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str(), ( getConfig<string>("log", "dir_log") + newPathLog ).c_str());
				updateConfig<string>("log", "path_timestamp", newPathLog);
			}
		}

		// Unhide folder
		LPWSTR wstr = CString(( getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") ).c_str()).AllocSysString();
		int attr = GetFileAttributes(wstr);
		if( ( attr & FILE_ATTRIBUTE_HIDDEN ) == FILE_ATTRIBUTE_HIDDEN )
		{
			SetFileAttributes(wstr, attr & ~FILE_ATTRIBUTE_HIDDEN);
		}
	}

	// Alert & Pause
	if( code > EFINISH )
	{
		CPrintHelper::Alert();
		_PAUSE_;
	}

	exit(code);
}

void global::Exit(int code, string error)
{
	ofstream errorFile(getConfig<string>("log", "dir_log") + getConfig<string>("log", "path_timestamp") + getConfig<string>("log", "file_error"), ios::app);
	errorFile << error << endl << endl;
	errorFile.close();

	Exit(code);
}
