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
		ifstream finalInput(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_FINAL, ios::in);
		if( finalInput.is_open()
		   && ( !finalInput.eof() ) )
		{

			ofstream copy(configs.log.DIR_LOG + configs.log.FILE_FINAL, ios::app);
			char temp[310] = { '\0' };
			copy.seekp(0, ios::end);
			if( !copy.tellp() )
			{
				copy << configs.log.INFO_FINAL;
			}
			finalInput.getline(temp, 300);  //skip head line
			finalInput.getline(temp, 300);
			string finalInfo(temp);
			copy << finalInfo << configs.log.INFO_LOG << TAB << "@" << finalTime << endl;
			copy.close();
		}
	}

	// Print final time

	if( code >= EFINISH )
	{
		fstream final(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_FINAL, ios::app | ios::in);
		final << configs.log.INFO_LOG << TAB << "@" << finalTime << endl;
		final.close();
	}

	// Remove name prefix '.' in front of log folder

	if( code == EFINISH )
	{

		if( _access(( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP ).c_str(), 02) == 0
		   && ( configs.log.PATH_TIMESTAMP.find(".") != configs.log.PATH_TIMESTAMP.npos ) )   //if writeable & '.' found in filename
		{
			string newPathLog = configs.log.PATH_TIMESTAMP.substr(1, configs.log.PATH_TIMESTAMP.npos);
			if( _access(( configs.log.DIR_LOG + newPathLog ).c_str(), 00) != 0 )  //if no collision
			{
				rename(( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP ).c_str(), ( configs.log.DIR_LOG + newPathLog ).c_str());
				configs.log.PATH_TIMESTAMP = newPathLog;
			}
		}

		// Unhide folder
		LPWSTR wstr = CString(( configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP ).c_str()).AllocSysString();
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
	ofstream errorFile(configs.log.DIR_LOG + configs.log.PATH_TIMESTAMP + configs.log.FILE_ERROR, ios::app);
	errorFile << error << endl << endl;
	errorFile.close();

	Exit(code);
}
